#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ctime>

namespace Logging {

// Simple logging functions
void init_logging();
void init_logging(const std::string& logFile);
void debug(const std::string& message);
void info(const std::string& message);
void warning(const std::string& message);
void error(const std::string& message);
void fatal(const std::string& message);

// Implementation details
namespace detail {

enum LogLevel {
    DEBUG_LEVEL,
    INFO_LEVEL,
    WARNING_LEVEL,
    ERROR_LEVEL,
    FATAL_LEVEL
};

class Logger {
public:
    static Logger& getInstance();
    void setLevel(LogLevel level);
    void log(LogLevel level, const std::string& message);
    void init(const std::string& logFile = "server.log");

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream logFile;
    LogLevel currentLevel;
    std::mutex mutex;
    bool initialized;

    std::string levelToString(LogLevel level);
    std::string getCurrentTime();
};

} // namespace detail

} // namespace Logging

#endif // LOGGING_HPP
