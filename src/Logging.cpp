#include "Logging.hpp"
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>

void init_logging() {
    // Configure console logging with a simple format.
    boost::log::add_console_log(
        std::clog,
        boost::log::keywords::format = "[%TimeStamp%] [%Severity%]: %Message%"
    );
    // Add common attributes like timestamp.
    boost::log::add_common_attributes();
}
