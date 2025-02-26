/**
 * @file SessionManager.hpp
 * @brief Declaration of the SessionManager class.
 */

 #ifndef SESSIONMANAGER_HPP
 #define SESSIONMANAGER_HPP
 
 #include "Session.hpp"
 #include <unordered_map>
 #include <memory>
 #include <string>
 #include <mutex>
 
 namespace ChatServer {
 
 /**
  * @brief Manages active client sessions.
  */
 class SessionManager {
 public:
     SessionManager();
     ~SessionManager();
 
     std::shared_ptr<Session> createSession(const std::string& sessionId);
     void removeSession(const std::string& sessionId);
     std::shared_ptr<Session> getSession(const std::string& sessionId) const;
 
 private:
     std::unordered_map<std::string, std::shared_ptr<Session>> sessions;
     mutable std::mutex mutex_;
 };
 
 } // namespace ChatServer
 
 #endif // SESSIONMANAGER_HPP
 