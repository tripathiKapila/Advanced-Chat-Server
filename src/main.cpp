#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include "Server.hpp"
#include "Logging.hpp"
#include "Database.hpp"    // For database initialization
#include <iostream>
#include <thread>
#include <vector>
#include <csignal>

int main() {
    // Initialize logging
    init_logging();

    // ----- Database Initialization Start -----
    // Open (or create) the SQLite database file "chat.db"
    if (!Database::instance().open("chat.db")) {
        std::cerr << "Failed to open database.\n";
        return 1;
    }
    // Create the users table if it doesn't exist.
    std::string createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            username TEXT PRIMARY KEY,
            password TEXT NOT NULL
        );
    )";
    if (!Database::instance().execute(createUsersTable)) {
        std::cerr << "Failed to create users table.\n";
        return 1;
    }
    // ----- Database Initialization End -----

    try {
        boost::asio::io_context io_context;
        unsigned short port = 12345;
        Server server(io_context, port);

        // Set up signal handling for graceful shutdown.
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](boost::system::error_code, int) {
            BOOST_LOG_TRIVIAL(info) << "Signal received. Initiating shutdown...";
            server.stop();
            io_context.stop();
        });

        server.start();

        // Run the I/O context in a thread pool.
        unsigned int thread_count = std::thread::hardware_concurrency();
        if (thread_count == 0)
            thread_count = 2;
        BOOST_LOG_TRIVIAL(info) << "Starting thread pool with " << thread_count << " threads.";
        std::vector<std::thread> threads;
        for (unsigned int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&io_context]() {
                try {
                    io_context.run();
                } catch (std::exception& e) {
                    BOOST_LOG_TRIVIAL(error) << "Exception in thread: " << e.what();
                }
            });
        }
        for (auto& t : threads)
            t.join();
    } catch (std::exception& e) {
        BOOST_LOG_TRIVIAL(fatal) << "Exception in main: " << e.what();
    }
    BOOST_LOG_TRIVIAL(info) << "Server shut down gracefully.";
    return 0;
}
