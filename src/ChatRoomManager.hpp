#ifndef CHATROOMMANAGER_HPP
#define CHATROOMMANAGER_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "ChatRoom.hpp"

class Session; // forward declaration

class ChatRoomManager {
public:
    static ChatRoomManager& instance() {
        static ChatRoomManager instance;
        return instance;
    }
    // Add a session to a room (creating the room if necessary).
    std::shared_ptr<ChatRoom> joinRoom(const std::string& room_name, std::shared_ptr<Session> session);
    void leaveRoom(const std::string& room_name, std::shared_ptr<Session> session);
    std::shared_ptr<ChatRoom> getRoom(const std::string& room_name);
private:
    ChatRoomManager() {}
    std::unordered_map<std::string, std::shared_ptr<ChatRoom>> rooms_;
    std::mutex mutex_;
};

#endif // CHATROOMMANAGER_HPP
