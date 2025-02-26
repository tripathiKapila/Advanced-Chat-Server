/**
 * @file ChatRoom.cpp
 * @brief Implementation of the ChatRoom class.
 */

 #include "ChatRoom.hpp"
 #include <algorithm>
 #include <mutex>
 
 namespace ChatServer {
 
 ChatRoom::ChatRoom(const std::string& name) : roomName(name) {}
 
 ChatRoom::~ChatRoom() {}
 
 void ChatRoom::addSession(const std::string& sessionId) {
     std::lock_guard<std::mutex> lock(mutex_);
     sessions.push_back(sessionId);
 }
 
 void ChatRoom::removeSession(const std::string& sessionId) {
     std::lock_guard<std::mutex> lock(mutex_);
     sessions.erase(std::remove(sessions.begin(), sessions.end(), sessionId), sessions.end());
 }
 
 std::string ChatRoom::getName() const {
     return roomName;
 }
 
 std::vector<std::string> ChatRoom::getSessions() const {
     std::lock_guard<std::mutex> lock(mutex_);
     return sessions;
 }
 
 } // namespace ChatServer
 