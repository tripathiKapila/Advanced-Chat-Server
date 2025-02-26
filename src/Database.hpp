/**
 * @file Database.hpp
 * @brief Declaration of the Database class for SQLite integration.
 */

 #ifndef DATABASE_HPP
 #define DATABASE_HPP
 
 #include <string>
 #include <sqlite3.h>
 
 namespace ChatServer {
 
 /**
  * @brief Manages database connections and operations.
  */
 class Database {
 public:
     explicit Database(const std::string& dbFile);
     ~Database();
 
     bool open();
     void close();
     bool executeQuery(const std::string& query);
 
 private:
     std::string dbFile;
     sqlite3* db;
 };
 
 } // namespace ChatServer
 
 #endif // DATABASE_HPP
 