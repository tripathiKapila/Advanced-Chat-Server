/**
 * @file Server.cpp
 * @brief Implementation of the Server class with asynchronous networking.
 */

#include "Server.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

namespace ChatServer {

class Server::Impl {
public:
    Impl() 
        : io_context(),
          acceptor(io_context),
          work(boost::asio::make_work_guard(io_context)),
          threadPool(4),
          running(false)
    {}

    // Start asynchronous accept loop.
    void asyncAccept() {
        auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
        acceptor.async_accept(*socket, [this, socket](const boost::system::error_code &ec) {
            if (!ec) {
                std::cout << "Accepted connection from: " 
                          << socket->remote_endpoint() << std::endl;
                // TODO: Create a new session and handle the connection.
            }
            asyncAccept(); // Continue accepting connections.
        });
    }

    void startAccepting(unsigned short port) {
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
        acceptor.open(endpoint.protocol());
        acceptor.bind(endpoint);
        acceptor.listen();
        asyncAccept();

        // Run the io_context in a separate thread.
        ioThread = std::thread([this]() { io_context.run(); });
    }

    void stopAccepting() {
        io_context.stop();
        if (ioThread.joinable()) {
            ioThread.join();
        }
    }

    bool running;
    ThreadPool threadPool;

private:
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work;
    std::thread ioThread;
};

Server::Server() : pImpl(std::make_unique<Impl>()) {}

Server::~Server() {
    if (pImpl->running) {
        stop();
    }
}

void Server::start() {
    pImpl->running = true;
    std::cout << "Server started." << std::endl;

    // Start asynchronous networking on port 12345.
    pImpl->startAccepting(12345);

    // Enqueue a sample background task.
    pImpl->threadPool.enqueue([]{
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Background task executed on thread " 
                  << std::this_thread::get_id() << std::endl;
    });
}

void Server::stop() {
    pImpl->running = false;
    pImpl->stopAccepting();
    std::cout << "Server stopped." << std::endl;
}

} // namespace ChatServer
