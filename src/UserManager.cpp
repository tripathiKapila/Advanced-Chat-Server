/**
 * @file UserManager.cpp
 * @brief Implementation of the UserManager class with secure password handling.
 */

#include "UserManager.hpp"
#include "Logging.hpp"
#include <iostream>
#include <mutex>
#include <string>
#include <algorithm>

// Dummy hash function; replace with a robust solution like bcrypt or Argon2.
std::string hashPassword(const std::string& password) {
    return std::string(password.rbegin(), password.rend());
}

namespace ChatServer {

// User implementation
User::User(const std::string& userId, const std::string& nickname)
    : userId_(userId), nickname_(nickname.empty() ? userId : nickname) {
}

const std::string& User::getUserId() const {
    return userId_;
}

const std::string& User::getNickname() const {
    return nickname_;
}

void User::setNickname(const std::string& nickname) {
    nickname_ = nickname;
}

// UserManager implementation
UserManager::UserManager() {
    Logging::info("UserManager initialized");
}

std::shared_ptr<User> UserManager::createUser(const std::string& userId, const std::string& nickname) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if user already exists
    auto it = users_.find(userId);
    if (it != users_.end()) {
        return it->second;
    }
    
    // Create a new user
    auto user = std::make_shared<User>(userId, nickname);
    users_[userId] = user;
    
    Logging::info("User created: " + userId + (nickname.empty() ? "" : " with nickname " + nickname));
    return user;
}

std::shared_ptr<User> UserManager::getUser(const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = users_.find(userId);
    if (it != users_.end()) {
        return it->second;
    }
    
    return nullptr;
}

void UserManager::removeUser(const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = users_.find(userId);
    if (it != users_.end()) {
        users_.erase(it);
        Logging::info("User removed: " + userId);
    }
}

std::map<std::string, std::shared_ptr<User>> UserManager::getAllUsers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return users_;
}

bool UserManager::updateNickname(const std::string& userId, const std::string& nickname) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = users_.find(userId);
    if (it != users_.end()) {
        it->second->setNickname(nickname);
        Logging::info("User " + userId + " updated nickname to " + nickname);
        return true;
    }
    
    return false;
}

} // namespace ChatServer
