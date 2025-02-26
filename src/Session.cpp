/**
 * @file Session.cpp
 * @brief Implementation of the Session class.
 */

 #include "Session.hpp"
 #include <iostream>
 
 namespace ChatServer {
 
 Session::Session(const std::string& id) : sessionId(id) {}
 
 Session::~Session() {}
 
 void Session::sendMessage(const std::string& message) {
     std::cout << "Sending message to session " << sessionId << ": " << message << std::endl;
 }
 
 std::string Session::getSessionId() const {
     return sessionId;
 }
 
 } // namespace ChatServer
 