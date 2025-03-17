/**
 * @file UserManager.hpp
 * @brief Declaration of the UserManager class.
 */

#pragma once

#include <string>
#include <map>
#include <mutex>
#include <memory>

namespace ChatServer {

class User {
public:
    User(const std::string& userId, const std::string& nickname = "");
    ~User() = default;
    
    // Get the user ID
    const std::string& getUserId() const;
    
    // Get the user's nickname
    const std::string& getNickname() const;
    
    // Set the user's nickname
    void setNickname(const std::string& nickname);

private:
    std::string userId_;
    std::string nickname_;
};

/**
 * @brief Manages user registration and authentication.
 */
class UserManager {
public:
    UserManager();
    ~UserManager() = default;
    
    // Create a new user
    std::shared_ptr<User> createUser(const std::string& userId, const std::string& nickname = "");
    
    // Get a user by ID
    std::shared_ptr<User> getUser(const std::string& userId);
    
    // Remove a user
    void removeUser(const std::string& userId);
    
    // Get all users
    std::map<std::string, std::shared_ptr<User>> getAllUsers() const;
    
    // Update a user's nickname
    bool updateNickname(const std::string& userId, const std::string& nickname);

private:
    std::map<std::string, std::shared_ptr<User>> users_;
    mutable std::mutex mutex_;
};

} // namespace ChatServer
 