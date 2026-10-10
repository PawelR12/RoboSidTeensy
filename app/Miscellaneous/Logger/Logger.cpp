#include "Logger.hpp"
#include <stdarg.h>

Logger *Logger::GetInstance()
{
    // Since C++11 static local variable initalization
    // is thread safe therefore we don't need any mutexes
    static Logger instance{};
    return &instance;
}

retTypes Logger::Init(const QueueHandle_t &queue)
{
    if (!m_IsInitialized && queue != nullptr)
    {
        m_IsInitialized = true;
        m_LoggerQ = queue;
        return retTypes::OK;
    }
    return retTypes::ERR;
}

void Logger::CreateLog(logLevel severity, const char *fmt, ...)
{
    if (!GetInstance()->m_IsInitialized)
        return;

    LogMessage logMessage;
    memset(&logMessage, 0, sizeof(logMessage));
    char rawMsg[settings::MaxLogSize - 8]; // max log size minus minimun
                                           // "[0]INF: "
    memset(rawMsg, 0, sizeof(rawMsg));

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(rawMsg, sizeof(rawMsg), fmt, ap);
    va_end(ap);

    snprintf(logMessage.buf, sizeof(logMessage.buf), "[%u] ",
             GetInstance()->GetTimestamp());
    switch (severity)
    {
    case logLevel::Trace:
        strncat(logMessage.buf,
                "TRC: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Debug:
        strncat(logMessage.buf,
                "DBG: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Info:
        strncat(logMessage.buf,
                "INF: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Warning:
        strncat(logMessage.buf,
                "WRN: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Error:
        strncat(logMessage.buf,
                "ERR: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Critical:
        strncat(logMessage.buf,
                "CRI: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Always:
        strncat(logMessage.buf,
                "ALW: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    default:
        break;
    }

    strncat(logMessage.buf, rawMsg,
            sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
    strncat(logMessage.buf, "\r\n",
            sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
    logMessage.size = strlen(logMessage.buf) + 1;

    xQueueSend(GetInstance()->m_LoggerQ, &logMessage, 0);
}

void Logger::CreateLog(logLevel severity, const char *filename, int line,
                       const char *fmt, ...)
{
    if (!GetInstance()->m_IsInitialized)
        return;

    LogMessage logMessage;
    memset(&logMessage, 0, sizeof(logMessage));
    char rawMsg[settings::MaxLogSize - 14]; // max log size minus minimun
                                            // "[0]x.c,1 INF: "
    memset(rawMsg, 0, sizeof(rawMsg));

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(rawMsg, sizeof(rawMsg), fmt, ap);
    va_end(ap);

    snprintf(logMessage.buf, sizeof(logMessage.buf), "[%u]",
             GetInstance()->GetTimestamp());

    snprintf(logMessage.buf + strlen(logMessage.buf),
             sizeof(logMessage.buf) - strlen(logMessage.buf), "%s,%d ",
             filename, line);

    switch (severity)
    {
    case logLevel::Trace:
        strncat(logMessage.buf,
                "TRC: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Debug:
        strncat(logMessage.buf,
                "DBG: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Info:
        strncat(logMessage.buf,
                "INF: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Warning:
        strncat(logMessage.buf,
                "WRN: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Error:
        strncat(logMessage.buf,
                "ERR: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Critical:
        strncat(logMessage.buf,
                "CRI: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    case logLevel::Always:
        strncat(logMessage.buf,
                "ALW: ", sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);
        break;
    default:
        break;
    }

    strncat(logMessage.buf, rawMsg,
            sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);

    strncat(logMessage.buf, "\r\n",
            sizeof(logMessage.buf) - strlen(logMessage.buf) - 1);

    logMessage.size = strlen(logMessage.buf) + 1;

    xQueueSend(GetInstance()->m_LoggerQ, &logMessage, 0);
}

u32 Logger::GetTimestamp()
{
    return xTaskGetTickCount();
}