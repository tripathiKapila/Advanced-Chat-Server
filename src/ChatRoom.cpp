#include "ChatRoom.hpp"
#include "Session.hpp"
#include <algorithm>

void ChatRoom::join(std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.insert(session);
}

void ChatRoom::leave(std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = sessions_.begin(); it != sessions_.end(); ) {
        if (it->lock() == session)
            it = sessions_.erase(it);
        else
            ++it;
    }
}

void ChatRoom::broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    addToHistory(message);
    for (auto it = sessions_.begin(); it != sessions_.end(); ) {
        if (auto sp = it->lock()) {
            sp->deliver(message);
            ++it;
        } else {
            it = sessions_.erase(it);
        }
    }
}

std::vector<std::string> ChatRoom::getChatHistory() {
    std::lock_guard<std::mutex> lock(mutex_);
    // Return last 10 messages.
    size_t count = chat_history_.size();
    size_t start = (count > 10) ? count - 10 : 0;
    return std::vector<std::string>(chat_history_.begin() + start, chat_history_.end());
}

void ChatRoom::addToHistory(const std::string& message) {
    chat_history_.push_back(message);
}
