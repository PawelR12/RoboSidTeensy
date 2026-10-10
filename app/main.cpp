#include <Arduino.h>
#include <Settings.hpp>
#include "FreeRTOS.h"
#include "task.h"
#include <HeartbeatService/Heartbeat.hpp>
// #include <LoggerService/LoggerService.hpp>

/* Services freeRTOS TCB blocks */
StaticTask_t heartbeatServiceTCB;

/* Services freeRTOS tasks */
StackType_t heartbeatServiceSTACK[settings::HeartBeatS_STACK];

/* Static global service instances */
services::Heartbeat Heartbeat("HeartBeatService", settings::HeartBeatS_STACK,
                              settings::HeartBeatS_PRIORITY, heartbeatServiceSTACK,
                              &heartbeatServiceTCB, settings::HeartBeatS_BlinkInterval);

int main(void)
{
    Serial.begin(9600); // USB is always 12 or 480 Mbit/sec

	Heartbeat.create();
	/* Start the scheduler to start the tasks executing. */
    vTaskStartScheduler();
}


#if 1
namespace __gnu_cxx {
void __verbose_terminate_handler()
{
	while (1) asm ("WFI");
}
}
#endif
