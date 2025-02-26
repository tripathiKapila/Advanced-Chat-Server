/**
 * @file UserManager.hpp
 * @brief Declaration of the UserManager class.
 */

 #ifndef USERMANAGER_HPP
 #define USERMANAGER_HPP
 
 #include <string>
 #include <unordered_map>
 #include <mutex>
 
 namespace ChatServer {
 
 /**
  * @brief Manages user registration and authentication.
  */
 class UserManager {
 public:
     UserManager();
     ~UserManager();
 
     bool registerUser(const std::string& username, const std::string& password);
     bool authenticateUser(const std::string& username, const std::string& password);
 
 private:
     std::unordered_map<std::string, std::string> users; // username -> password
     mutable std::mutex mutex_;
 };
 
 } // namespace ChatServer
 
 #endif // USERMANAGER_HPP
 