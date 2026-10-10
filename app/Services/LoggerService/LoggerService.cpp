/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.20.2025
/   @author: pawrap
/   @brief: Simple service to blink LED
*/
#include "LoggerService.hpp"
#include <LoggerService/LoggerService.hpp>
#include <Arduino.h>

using namespace services;

namespace
{
    u8 _clampLog(u8 logSize, u8 maxLogSize)
    {
        if(logSize > maxLogSize)
        {
            return maxLogSize;
        }
        return logSize;
    }
}

LoggerService::LoggerService(const char *serviceName,
                             configSTACK_DEPTH_TYPE stackDepth,
                             const UBaseType_t servicePriority,
                             StackType_t *stackBuffer, StaticTask_t *taskBuffer,
                             loggerSettings settings, QueueHandle_t loggerQueue)
    : ServiceTemplate(serviceName, stackDepth, servicePriority, stackBuffer,
                      taskBuffer),
      m_Settings(settings), m_LoggerQ(loggerQueue)
{
}

void LoggerService::task()
{
    Serial.begin(9600);
    while (true)
    {
        if (xQueueReceive(m_LoggerQ, &m_LogMessage, portMAX_DELAY) == pdPASS)
        {
            flushUart();
        }
    }
}

void LoggerService::flushUart()
{
    // TODO: improvments -> we can check out how logs are flushed and see if it uses DMA or IRQ
    u8 logSize = _clampLog(m_LogMessage.size, m_MaxLogSize);
    for(u8 i = 0; i < logSize; i++)
    {
        Serial.print(m_LogMessage.buf[i]);
    }
    if(logSize == m_MaxLogSize)
    {
        Serial.print("\r\n");
    }
}