/**
 * @file SessionManager.cpp
 * @brief Implementation of the SessionManager class.
 */

 #include "SessionManager.hpp"
 #include <iostream>
 
 namespace ChatServer {
 
 SessionManager::SessionManager() {}
 
 SessionManager::~SessionManager() {}
 
 std::shared_ptr<Session> SessionManager::createSession(const std::string& sessionId) {
     auto session = std::make_shared<Session>(sessionId);
     {
         std::lock_guard<std::mutex> lock(mutex_);
         sessions[sessionId] = session;
     }
     std::cout << "Session created: " << sessionId << std::endl;
     return session;
 }
 
 void SessionManager::removeSession(const std::string& sessionId) {
     {
         std::lock_guard<std::mutex> lock(mutex_);
         sessions.erase(sessionId);
     }
     std::cout << "Session removed: " << sessionId << std::endl;
 }
 
 std::shared_ptr<Session> SessionManager::getSession(const std::string& sessionId) const {
     std::lock_guard<std::mutex> lock(mutex_);
     auto it = sessions.find(sessionId);
     return (it != sessions.end()) ? it->second : nullptr;
 }
 
 } // namespace ChatServer
 