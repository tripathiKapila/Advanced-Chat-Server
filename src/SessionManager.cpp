/**
 * @file SessionManager.cpp
 * @brief Implementation of the SessionManager class.
 */

#include "SessionManager.hpp"
#include "Session.hpp"
#include "Logging.hpp"

namespace ChatServer {

std::shared_ptr<SessionManager> SessionManager::instance = nullptr;

std::shared_ptr<SessionManager> SessionManager::getInstance() {
    if (!instance) {
        // Use the SessionManagerCreator to create the instance
        instance = SessionManagerCreator::create();
    }
    return instance;
}

SessionManager::SessionManager() {
    Logging::info("SessionManager initialized");
}

void SessionManager::addSession(std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(mutex);
    sessions[session->getSessionId()] = session;
    Logging::info("Session added: " + session->getSessionId());
}

void SessionManager::removeSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = sessions.find(sessionId);
    if (it != sessions.end()) {
        sessions.erase(it);
        Logging::info("Session removed: " + sessionId);
    }
}

std::shared_ptr<Session> SessionManager::getSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = sessions.find(sessionId);
    if (it != sessions.end()) {
        return it->second;
    }
    return nullptr;
}

std::map<std::string, std::shared_ptr<Session>> SessionManager::getAllSessions() const {
    std::lock_guard<std::mutex> lock(mutex);
    return sessions;
}

void SessionManager::broadcastMessage(const std::string& message, const std::string& senderSessionId) {
    std::map<std::string, std::shared_ptr<Session>> sessionsCopy;
    {
        std::lock_guard<std::mutex> lock(mutex);
        sessionsCopy = sessions;
    }
    
    for (const auto& pair : sessionsCopy) {
        // Don't send the message back to the sender
        if (pair.first != senderSessionId) {
            pair.second->sendMessage(message);
        }
    }
    
    Logging::info("Message broadcast to all sessions" + 
                 (senderSessionId.empty() ? "" : " except " + senderSessionId));
}

} // namespace ChatServer
 