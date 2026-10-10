/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.29.2025
/   @author: janpaw
/   @brief: Controls servo motors
*/
#include <cmath>
#include <Wire.h>

#include <Logger/Logger.hpp>
#include <MotorControlService/MotorControl.hpp>
#include <MotorControlService/MotorControlMessage.hpp>

using namespace services;
using namespace robot;

// helpers
namespace
{
    float clampf(float x, float lo, float hi)
    {
        return (x < lo) ? lo : (x > hi) ? hi : x;
    }

    /**
     * @brief Converts a normalized control value (u ∈ [-1, 1], where 0 is
     * neutral) into a servo pulse width in microseconds.
     *
     * Handles asymmetric ranges on each side of zero and ensures
     * the resulting pulse never exceeds physical joint limits.
     */
    float mapUtoPulseUs(float u, const ServoConfig &cfg)
    {
        u = clampf(u, -1.0f, 1.0f);

        const float zeroUs = static_cast<float>(cfg.zeroUs);
        const float negLimitUs = static_cast<float>(cfg.negLimitUs);
        const float posLimitUs = static_cast<float>(cfg.posLimitUs);

        // precompute distances from neutral position
        // servo may have asymmetric travel on negative and positive sides
        const float negRange = fabsf(zeroUs - negLimitUs);
        const float posRange = fabsf(posLimitUs - zeroUs);

        float pulse = zeroUs;

        // maps normalized input toward the corresponding mechanical limit
        // 'dir' resolves whether pulse width increases or ecreases depending on
        // servo configuration
        if (u < 0.0f)
        {
            const float dir = (negLimitUs >= zeroUs) ? 1.0f : -1.0f;
            pulse = zeroUs + dir * (-u) * negRange;
        }
        else
        {
            const float dir = (posLimitUs >= zeroUs) ? 1.0f : -1.0f;
            pulse = zeroUs + dir * u * posRange;
        }

        // hard clamp to physical limits
        const float lo = fminf(zeroUs, fminf(negLimitUs, posLimitUs));
        const float hi = fmaxf(zeroUs, fmaxf(negLimitUs, posLimitUs));
        return clampf(pulse, lo, hi);
    }

    float clampPulse(float us, const ServoConfig &cfg)
    {
        const float negLimitUs = static_cast<float>(cfg.negLimitUs);
        const float posLimitUs = static_cast<float>(cfg.posLimitUs);

        LOG_TRC("Neg limit %f, pos limit: %f", negLimitUs, posLimitUs);
        if(us < cfg.negLimitUs)
        {
            us = cfg.negLimitUs;
        }
        else if(us > cfg.posLimitUs)
        {
            us = cfg.posLimitUs;
        }
        return us;
    }
} // namespace

MotorControl::MotorControl(const char *serviceName,
                           configSTACK_DEPTH_TYPE stackDepth,
                           const UBaseType_t servicePriority,
                           StackType_t *stackBuffer, StaticTask_t *taskBuffer,
                           PCA9685 &driver)
    : ServiceTemplate(serviceName, stackDepth, servicePriority, stackBuffer,
                      taskBuffer),
    m_driver(driver)
{
    initServoStates();

    
    m_cmdQueueSingle = xQueueCreateStatic(kQueueLength, sizeof(CommandSingle),
                                    m_cmdQueueSingleStorage, &m_cmdQueueSingleBuffer);

    m_cmdQueueMulti = xQueueCreateStatic(kQueueLength, sizeof(CommandMulti),
                            m_cmdQueueMultiStorage, &m_cmdQueueMultiBuffer);
}

void MotorControl::task()
{
    // we refresh position once per PCA PWM cycle
    // (it's not mandatory)
    // const TickType_t period = pdMS_TO_TICKS(kControlPeriodMs);
    // TODO: PR JP temporary set to 500, figure out what value we can set here
    LOG_TRC("Hey from motor control!");

    if (!m_driver.init())
    {
        LOG_WRN("Failed to initialize PCA9685\r\n");
    }
    const TickType_t period = pdMS_TO_TICKS(3);
    for (;;)
    {
        TickType_t lastWake = xTaskGetTickCount();
        // LOG_TRC("Motor control - runs");
        processCommands();
        updateInterpolation();
        updateAllOutputs();

        vTaskDelayUntil(&lastWake, period);
        // vTaskDelay(5); // TODO for tests vTaskDelayUnitl doesn't work for some reason
    }
}

void MotorControl::initServoStates()
{
    for (std::size_t i = 0; i < jointCount(); ++i)
    {
        JointId id = static_cast<JointId>(i);
        const auto &cfg = getServoConfig(id);

        auto &servo = m_servos[i];
        servo.cfg = &cfg;

        // TODO: JP, set to home position? zero?
        servo.currentPulseUs = static_cast<float>(cfg.zeroUs);
        servo.targetPulseUs = static_cast<float>(cfg.zeroUs);
        servo.velUsPerS = 0.0f; // TODO: JP, smooth homing movement
        servo.timeRemainingS = 0.0f;
    }
}

bool MotorControl::moveJoint(JointId joint, float targetU, u16 timeToReachMs)
{
    // TODO: JP, whoever hands the joint id to this funcion should validate it
    CommandSingle cmd{joint, targetU, timeToReachMs};

    // TODO: JP, non-blocking - caller gets false if queue full (for now?)
    if (xQueueSend(m_cmdQueueSingle, &cmd, 0) != pdPASS)
        return false;

    return true;
}

bool MotorControl::moveJoints(const std::array<float, robot::jointCount()>& targetsUs, u16 timeToReachMs)
{
    CommandMulti cmd{targetsUs, timeToReachMs};
    LOG_TRC("Inisde move Joints: 4: %f, 5: %f", cmd.targetUs[4], cmd.targetUs[5]);
    if (xQueueSend(m_cmdQueueMulti, &cmd, 0) != pdPASS)
        return false;
    
    return true;
}

void MotorControl::startMove(JointId joint, float targetUs, u16 timeToReachMs)
{
    auto idx = static_cast<std::size_t>(joint);
    auto &servo = m_servos[idx];
    const auto &cfg = *servo.cfg;

    float finalPulse = clampPulse(targetUs, cfg);
    LOG_TRC("After clamp: %d, %f", joint, finalPulse);
    LOG_TRC("Before clamp: %d, %f", joint, targetUs);
    servo.targetPulseUs = finalPulse;

    if (timeToReachMs == 0)
    {
        // snap - no interpolation
        servo.currentPulseUs = servo.targetPulseUs;
        servo.velUsPerS = 0.0f;
        servo.timeRemainingS = 0.0f;
        return;
    }

    // TODO: JP, clamp velDegPerS to cfg.maxSpeedDegPerS once it's added to
    // ServoConfig

    // calculate speed
    const float timeToReachS = static_cast<float>(timeToReachMs) / 1000.0f;
    const float diff = servo.targetPulseUs - servo.currentPulseUs;
    servo.velUsPerS = diff / timeToReachS;

    servo.timeRemainingS = timeToReachS;
}

void MotorControl::startMoves(const CommandMulti& cmd)
{
    u8 i = 0;
    float finalPulse = 0;

    for(auto x : cmd.targetUs)
    {
        // skip for targets that requested 0
        if(x == 0)
        {
            LOG_DBG("Target %d, skipped", i);
            i++;
            continue;
        }
        finalPulse = clampPulse(x, *(m_servos[i].cfg));
        m_servos[i].targetPulseUs = finalPulse;

        if (cmd.timeToReachMs == 0)
        {
            // snap - no interpolation
            m_servos[i].currentPulseUs = m_servos[i].targetPulseUs;
            m_servos[i].velUsPerS = 0.0f;
            m_servos[i].timeRemainingS = 0.0f;
            LOG_DBG("Target %d, set to: %f", i, finalPulse);
            i++;
        }
        else
        {
            const float timeToReachS = static_cast<float>(cmd.timeToReachMs) / 1000.0f;
            const float diff = m_servos[i].targetPulseUs - m_servos[i].currentPulseUs;
            m_servos[i].velUsPerS = diff / timeToReachS;
            m_servos[i].timeRemainingS = timeToReachS;
            i++;
        }
    }
}

void MotorControl::updateInterpolation()
{
    // control loop period (must match task period)
    const float dtS = static_cast<float>(kControlPeriodMs) / 1000.0f;

    for (auto &servo : m_servos)
    {
        if (servo.timeRemainingS <= 0.0f || servo.velUsPerS == 0.0f)
        {
            // nothing to do for this joint
            continue;
        }

        float stepUs = servo.velUsPerS * dtS;
        float remainingUs = servo.targetPulseUs - servo.currentPulseUs;

        if ((stepUs > 0.0f && stepUs > remainingUs) ||
            (stepUs < 0.0f && stepUs < remainingUs))
        {
            // next step would exceed the target so set position exactly
            // to the target and end the interpolation
            servo.currentPulseUs = servo.targetPulseUs;
            servo.timeRemainingS = 0.0f;
            servo.velUsPerS = 0.0f;
        }
        else
        {
            // move one step
            servo.currentPulseUs += stepUs;

            // subtract timestep from the remaining time
            // in a correctly planned move this will reach zero exactly in the
            // final interpolation step
            servo.timeRemainingS -= dtS;

            // sanity check - keep time non-negative
            if (servo.timeRemainingS < 0.0f)
                servo.timeRemainingS = 0.0f;
        }
    }
}

void MotorControl::updateAllOutputs()
{
    for (const auto &servo : m_servos)
    {
        const auto &cfg = *servo.cfg;

        const float pulseUs = servo.currentPulseUs;

        // TODO: JP, update only when neccessary? to consider, now it shouldn't
        // hurt
        if (!m_driver.setPulseUs(cfg.pcaChannel, pulseUs))
        {
            // TODO: JP, log error? dont stop the robot though
        }
    }
}

void MotorControl::processCommands()
{
    CommandSingle cmdS;
    CommandMulti cmdM;
    // drain queue - last command for a given joint in this cycle wins
    // Queue multi has a priority over single queue
    while( xQueueReceive(m_cmdQueueMulti, &cmdM, 0) == pdPASS)
    {
        LOG_TRC("Received log multi");
        startMoves(cmdM);
    }
    while (xQueueReceive(m_cmdQueueSingle, &cmdS, 0) == pdPASS)
    {
        LOG_TRC("New single move cmd received %d, %f", cmdS.joint, cmdS.targetU);
        startMove(cmdS.joint, cmdS.targetU, cmdS.timeToReachMs);
    }
}

void MotorControl::setServosDefault()
{
    m_driver.setPulseUs(0, 1500.0f);
    m_driver.setPulseUs(1, 1500.0f);
    m_driver.setPulseUs(2, 1500.0f);
    m_driver.setPulseUs(3, 1500.0f);
    m_driver.setPulseUs(4, 1500.0f);
    m_driver.setPulseUs(5, 1500.0f);
    m_driver.setPulseUs(6, 1500.0f);
    m_driver.setPulseUs(7, 1500.0f);
    m_driver.setPulseUs(8, 1500.0f);
    m_driver.setPulseUs(9, 1500.0f);
    m_driver.setPulseUs(10, 1500.0f);
    m_driver.setPulseUs(11, 1500.0f);

}

// TODO: JP, HOMING FUNCTIONALITY
