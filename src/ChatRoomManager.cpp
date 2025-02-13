#include "ChatRoomManager.hpp"
#include "Session.hpp"

std::shared_ptr<ChatRoom> ChatRoomManager::joinRoom(const std::string& room_name, std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rooms_.find(room_name);
    if (it == rooms_.end()) {
        auto room = std::make_shared<ChatRoom>(room_name);
        rooms_[room_name] = room;
        room->join(session);
        return room;
    } else {
        it->second->join(session);
        return it->second;
    }
}

void ChatRoomManager::leaveRoom(const std::string& room_name, std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rooms_.find(room_name);
    if (it != rooms_.end())
        it->second->leave(session);
}

std::shared_ptr<ChatRoom> ChatRoomManager::getRoom(const std::string& room_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rooms_.find(room_name);
    return (it != rooms_.end()) ? it->second : nullptr;
}
