/**
 * @file Session.hpp
 * @brief Declaration of the Session class.
 */

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <mutex>
#include <chrono>
#include <boost/asio.hpp>

namespace ChatServer {

class CommandManager;

/**
 * @brief Represents a client session.
 */
class Session : public std::enable_shared_from_this<Session> {
public:
    using MessageHandler = std::function<void(const std::string&, std::shared_ptr<Session>)>;
    
    Session(boost::asio::ip::tcp::socket socket, const std::string& sessionId);
    ~Session();
    
    // Start the session
    void start();
    
    // Send a message to the client
    void sendMessage(const std::string& message);
    
    // Get the session ID
    const std::string& getSessionId() const;
    
    // Set the message handler
    void setMessageHandler(MessageHandler handler);
    
    // Set the command manager
    void setCommandManager(std::shared_ptr<CommandManager> cmdManager);

    // Advanced session handling.
    void updateLastActive();
    bool isTimedOut(std::chrono::seconds timeout) const;

private:
    void readMessage();
    void handleMessage(const std::string& message);
    void writeMessage(const std::string& message);
    
    boost::asio::ip::tcp::socket socket_;
    std::string sessionId_;
    std::string readBuffer_;
    std::string writeBuffer_;
    MessageHandler messageHandler_;
    std::shared_ptr<CommandManager> commandManager_;
    std::mutex writeMutex_;
    bool isWriting_;
    std::chrono::steady_clock::time_point lastActive;
};

} // namespace ChatServer
