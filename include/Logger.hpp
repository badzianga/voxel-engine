#pragma once
#include <string>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

class Logger {
public:
    static void log(const std::string& message, LogLevel level, const char* file, int line);
private:
    static std::string logLevelToString(LogLevel level);
};

#define LOG_DEBUG(message) Logger::log(message, LogLevel::Debug, __FILE__, __LINE__)
#define LOG_INFO(message) Logger::log(message, LogLevel::Info, __FILE__, __LINE__)
#define LOG_WARNING(message) Logger::log(message, LogLevel::Warning, __FILE__, __LINE__)
#define LOG_ERROR(message) Logger::log(message, LogLevel::Error, __FILE__, __LINE__)
#define LOG_CRITICAL(message) Logger::log(message, LogLevel::Critical, __FILE__, __LINE__)
