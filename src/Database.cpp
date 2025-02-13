#include "Database.hpp"
#include <iostream>

Database& Database::instance() {
    static Database instance;
    return instance;
}

Database::Database() : db_(nullptr) {}

Database::~Database() {
    close();
}

bool Database::open(const std::string& dbFile) {
    std::lock_guard<std::mutex> lock(mutex_);
    int rc = sqlite3_open(dbFile.c_str(), &db_);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db_) << "\n";
        sqlite3_close(db_);
        db_ = nullptr;
        return false;
    }
    return true;
}

void Database::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool Database::execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex_);
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (errMsg ? errMsg : "unknown error") << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::query(const std::string& sql,
                     std::function<void(int argc, char** argv, char** azColName)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    char* errMsg = nullptr;
    auto cb = [](void* data, int argc, char** argv, char** azColName) -> int {
        auto func = static_cast<std::function<void(int, char**, char**)>*>(data);
        (*func)(argc, argv, azColName);
        return 0;
    };
    int rc = sqlite3_exec(db_, sql.c_str(), cb, &callback, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (errMsg ? errMsg : "unknown error") << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}
