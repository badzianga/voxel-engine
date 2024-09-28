#include "Logger.hpp"
#include <cstdio>

void Logger::log(const std::string& message, LogLevel level, const char* file, int line) {
    fprintf(stdout, "%s:%d [%s]: %s\n", file, line, logLevelToString(level).c_str(), message.c_str());
}

std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Critical:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}
