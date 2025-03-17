/**
 * @file ChatRoom.hpp
 * @brief Declaration of the ChatRoom class.
 */

#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <mutex>

namespace ChatServer {

class Session;
class SessionManager;

/**
 * @brief Represents a chat room for client sessions.
 */
class ChatRoom {
public:
    ChatRoom(const std::string& name);
    ~ChatRoom() = default;

    // Add a session to the chat room
    void addSession(const std::string& sessionId);
    
    // Remove a session from the chat room
    void removeSession(const std::string& sessionId);
    
    // Broadcast a message to all sessions in the chat room
    void broadcastMessage(const std::string& message, const std::string& senderSessionId);
    
    // Get the name of the chat room
    const std::string& getName() const;
    
    // Get all sessions in the chat room
    std::set<std::string> getSessions() const;

private:
    std::string name;
    std::set<std::string> sessions;
    mutable std::mutex mutex;
};

class ChatRoomManager {
public:
    ChatRoomManager();
    ~ChatRoomManager() = default;

    // Create a new chat room
    std::shared_ptr<ChatRoom> createChatRoom(const std::string& name);
    
    // Get a chat room by name
    std::shared_ptr<ChatRoom> getChatRoom(const std::string& name);
    
    // Remove a chat room
    void removeChatRoom(const std::string& name);
    
    // Get all chat rooms
    std::vector<std::shared_ptr<ChatRoom>> getAllRooms() const;

private:
    std::map<std::string, std::shared_ptr<ChatRoom>> chatRooms;
    mutable std::mutex mutex;
};

} // namespace ChatServer
 