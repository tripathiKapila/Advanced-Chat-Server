/**
 * @file ChatRoomManager.cpp
 * @brief Implementation of the ChatRoomManager class.
 */

 #include "ChatRoomManager.hpp"
 #include <iostream>
 #include <mutex>
 
 namespace ChatServer {
 
 ChatRoomManager::ChatRoomManager() {}
 
 ChatRoomManager::~ChatRoomManager() {}
 
 std::shared_ptr<ChatRoom> ChatRoomManager::createChatRoom(const std::string& name) {
     auto room = std::make_shared<ChatRoom>(name);
     {
         std::lock_guard<std::mutex> lock(mutex_);
         chatRooms[name] = room;
     }
     std::cout << "Chat room created: " << name << std::endl;
     return room;
 }
 
 void ChatRoomManager::deleteChatRoom(const std::string& name) {
     {
         std::lock_guard<std::mutex> lock(mutex_);
         chatRooms.erase(name);
     }
     std::cout << "Chat room deleted: " << name << std::endl;
 }
 
 std::shared_ptr<ChatRoom> ChatRoomManager::getChatRoom(const std::string& name) const {
     std::lock_guard<std::mutex> lock(mutex_);
     auto it = chatRooms.find(name);
     return (it != chatRooms.end()) ? it->second : nullptr;
 }
 
 } // namespace ChatServer
 