#include "UserManager.hpp"
#include "Database.hpp"
#include <sstream>
#include <cstdlib>

bool UserManager::registerUser(const std::string& username, const std::string& password) {
    // Check if the user already exists.
    std::string checkSql = "SELECT COUNT(*) FROM users WHERE username = '" + username + "';";
    bool exists = false;
    Database::instance().query(checkSql, [&exists](int argc, char** argv, char** /*azColName*/) {
        if (argc > 0 && argv[0]) {
            int count = std::atoi(argv[0]);
            if (count > 0)
                exists = true;
        }
    });
    if (exists)
        return false; // User already registered.

    // Insert the new user into the database.
    std::stringstream ss;
    ss << "INSERT INTO users (username, password) VALUES ('" << username << "', '" << password << "');";
    return Database::instance().execute(ss.str());
}

bool UserManager::authenticate(const std::string& username, const std::string& password) {
    bool authenticated = false;
    std::stringstream ss;
    ss << "SELECT password FROM users WHERE username = '" << username << "';";
    Database::instance().query(ss.str(), [&authenticated, password](int argc, char** argv, char** /*azColName*/) {
        if (argc > 0 && argv[0]) {
            std::string storedPassword = argv[0];
            if (storedPassword == password)
                authenticated = true;
        }
    });
    return authenticated;
}
