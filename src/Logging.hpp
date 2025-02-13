#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>

// Initializes logging with a simple console sink and common attributes.
inline void init_logging() {
    boost::log::add_console_log(
        std::clog,
        boost::log::keywords::format = "[%TimeStamp%] [%Severity%]: %Message%"
    );
    boost::log::add_common_attributes();
}

#endif // LOGGING_HPP
