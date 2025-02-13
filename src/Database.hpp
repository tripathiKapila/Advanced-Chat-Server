#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <sqlite3.h>
#include <string>
#include <mutex>
#include <vector>
#include <functional>

class Database {
public:
    // Get the singleton instance of the Database manager.
    static Database& instance();

    // Open the database file (e.g., "chat.db"). Returns true on success.
    bool open(const std::string& dbFile);

    // Close the database connection.
    void close();

    // Execute an SQL command that does not return data (e.g., CREATE, INSERT).
    // Returns true on success.
    bool execute(const std::string& sql);

    // Execute a query and process each row with a callback.
    // The callback is of the form: void callback(int argc, char** argv, char** azColName)
    bool query(const std::string& sql,
               std::function<void(int argc, char** argv, char** azColName)> callback);

private:
    Database();
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    sqlite3* db_;
    std::mutex mutex_;
};

#endif // DATABASE_HPP
