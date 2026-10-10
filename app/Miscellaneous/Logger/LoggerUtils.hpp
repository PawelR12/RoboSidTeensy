// /**
// /   _           __
// /  |_) _ |_  _ (_  o  _|
// /  | \(_)|_)(_)__) | (_|
// /
// /   @date: 01.04.2026
// /   @author: pawrap
// /   @brief: Logger utils
// */
// #pragma once

// #include <Types/types.hpp>
// #include <etl/string.h>

// using namespace types;

// #define __FILENAME__                                                           \
//     (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// // Verbosity levels
// #define L_VERBOSE_ON 1
// #define L_VERBOSE_OFF 0

// // Possible Log levels
// #define L_TRACE 1
// #define L_DEBUG 2
// #define L_INFO 3
// #define L_WARNING 4
// #define L_ERROR 5
// #define L_CRITICAL 6

// namespace settings
// {
//     // MaxLogSize decided to be there because LogMessage relies on it
//     // and we need sizeof(LogMessage) in settings, queue size so if it would
//     // be in Settings.hpp then we would have to define LogMessage there to don't
//     // make cycle include.
//     constexpr u8 MaxLogSize = 128;
// } // namespace settings

// /* Shared Queues Item*/
// struct LogMessage
// {
//     char buf[settings::MaxLogSize];
//     u8 size;
// };

// enum class logLevel : u8
// {
//     Trace = 100,
//     Debug,
//     Info,
//     Warning,
//     Error,
//     Critical,
//     Always,
//     None
// };