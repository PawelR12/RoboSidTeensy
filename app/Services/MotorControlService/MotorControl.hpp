/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.29.2025
/   @author: janpaw
/   @brief: Controls servo motors
*/
#pragma once

#include <array>

#include <Drivers/PCA9685/PCA9685.hpp>
#include <Robot/jointId.hpp>
#include <Robot/servoConfig.hpp>
#include <ServiceTemplate/Service.hpp>
#include <queue.h>

using namespace types;

namespace services
{
    /**
     * @brief Controls robot servos over PCA9685 using timed interpolation.
     *
     * MotorControl accepts target positions with desired time-to-reach and
     * interpolates servo motion at a fixed control rate defined by
     * kControlPeriodMs
     */
    class MotorControl : public ServiceTemplate
    {
      public:
        MotorControl(const char *serviceName, configSTACK_DEPTH_TYPE stackDepth,
                     const UBaseType_t servicePriority,
                     StackType_t *stackBuffer, StaticTask_t *taskBuffer,
                     PCA9685 &driver);

        [[noreturn]] void task() override;

        /**
         * @brief Schedule joint motion to the given position in the specified
         * time.
         *
         * If @p timeToReachMs is zero, the joint is snapped to @p targetU
         * without interpolation.
         *
         * @param id             Joint identifier.
         * @param targetU        Target position [-1..+1]
         * @param timeToReachMs  Requested time to reach target (ms).
         * @return true          Command enqueued successfully.
         * @return false         Command queue is full.
         */
        [[nodiscard]] bool moveJoint(robot::JointId id, float targetU,
                                     u16 timeToReachMs);
        
        /**
         * @brief Schedule joints motion to the given position in the specified
         * time.
         *
         * If @p timeToReachMs is zero, the joint is snapped to @p targetUs
         * without interpolation.
         *
         * @param targetUs       Array with expected target Us, if has values eq. to "0" then joint is omitted
         * @param timeToReachMs  Requested time to reach target (ms).
         * @return true          Command enqueued successfully.
         * @return false         Command queue is full.
         */
        bool moveJoints(const std::array<float, robot::jointCount()>& targetsUs, u16 timeToReachMs);
      
    private:
        /// control loop period in ms
        static constexpr u8 kControlPeriodMs = 20;

        /**
         * @brief Per-joint runtime state for servo motion.
         */
        struct ServoState
        {
            const robot::ServoConfig *cfg;
            float currentPulseUs;
            float targetPulseUs;
            float velUsPerS;
            float timeRemainingS;
        };

        /// runtime state for all joints
        std::array<ServoState, robot::jointCount()> m_servos;

        PCA9685 &m_driver;

        struct CommandSingle
        {
            robot::JointId joint;
            float targetU;
            u16 timeToReachMs;
        };

        struct CommandMulti
        {
            std::array<float, robot::jointCount()> targetUs;
            u16 timeToReachMs;
        };
        
        // TODO: JP, tune queue length
        static constexpr size_t kQueueLength = 5;

        /**
        * @brief queue for updating single servo 
        */
        QueueHandle_t m_cmdQueueSingle = nullptr;
        StaticQueue_t m_cmdQueueSingleBuffer;
        u8 m_cmdQueueSingleStorage[kQueueLength * sizeof(CommandSingle)];

        /**
        * @brief queue for updating multiple servos 
        */
        QueueHandle_t m_cmdQueueMulti = nullptr;
        StaticQueue_t m_cmdQueueMultiBuffer;
        u8 m_cmdQueueMultiStorage[kQueueLength * sizeof(CommandMulti)];
        /**
         * @brief Initializes runtime state for all servos.
         */
        void initServoStates();
        /**
         * @brief Processes pending motion commands from the queue.
         */
        void processCommands();

        /**
         * @brief Initializes a new servo movement by computing target pulse,
         * interpolation velocity, and movement duration.
         */
        void startMove(robot::JointId joint, float targetUs, u16 timeToReachMs);

        /**
         * @brief Initializes a new servos movements by computing target pulse,
         * interpolation velocity, and movement duration.
         */
        void startMoves(const CommandMulti& cmd);

        /**
         * @brief Advances ongoing servo movements by one control period,
         * updating current pulse toward the target using linear
         * interpolation.
         */
        void updateInterpolation();

        /**
         * @brief Sends current pulse values to the PCA driver to update all
         * servos.
         */
        void updateAllOutputs();

        /**
         * @brief Sets servos in default position.
         */
        void setServosDefault();
    };
} // namespace services