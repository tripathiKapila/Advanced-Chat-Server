#include "SessionManager.hpp"
#include "Session.hpp"

void SessionManager::addSession(const std::string& username, std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[username] = session;
}

void SessionManager::removeSession(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(username);
}

std::shared_ptr<Session> SessionManager::getSession(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(username);
    if (it != sessions_.end())
        return it->second.lock();
    return nullptr;
}

std::vector<std::string> SessionManager::listSessions() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> list;
    for (auto it = sessions_.begin(); it != sessions_.end(); ) {
        if (it->second.expired())
            it = sessions_.erase(it);
        else {
            list.push_back(it->first);
            ++it;
        }
    }
    return list;
}
