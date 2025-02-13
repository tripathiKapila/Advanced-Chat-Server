#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <vector>

class Session; // forward declaration

class SessionManager {
public:
    static SessionManager& instance() {
        static SessionManager instance;
        return instance;
    }
    void addSession(const std::string& username, std::shared_ptr<Session> session);
    void removeSession(const std::string& username);
    std::shared_ptr<Session> getSession(const std::string& username);
    std::vector<std::string> listSessions();
private:
    SessionManager() {}
    std::unordered_map<std::string, std::weak_ptr<Session>> sessions_;
    std::mutex mutex_;
};

#endif // SESSIONMANAGER_HPP
