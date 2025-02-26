/**
 * @file Server.hpp
 * @brief Declaration of the Server class.
 */

 #ifndef SERVER_HPP
 #define SERVER_HPP
 
 #include <memory>
 #include "ThreadPool.hpp"  // For background task execution
 
 namespace ChatServer {
 
 /**
  * @brief Manages incoming connections and delegates session handling.
  *
  * Integrates a thread pool for offloading background tasks.
  */
 class Server {
 public:
     Server();
     ~Server();
 
     void start();
     void stop();
 
 private:
     class Impl;
     std::unique_ptr<Impl> pImpl;
 };
 
 } // namespace ChatServer
 
 #endif // SERVER_HPP
 