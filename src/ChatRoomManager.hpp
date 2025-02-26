/**
 * @file ChatRoomManager.hpp
 * @brief Declaration of the ChatRoomManager class.
 */

 #ifndef CHATROOMMANAGER_HPP
 #define CHATROOMMANAGER_HPP
 
 #include "ChatRoom.hpp"
 #include <unordered_map>
 #include <memory>
 #include <string>
 #include <mutex>
 
 namespace ChatServer {
 
 /**
  * @brief Manages multiple chat rooms.
  */
 class ChatRoomManager {
 public:
     ChatRoomManager();
     ~ChatRoomManager();
 
     std::shared_ptr<ChatRoom> createChatRoom(const std::string& name);
     void deleteChatRoom(const std::string& name);
     std::shared_ptr<ChatRoom> getChatRoom(const std::string& name) const;
 
 private:
     std::unordered_map<std::string, std::shared_ptr<ChatRoom>> chatRooms;
     mutable std::mutex mutex_;
 };
 
 } // namespace ChatServer
 
 #endif // CHATROOMMANAGER_HPP
 