/**
 * @file main.cpp
 * @brief Entry point for the AdvancedChatServer application.
 */

 #include "Logging.hpp"
 #include "Server.hpp"
 #include <iostream>
 #include <boost/log/trivial.hpp>
 
 int main() {
     try {
         // Initialize logging.
         Logging::init_logging();
         BOOST_LOG_TRIVIAL(info) << "Logging initialized.";
 
         // Create and start the server.
         ChatServer::Server server;
         server.start();
         BOOST_LOG_TRIVIAL(info) << "Server started.";
 
         std::cout << "Server is running. Press Enter to stop." << std::endl;
         std::cin.get();
 
         server.stop();
         BOOST_LOG_TRIVIAL(info) << "Server stopped.";
     } catch (const std::exception &ex) {
         BOOST_LOG_TRIVIAL(fatal) << "Error: " << ex.what();
         std::cerr << "Error: " << ex.what() << std::endl;
         return EXIT_FAILURE;
     }
     return EXIT_SUCCESS;
 }
 