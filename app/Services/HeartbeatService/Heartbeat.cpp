/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.20.2025
/   @author: pawrap
/   @brief: Simple service to blink LED
*/
#include <HeartbeatService/Heartbeat.hpp>
#include <Logger/Logger.hpp>
#include <Arduino.h>

#define INTERNAL_LED 13

using namespace services;
Heartbeat::Heartbeat(const char *serviceName, configSTACK_DEPTH_TYPE stackDepth,
                     const UBaseType_t servicePriority,
                     StackType_t *stackBuffer, StaticTask_t *taskBuffer,
                     u32 blinkTime)
    : ServiceTemplate(serviceName, stackDepth, servicePriority, stackBuffer,
                      taskBuffer),
      m_blinkTime(blinkTime)
{
}

void Heartbeat::task()
{
    pinMode(INTERNAL_LED, OUTPUT);
    LOG_INF("HeartBeat started");
    while (true)
    {
        digitalWrite(INTERNAL_LED, HIGH);
        vTaskDelay(pdMS_TO_TICKS(m_blinkTime));
        digitalWrite(INTERNAL_LED, LOW);
        vTaskDelay(pdMS_TO_TICKS(m_blinkTime));
    }
}
