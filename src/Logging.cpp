#include "Logging.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Logging {

namespace detail {

Logger::Logger() : currentLevel(INFO_LEVEL), initialized(false) {}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::init(const std::string& logFilePath) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!initialized) {
        logFile.open(logFilePath, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << logFilePath << std::endl;
        }
        initialized = true;
    }
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex);
    currentLevel = level;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < currentLevel) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);
    std::string timestamp = getCurrentTime();
    std::string levelStr = levelToString(level);
    
    std::string logMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    // Output to console
    std::cout << logMessage << std::endl;
    
    // Output to file if initialized
    if (initialized && logFile.is_open()) {
        logFile << logMessage << std::endl;
        logFile.flush();
    }
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case DEBUG_LEVEL:   return "DEBUG";
        case INFO_LEVEL:    return "INFO";
        case WARNING_LEVEL: return "WARNING";
        case ERROR_LEVEL:   return "ERROR";
        case FATAL_LEVEL:   return "FATAL";
        default:            return "UNKNOWN";
    }
}

std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace detail

// Global functions implementation
void init_logging() {
    detail::Logger::getInstance().init();
    detail::Logger::getInstance().setLevel(detail::INFO_LEVEL);
}

void init_logging(const std::string& logFile) {
    detail::Logger::getInstance().init(logFile);
    detail::Logger::getInstance().setLevel(detail::INFO_LEVEL);
}

void debug(const std::string& message) {
    detail::Logger::getInstance().log(detail::DEBUG_LEVEL, message);
}

void info(const std::string& message) {
    detail::Logger::getInstance().log(detail::INFO_LEVEL, message);
}

void warning(const std::string& message) {
    detail::Logger::getInstance().log(detail::WARNING_LEVEL, message);
}

void error(const std::string& message) {
    detail::Logger::getInstance().log(detail::ERROR_LEVEL, message);
}

void fatal(const std::string& message) {
    detail::Logger::getInstance().log(detail::FATAL_LEVEL, message);
}

} // namespace Logging
