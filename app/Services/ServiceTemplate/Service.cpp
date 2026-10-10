/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.20.2025
/   @author: pawrap
/   @brief: Parent class for all the RoboSid services.
*/
#include <ServiceTemplate/Service.hpp>

using namespace services;

void ServiceTemplate::create()
{
    // TODO: Add static assert (?)
    m_taskHandle =
        xTaskCreateStatic(trampoline, m_serviceName, m_stackDepth, this,
                          m_servicePriority, m_stackBuffer, m_taskBuffer);
}

ServiceTemplate::ServiceTemplate(const char *serviceName,
                                 configSTACK_DEPTH_TYPE stackDepth,
                                 const UBaseType_t servicePriority,
                                 StackType_t *stackBuffer,
                                 StaticTask_t *taskBuffer)
    : m_serviceName(serviceName), m_stackDepth(stackDepth),
      m_servicePriority(servicePriority), m_stackBuffer(stackBuffer),
      m_taskBuffer(taskBuffer)
{
}

services::ServiceTemplate::~ServiceTemplate()
{
    vTaskDelete(m_taskHandle);
}

void ServiceTemplate::resume()
{
    vTaskResume(m_taskHandle);
}

void ServiceTemplate::suspend()
{
    vTaskSuspend(m_taskHandle);
}

void ServiceTemplate::trampoline(void *service)
{
    auto servicePtr = static_cast<ServiceTemplate *>(service);
    servicePtr->task();
}

TaskHandle_t services::ServiceTemplate::getTaskHandle()
{
    return m_taskHandle;
}