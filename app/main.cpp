#include <Arduino.h>
#include <Settings.hpp>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <MotorControlService/MotorControl.hpp>
#include <HeartbeatService/Heartbeat.hpp>
#include <LoggerService/LoggerService.hpp>
#include <Logger/Logger.hpp>
#include <Wire.h>

using namespace settings;
using namespace types;

/* Static global driver instances */
PCA9685 PCA968Driver{Wire, 0x40};

/* Services freeRTOS TCB blocks */
StaticTask_t heartbeatServiceTCB;
StaticTask_t loggerServiceTCB;
StaticTask_t motorControlServiceTCB;

/* Services freeRTOS tasks */
StackType_t heartbeatServiceSTACK[HeartBeatS_STACK];
StackType_t loggerServiceSTACK[LoggerS_STACK];
StackType_t motorControlServiceSTACK[MotorControlS_STACK];

/* Shared queues control blocks */
StaticQueue_t loggerQueue_CB;

/* Shared queues buffers */
uint8_t loggerQueue_StorageBuffer[LoggerQueue_Lenght * LoggerQueue_ItemSize];

/* Global Shared Queues */
QueueHandle_t LoggerQueue =
    xQueueCreateStatic(LoggerQueue_Lenght, LoggerQueue_ItemSize,
                       loggerQueue_StorageBuffer, &loggerQueue_CB);

// Create logger instance
Logger *Logger = Logger::GetInstance();

// Create logger settings struct
services::loggerSettings loggerSettings{
    .isUartLogOn = true,
    .isSwoLogOn = false,
    .isSdLogOn = false,
};

services::LoggerService LoggerService("LoggerService", LoggerS_STACK,
                                      LoggerS_PRIORITY, loggerServiceSTACK,
                                      &loggerServiceTCB, loggerSettings,
                                      LoggerQueue);

/* Static global service instances */
services::Heartbeat Heartbeat("HeartBeatService", HeartBeatS_STACK,
                              HeartBeatS_PRIORITY, heartbeatServiceSTACK,
                              &heartbeatServiceTCB, HeartBeatS_BlinkInterval);

services::MotorControl MotorControl("MotorControlService", MotorControlS_STACK,
    MotorControlS_PRIORITY,
    motorControlServiceSTACK,
    &motorControlServiceTCB, PCA968Driver);

int main(void)
{
    // Serial.begin(9600); // USB is always 12 or 480 Mbit/sec

	// DebugP_log("We are in main!\r\n");
    Logger->Init(LoggerQueue);
	Heartbeat.create();
	LoggerService.create();
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
