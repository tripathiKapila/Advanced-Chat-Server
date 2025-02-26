#ifndef LOGGING_HPP
#define LOGGING_HPP

// Only define BOOST_DATE_TIME_NO_LIB if not already defined.
#ifndef BOOST_DATE_TIME_NO_LIB
#define BOOST_DATE_TIME_NO_LIB
#endif

#include <boost/date_time/posix_time/posix_time.hpp>  // Full definition for ptime
#include <boost/log/support/date_time.hpp>            // Provides date_time formatter support
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace Logging {

inline void init_logging() {
    namespace logging = boost::log;
    namespace keywords = boost::log::keywords;
    namespace expr = boost::log::expressions;

    // Setup console logging with formatted timestamp and severity.
    logging::add_console_log(
        std::clog,
        keywords::format = (
            expr::stream
                << "[" << expr::format_date_time<boost::posix_time::ptime>(
                    expr::attr<boost::posix_time::ptime>("TimeStamp"),
                    "%Y-%m-%d %H:%M:%S"
                ) << "] "
                << "[" << logging::trivial::severity << "] : "
                << expr::smessage
        )
    );

    // Set a global filter to log messages with severity >= info.
    logging::core::get()->set_filter(
        logging::trivial::severity >= logging::trivial::info
    );

    // Add common attributes like TimeStamp.
    logging::add_common_attributes();
}

} // namespace Logging

#endif // LOGGING_HPP
