/**
 * @file SessionManager.hpp
 * @brief Declaration of the SessionManager class.
 */

#pragma once

#include <memory>
#include <map>
#include <mutex>
#include <string>

namespace ChatServer {

class Session;

/**
 * @brief Manages active client sessions.
 */
class SessionManager : public std::enable_shared_from_this<SessionManager> {
private:
    // Private constructor for singleton
    SessionManager();
    
    // Structure to allow creation of shared_ptr with private constructor
    struct SessionManagerCreator {
        static std::shared_ptr<SessionManager> create() {
            return std::shared_ptr<SessionManager>(new SessionManager());
        }
    };
    
public:
    // Singleton instance
    static std::shared_ptr<SessionManager> getInstance();
    
    // Add a session
    void addSession(std::shared_ptr<Session> session);
    
    // Remove a session
    void removeSession(const std::string& sessionId);
    
    // Get a session by ID
    std::shared_ptr<Session> getSession(const std::string& sessionId);
    
    // Get all sessions
    std::map<std::string, std::shared_ptr<Session>> getAllSessions() const;
    
    // Broadcast a message to all sessions
    void broadcastMessage(const std::string& message, const std::string& senderSessionId = "");
    
    // Public destructor
    ~SessionManager() = default;

private:
    static std::shared_ptr<SessionManager> instance;
    std::map<std::string, std::shared_ptr<Session>> sessions;
    mutable std::mutex mutex;
    
    // Friend declaration for the creator
    friend struct SessionManagerCreator;
};

} // namespace ChatServer
 