/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 10.10.2026
/   @author: pawrap
/   @brief: Header file with vital, global settings for the project
*/

#pragma once

#include <Logger/LoggerUtils.hpp>
#include <Types/types.hpp>

using namespace types;

// some of log settings are in LoggerUtils.hpp
#define LOG_LEVEL                                                              \
    L_TRACE // change it to set appropriate log level, see LoggerUtils.hpp

#define LOG_VERBOSITY                                                          \
    L_VERBOSE_ON // choose between LOG_VERBOSE_ON/OFF in order to display
                  // additional debug info

namespace settings
{
    /* Services priorities */
    constexpr u32 HeartBeatS_PRIORITY = 1;
    constexpr u32 MotorControlS_PRIORITY = 2;
    constexpr u32 LoggerS_PRIORITY = 2;
    constexpr u32 IpcS_PRIORITY = 4;

    /* Services STACK sizes */
    constexpr u32 HeartBeatS_STACK = 2048;
    constexpr u32 MotorControlS_STACK = 4096;
    constexpr u32 LoggerS_STACK = 2048;
    constexpr u32 IpcS_STACK = 8168;

    /* Miscellaneous services parameters */
    constexpr u32 HeartBeatS_BlinkInterval = 500;

    /* Shared Queues Lengths*/
    constexpr u32 LoggerQueue_Lenght = 10;

    // constexpr u32 LoggerQueue_ItemSize = sizeof(LogMessage);
} // namespace settings