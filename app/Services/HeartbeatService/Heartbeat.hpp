/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.20.2025
/   @author: pawrap
/   @brief: Simple service to blink LED
*/
#pragma once

#include <ServiceTemplate/Service.hpp>

using namespace types;

namespace services
{
    class Heartbeat : public ServiceTemplate
    {
      private:
        u32 m_blinkTime;

      public:
        Heartbeat(const char *serviceName, configSTACK_DEPTH_TYPE stackDepth,
                  const UBaseType_t servicePriority, StackType_t *stackBuffer,
                  StaticTask_t *taskBuffer, u32 blinkTime);

        [[noreturn]] void task() override;
    };

} // namespace services