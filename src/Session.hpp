/**
 * @file Session.hpp
 * @brief Declaration of the Session class.
 */

 #ifndef SESSION_HPP
 #define SESSION_HPP
 
 #include <string>
 
 namespace ChatServer {
 
 /**
  * @brief Represents a client session.
  */
 class Session {
 public:
     explicit Session(const std::string& id);
     ~Session();
 
     void sendMessage(const std::string& message);
     std::string getSessionId() const;
 
 private:
     std::string sessionId;
 };
 
 } // namespace ChatServer
 
 #endif // SESSION_HPP
 