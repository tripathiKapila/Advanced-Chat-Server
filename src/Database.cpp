/**
 * @file Database.cpp
 * @brief Implementation of the Database class with prepared statement support.
 */

#include "Database.hpp"
#include "PreparedStatement.hpp"
#include <iostream>

namespace ChatServer {

Database::Database(const std::string& dbFile)
    : dbFile(dbFile), db(nullptr) {}

Database::~Database() {
    close();
}

bool Database::open() {
    if (sqlite3_open(dbFile.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    std::cout << "Database opened: " << dbFile << std::endl;
    return true;
}

void Database::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
        std::cout << "Database closed." << std::endl;
    }
}

bool Database::executeQuery(const std::string& query) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << (errMsg ? errMsg : "unknown error") << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::executePreparedQuery(const std::string& query) {
    try {
        PreparedStatement stmt(db, query);
        if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
            std::cerr << "Failed to execute prepared statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
    } catch (const std::exception& ex) {
        std::cerr << "SQL Exception: " << ex.what() << std::endl;
        return false;
    }
    return true;
}

} // namespace ChatServer
