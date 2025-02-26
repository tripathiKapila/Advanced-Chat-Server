/**
 * @file UserManager.cpp
 * @brief Implementation of the UserManager class.
 */

 #include "UserManager.hpp"
 #include <iostream>
 
 namespace ChatServer {
 
 UserManager::UserManager() {}
 
 UserManager::~UserManager() {}
 
 bool UserManager::registerUser(const std::string& username, const std::string& password) {
     std::lock_guard<std::mutex> lock(mutex_);
     if (users.find(username) == users.end()) {
         users[username] = password;
         std::cout << "User registered: " << username << std::endl;
         return true;
     }
     return false;
 }
 
 bool UserManager::authenticateUser(const std::string& username, const std::string& password) {
     std::lock_guard<std::mutex> lock(mutex_);
     auto it = users.find(username);
     if (it != users.end() && it->second == password) {
         std::cout << "User authenticated: " << username << std::endl;
         return true;
     }
     return false;
 }
 
 } // namespace ChatServer
 