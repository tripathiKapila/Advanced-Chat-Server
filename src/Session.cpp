/**
 * @file Session.cpp
 * @brief Implementation of the Session class with advanced session handling.
 */

#include "Session.hpp"
#include "Command.hpp"
#include "Logging.hpp"
#include <iostream>
#include <chrono>

namespace ChatServer {

Session::Session(boost::asio::ip::tcp::socket socket, const std::string& sessionId)
    : socket_(std::move(socket)), 
      sessionId_(sessionId), 
      isWriting_(false),
      lastActive(std::chrono::steady_clock::now()) {
    Logging::info("Session created: " + sessionId_);
}

Session::~Session() {
    Logging::info("Session destroyed: " + sessionId_);
}

void Session::start() {
    Logging::info("Session started: " + sessionId_);
    readMessage();
}

void Session::sendMessage(const std::string& message) {
    boost::asio::post(socket_.get_executor(), [self = shared_from_this(), message]() {
        self->writeMessage(message);
    });
}

const std::string& Session::getSessionId() const {
    return sessionId_;
}

void Session::setMessageHandler(MessageHandler handler) {
    messageHandler_ = std::move(handler);
}

void Session::setCommandManager(std::shared_ptr<CommandManager> cmdManager) {
    commandManager_ = cmdManager;
}

void Session::updateLastActive() {
    lastActive = std::chrono::steady_clock::now();
}

bool Session::isTimedOut(std::chrono::seconds timeout) const {
    return (std::chrono::steady_clock::now() - lastActive) > timeout;
}

void Session::readMessage() {
    readBuffer_.resize(1024);
    
    socket_.async_read_some(
        boost::asio::buffer(readBuffer_),
        [this, self = shared_from_this()](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                std::string message(readBuffer_.data(), length);
                handleMessage(message);
                updateLastActive(); // Update last active time on read
                readMessage(); // Continue reading
            } else {
                Logging::error("Read error in session " + sessionId_ + ": " + ec.message());
                // Handle disconnection
            }
        });
}

void Session::handleMessage(const std::string& message) {
    Logging::info("Message received from session " + sessionId_ + ": " + message);
    
    // Check if this is a command (starts with '/')
    if (!message.empty() && message[0] == '/') {
        if (commandManager_) {
            std::string response = commandManager_->processCommand(shared_from_this(), message.substr(1));
            sendMessage(response);
        } else {
            sendMessage("Command processing is not available.");
        }
    } else if (messageHandler_) {
        // Handle as a regular message
        messageHandler_(message, shared_from_this());
    }
}

void Session::writeMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(writeMutex_);
    
    bool writeInProgress = isWriting_;
    if (!writeInProgress) {
        isWriting_ = true;
    }
    
    writeBuffer_ = message + "\n";
    
    if (!writeInProgress) {
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(writeBuffer_),
            [this, self = shared_from_this()](boost::system::error_code ec, std::size_t /*length*/) {
                std::lock_guard<std::mutex> lock(writeMutex_);
                
                if (!ec) {
                    isWriting_ = false;
                    updateLastActive(); // Update last active time on write
                } else {
                    Logging::error("Write error in session " + sessionId_ + ": " + ec.message());
                    // Handle disconnection
                }
            });
    }
}

} // namespace ChatServer
