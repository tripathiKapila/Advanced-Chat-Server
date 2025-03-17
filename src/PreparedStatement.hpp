#ifndef PREPAREDSTATEMENT_HPP
#define PREPAREDSTATEMENT_HPP

#include <sqlite3.h>
#include <string>
#include <stdexcept>

class PreparedStatement {
public:
    PreparedStatement(sqlite3* db, const std::string& query) : stmt(nullptr) {
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(db));
        }
    }
    ~PreparedStatement() {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
    }
    sqlite3_stmt* get() { return stmt; }
private:
    sqlite3_stmt* stmt;
};

#endif // PREPAREDSTATEMENT_HPP
