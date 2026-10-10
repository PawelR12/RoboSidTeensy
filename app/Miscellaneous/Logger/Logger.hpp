/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 12.29.2025
/   @author: pawrap
/   @brief: Simple logger class that just facilatate log creation, then logs are
/           are transmited to logging task queue where they are flushed
*/
#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include <Logger/LoggerUtils.hpp>
#include <Settings.hpp>
#include <etl/string.h>

using namespace types;

// clang-format off
#if LOG_VERBOSITY == 1
  #define VERBOSE __FILENAME__ , __LINE__,
#else
  #define VERBOSE 
#endif

// Define proper log level functions only if proper LOG_LEVEL is set
// LOG_LEVEL is defined in Settings.hpp
#if LOG_LEVEL <= L_TRACE
  #define LOG(...) Logger::CreateLog(logLevel::None, VERBOSE __VA_ARGS__)
  #define LOG_TRC(...) Logger::CreateLog(logLevel::Trace, VERBOSE __VA_ARGS__)
#endif
#if LOG_LEVEL <= L_DEBUG
  #define LOG_DBG(...) Logger::CreateLog(logLevel::Debug, VERBOSE __VA_ARGS__)
#endif
#if LOG_LEVEL <= L_INFO
  #define LOG_INF(...) Logger::CreateLog(logLevel::Info, VERBOSE __VA_ARGS__)
#endif
#if LOG_LEVEL <= L_WARNING
  #define LOG_WRN(...) Logger::CreateLog(logLevel::Warning, VERBOSE __VA_ARGS__)
#endif
#if LOG_LEVEL <= L_ERROR
  #define LOG_ERR(...) Logger::CreateLog(logLevel::Error, VERBOSE __VA_ARGS__)
#endif
#if LOG_LEVEL <= L_CRITICAL
  #define LOG_CRI(...) Logger::CreateLog(logLevel::Critical, VERBOSE __VA_ARGS__)
#endif
#define LOG_ALW(...) Logger::CreateLog(logLevel::Always, VERBOSE __VA_ARGS__)

#if LOG_LEVEL > L_TRACE
  #define LOG(...)
  #define LOG_TRC(...)
#endif
#if LOG_LEVEL > L_DEBUG
  #define LOG_DBG(...)
#endif
#if LOG_LEVEL > L_INFO
  #define LOG_INF(...)
#endif
#if LOG_LEVEL > L_WARNING
  #define LOG_WRN(...)
#endif
#if LOG_LEVEL > L_ERROR
  #define LOG_ERR(...)
#endif
#if LOG_LEVEL > L_CRITICAL
  #define LOG_CRI(...)
#endif

// clang-format on

class Logger
{
  private:
    bool m_IsInitialized = false;
    QueueHandle_t m_LoggerQ;

    Logger() = default;
    ~Logger() = default;

    // removing copy and move constructors, assigment operators
    Logger(const Logger &other) = delete;
    void operator=(const Logger &other) = delete;
    Logger(Logger &&other) = delete;
    void operator=(Logger &&other) = delete;

    u32 GetTimestamp();

  public:
    retTypes Init(const QueueHandle_t &queue);

    // Logger is implemented as a singleton
    static Logger *GetInstance();
    static void CreateLog(logLevel severity, const char *fmt, ...);
    static void CreateLog(logLevel severity, const char *filename, int line,
                          const char *fmt, ...);
};