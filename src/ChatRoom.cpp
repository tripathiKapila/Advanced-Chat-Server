/**
 * @file ChatRoom.cpp
 * @brief Implementation of the ChatRoom class.
 */

#include "ChatRoom.hpp"
#include "Session.hpp"
#include "SessionManager.hpp"
#include "Logging.hpp"
#include <algorithm>
#include <mutex>

namespace ChatServer {

// ChatRoom implementation
ChatRoom::ChatRoom(const std::string& name) : name(name) {
    Logging::info("Created chat room: " + name);
}

void ChatRoom::addSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex);
    sessions.insert(sessionId);
    Logging::info("Session " + sessionId + " added to room " + name);
}

void ChatRoom::removeSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex);
    sessions.erase(sessionId);
    Logging::info("Session " + sessionId + " removed from room " + name);
}

void ChatRoom::broadcastMessage(const std::string& message, const std::string& senderSessionId) {
    std::set<std::string> sessionsCopy;
    {
        std::lock_guard<std::mutex> lock(mutex);
        sessionsCopy = sessions;
    }
    
    // Get the SessionManager instance to access sessions
    auto sessionManager = SessionManager::getInstance();
    
    for (const auto& sessionId : sessionsCopy) {
        // Don't send the message back to the sender
        if (sessionId != senderSessionId) {
            auto session = sessionManager->getSession(sessionId);
            if (session) {
                session->sendMessage(message);
            }
        }
    }
    
    Logging::info("Message broadcast in room " + name + " by session " + senderSessionId);
}

const std::string& ChatRoom::getName() const {
    return name;
}

std::set<std::string> ChatRoom::getSessions() const {
    std::lock_guard<std::mutex> lock(mutex);
    return sessions;
}

// ChatRoomManager implementation
ChatRoomManager::ChatRoomManager() {
    Logging::info("ChatRoomManager initialized");
}

std::shared_ptr<ChatRoom> ChatRoomManager::createChatRoom(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check if the room already exists
    auto it = chatRooms.find(name);
    if (it != chatRooms.end()) {
        return it->second;
    }
    
    // Create a new chat room
    auto room = std::make_shared<ChatRoom>(name);
    chatRooms[name] = room;
    
    Logging::info("Chat room created: " + name);
    return room;
}

std::shared_ptr<ChatRoom> ChatRoomManager::getChatRoom(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = chatRooms.find(name);
    if (it != chatRooms.end()) {
        return it->second;
    }
    
    return nullptr;
}

void ChatRoomManager::removeChatRoom(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = chatRooms.find(name);
    if (it != chatRooms.end()) {
        chatRooms.erase(it);
        Logging::info("Chat room removed: " + name);
    }
}

std::vector<std::shared_ptr<ChatRoom>> ChatRoomManager::getAllRooms() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::shared_ptr<ChatRoom>> rooms;
    rooms.reserve(chatRooms.size());
    
    for (const auto& pair : chatRooms) {
        rooms.push_back(pair.second);
    }
    
    return rooms;
}

} // namespace ChatServer
 