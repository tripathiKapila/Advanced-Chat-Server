/**
 * @file ChatRoom.hpp
 * @brief Declaration of the ChatRoom class.
 */

 #ifndef CHATROOM_HPP
 #define CHATROOM_HPP
 
 #include <string>
 #include <vector>
 #include <mutex>
 
 namespace ChatServer {
 
 /**
  * @brief Represents a chat room for client sessions.
  */
 class ChatRoom {
 public:
     explicit ChatRoom(const std::string& name);
     ~ChatRoom();
 
     void addSession(const std::string& sessionId);
     void removeSession(const std::string& sessionId);
     std::string getName() const;
     std::vector<std::string> getSessions() const;
 
 private:
     std::string roomName;
     std::vector<std::string> sessions;
     mutable std::mutex mutex_;
 };
 
 } // namespace ChatServer
 
 #endif // CHATROOM_HPP
 