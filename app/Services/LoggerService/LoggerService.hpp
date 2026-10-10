/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 12.31.2025
/   @author: pawrap
/   @brief: Main logger service that flushes previously
/           prepared logs to particular interfaces
*/
#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include <Logger/Logger.hpp>
#include <ServiceTemplate/Service.hpp>
#include <Settings.hpp>

using namespace types;

namespace services
{
    struct loggerSettings
    {
        bool isUartLogOn = false;
        bool isSwoLogOn = false;
        bool isSdLogOn = false;
    };

    class LoggerService : public ServiceTemplate
    {
      private:
        loggerSettings m_Settings;
        QueueHandle_t m_LoggerQ = nullptr;
        LogMessage m_LogMessage;

        uint8_t m_MaxLogSize = settings::MaxLogSize;
        void flushUart();
        // TODO: PR may be next steps to implement
        // void flushSWO();
        // void flushSD();

      public:
        LoggerService(const char *serviceName,
                      configSTACK_DEPTH_TYPE stackDepth,
                      const UBaseType_t servicePriority,
                      StackType_t *stackBuffer, StaticTask_t *taskBuffer,
                      loggerSettings settings, QueueHandle_t loggerQueue);

        [[noreturn]] void task() override;
    };

} // namespace services