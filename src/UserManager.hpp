#ifndef USERMANAGER_HPP
#define USERMANAGER_HPP

#include <string>
#include <mutex>

class UserManager {
public:
    static UserManager& instance() {
        static UserManager instance;
        return instance;
    }
    // Register a new user using the database.
    // Returns false if the username already exists.
    bool registerUser(const std::string& username, const std::string& password);
    // Authenticate the user using the database.
    bool authenticate(const std::string& username, const std::string& password);
private:
    UserManager() {}
    // We rely entirely on the database (no in-memory map).
    std::mutex mutex_;
};

#endif // USERMANAGER_HPP
