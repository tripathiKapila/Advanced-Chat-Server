/**
 * @file Server.cpp
 * @brief Implementation of the Server class.
 */

 #include "Server.hpp"
 #include <iostream>
 #include <thread>
 #include <chrono>
 
 namespace ChatServer {
 
 class Server::Impl {
 public:
     bool running;
     // Create a thread pool with 4 worker threads for background tasks.
     ThreadPool threadPool;
 
     Impl() : running(false), threadPool(4) {}
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
 
     // Example: Enqueue a background task using the thread pool.
     pImpl->threadPool.enqueue([]{
         std::this_thread::sleep_for(std::chrono::milliseconds(100));
         std::cout << "Background task executed on thread "
                   << std::this_thread::get_id() << std::endl;
     });
 
     // (Start networking threads to accept client connections here.)
 }
 
 void Server::stop() {
     pImpl->running = false;
     std::cout << "Server stopped." << std::endl;
     // (Cleanup resources and stop networking threads here.)
 }
 
 } // namespace ChatServer
 