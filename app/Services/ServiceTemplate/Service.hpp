/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.20.2025
/   @author: pawrap
/   @brief: Parent class for all the RoboSid services.
*/
#pragma once

#include <FreeRTOS.h>
#include <Types/types.hpp>
#include <string>
#include <task.h>

using namespace types;

namespace services
{

    class ServiceTemplate
    {
      private:
        const char *m_serviceName;
        u32 m_stackDepth;
        u32 m_servicePriority;
        StackType_t *m_stackBuffer;
        StaticTask_t *m_taskBuffer;
        TaskHandle_t m_taskHandle;

      public:
        ServiceTemplate(const char *serviceName,
                        configSTACK_DEPTH_TYPE stackDepth,
                        const UBaseType_t servicePriority,
                        StackType_t *stackBuffer, StaticTask_t *taskBuffer);
        virtual ~ServiceTemplate();
        void create();
        void resume();
        void suspend();

        static void trampoline(void *service);
        virtual void task() = 0;

        TaskHandle_t getTaskHandle();
    };

} // namespace services

/**
 * TODO: PR
 * - implement destructor (taskHandle)
 * - remove default constructor, copy constr and assigmnent operator ?
 * - introduce singleton design pattern (?)
 */