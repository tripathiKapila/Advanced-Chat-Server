#ifndef CHATROOM_HPP
#define CHATROOM_HPP

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <mutex>

class Session; // forward declaration

class ChatRoom {
public:
    ChatRoom(const std::string& name) : name_(name) {}
    void join(std::shared_ptr<Session> session);
    void leave(std::shared_ptr<Session> session);
    void broadcast(const std::string& message);
    // Retrieve recent chat history.
    std::vector<std::string> getChatHistory();
    void addToHistory(const std::string& message);
    const std::string& getName() const { return name_; }
private:
    std::string name_;
    std::set<std::weak_ptr<Session>, std::owner_less<std::weak_ptr<Session>>> sessions_;
    std::vector<std::string> chat_history_;
    std::mutex mutex_;
};

#endif // CHATROOM_HPP
