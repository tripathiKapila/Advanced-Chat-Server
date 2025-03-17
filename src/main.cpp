/**
 * @file main.cpp
 * @brief Entry point for the Integrated Chat Server application.
 */

#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <deque>
#include <windows.h>
#include <atomic>
#include <vector>
#include <random>

#include "Logging.hpp"
#include "ChatRoom.hpp"
#include "Session.hpp"
#include "SessionManager.hpp"
#include "UserManager.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "ThreadPool.hpp"

using boost::asio::ip::tcp;

// Console colors
enum class Color {
    RED = 12,
    GREEN = 10,
    BLUE = 9,
    CYAN = 11,
    MAGENTA = 13,
    YELLOW = 14,
    WHITE = 15,
    GRAY = 8,
    DARK_RED = 4,
    DARK_GREEN = 2,
    DARK_BLUE = 1,
    DARK_CYAN = 3,
    DARK_MAGENTA = 5,
    DARK_YELLOW = 6,
    BLACK = 0
};

// ASCII Art for the server
const std::string SERVER_ART = R"(
  _  __          _ _ _       ___ _           _   
 | |/ /__ _ _ __(_) ( )___  / __| |__   __ _| |_ 
 | ' // _` | '_ \ | |// __| \__ \ '_ \ / _` | __|
 | . \ (_| | |_) | |  \__ \ ___) | | | (_| | |_ 
 |_|\_\__,_| .__/|_|  |___/|____/|_|  \__,_|\__|
           |_|                                   
                 UNIFIED CHAT SERVER
)";

// Server stats
struct ServerStats {
    std::atomic<int> totalConnections{0};
    std::atomic<int> activeConnections{0};
    std::atomic<int> messagesProcessed{0};
    std::atomic<int> bytesReceived{0};
    std::atomic<int> bytesSent{0};
    std::chrono::system_clock::time_point startTime;
    
    ServerStats() {
        startTime = std::chrono::system_clock::now();
    }
    
    std::string getUptime() const {
        auto now = std::chrono::system_clock::now();
        auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        
        int days = uptime / (24 * 3600);
        uptime %= (24 * 3600);
        int hours = uptime / 3600;
        uptime %= 3600;
        int minutes = uptime / 60;
        int seconds = uptime % 60;
        
        std::stringstream ss;
        if (days > 0) ss << days << "d ";
        ss << std::setfill('0') << std::setw(2) << hours << ":"
           << std::setfill('0') << std::setw(2) << minutes << ":"
           << std::setfill('0') << std::setw(2) << seconds;
        
        return ss.str();
    }
};

// Console UI for the server
class ConsoleUI {
public:
    ConsoleUI() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        originalAttributes = csbi.wAttributes;
        
        clearScreen();
        drawHeader();
        messageAreaEnd = csbi.dwSize.Y - 3;
    }
    
    ~ConsoleUI() {
        SetConsoleTextAttribute(hConsole, originalAttributes);
    }
    
    void clearScreen() {
        COORD topLeft = { 0, 0 };
        CONSOLE_SCREEN_BUFFER_INFO screen;
        DWORD written;
        
        GetConsoleScreenBufferInfo(hConsole, &screen);
        FillConsoleOutputCharacterA(
            hConsole, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
        );
        FillConsoleOutputAttribute(
            hConsole, csbi.wAttributes, screen.dwSize.X * screen.dwSize.Y, topLeft, &written
        );
        SetConsoleCursorPosition(hConsole, topLeft);
    }
    
    void setColor(Color color) {
        SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
    }
    
    void resetColor() {
        SetConsoleTextAttribute(hConsole, originalAttributes);
    }
    
    void drawHeader() {
        setColor(Color::CYAN);
        std::cout << SERVER_ART << std::endl;
        resetColor();
        
        CONSOLE_SCREEN_BUFFER_INFO screen;
        GetConsoleScreenBufferInfo(hConsole, &screen);
        int width = screen.dwSize.X;
        
        setColor(Color::DARK_CYAN);
        for (int i = 0; i < width; i++) {
            std::cout << "=";
        }
        std::cout << std::endl;
        resetColor();
    }
    
    void drawStatusBar() {
        CONSOLE_SCREEN_BUFFER_INFO screen;
        GetConsoleScreenBufferInfo(hConsole, &screen);
        int width = screen.dwSize.X;
        
        COORD statusPos = { 0, static_cast<SHORT>(screen.dwSize.Y - 2) };
        SetConsoleCursorPosition(hConsole, statusPos);
        
        setColor(Color::DARK_CYAN);
        for (int i = 0; i < width; i++) {
            std::cout << "=";
        }
        resetColor();
    }
    
    void updateStatus(const ServerStats& stats) {
        std::lock_guard<std::mutex> lock(statusMutex);
        
        CONSOLE_SCREEN_BUFFER_INFO screen;
        GetConsoleScreenBufferInfo(hConsole, &screen);
        int width = screen.dwSize.X;
        
        COORD statusPos = { 0, static_cast<SHORT>(screen.dwSize.Y - 1) };
        SetConsoleCursorPosition(hConsole, statusPos);
        
        // Clear the status line
        for (int i = 0; i < width; i++) {
            std::cout << " ";
        }
        SetConsoleCursorPosition(hConsole, statusPos);
        
        // Format the status line
        std::stringstream ss;
        ss << "Uptime: " << stats.getUptime() 
           << " | Connections: " << stats.activeConnections << "/" << stats.totalConnections
           << " | Messages: " << stats.messagesProcessed
           << " | Data: " << (stats.bytesReceived / 1024) << "KB in, " 
           << (stats.bytesSent / 1024) << "KB out";
        
        std::string status = ss.str();
        
        // Truncate if too long
        if (status.length() > static_cast<size_t>(width)) {
            status = status.substr(0, width - 3) + "...";
        }
        
        setColor(Color::YELLOW);
        std::cout << status;
        resetColor();
        
        // Reset cursor position to not interfere with message display
        COORD resetPos = { 0, 0 };
        SetConsoleCursorPosition(hConsole, resetPos);
    }
    
    void addMessage(const std::string& type, const std::string& message, bool isError = false) {
        std::lock_guard<std::mutex> lock(messagesMutex);
        
        // Get current time
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        
        // Add message to queue
        ServerMessage msg;
        msg.timestamp = ss.str();
        msg.type = type;
        msg.message = message;
        msg.isError = isError;
        
        messages.push_back(msg);
        
        // Keep only the last 100 messages
        if (messages.size() > 100) {
            messages.pop_front();
        }
        
        // Redraw messages
        redrawMessages();
    }
    
    void redrawMessages() {
        CONSOLE_SCREEN_BUFFER_INFO screen;
        GetConsoleScreenBufferInfo(hConsole, &screen);
        
        // Clear message area
        COORD topLeft = { 0, 7 }; // Start after header
        DWORD written;
        
        int messageAreaHeight = messageAreaEnd - 7;
        FillConsoleOutputCharacterA(
            hConsole, ' ', screen.dwSize.X * messageAreaHeight, topLeft, &written
        );
        FillConsoleOutputAttribute(
            hConsole, originalAttributes, screen.dwSize.X * messageAreaHeight, topLeft, &written
        );
        
        // Display messages
        int startIdx = 0;
        if (messages.size() > static_cast<size_t>(messageAreaHeight)) {
            startIdx = messages.size() - messageAreaHeight;
        }
        
        for (size_t i = startIdx; i < messages.size(); i++) {
            const auto& msg = messages[i];
            int lineIdx = 7 + (i - startIdx);
            
            COORD linePos = { 0, static_cast<SHORT>(lineIdx) };
            SetConsoleCursorPosition(hConsole, linePos);
            
            // Timestamp
            setColor(Color::GRAY);
            std::cout << "[" << msg.timestamp << "] ";
            
            // Message type
            if (msg.isError) {
                setColor(Color::RED);
            } else if (msg.type == "INFO") {
                setColor(Color::GREEN);
            } else if (msg.type == "SYSTEM") {
                setColor(Color::YELLOW);
            } else if (msg.type == "DEBUG") {
                setColor(Color::CYAN);
            } else {
                setColor(Color::WHITE);
            }
            
            std::cout << "[" << msg.type << "] ";
            
            // Message content
            if (msg.isError) {
                setColor(Color::RED);
            } else {
                resetColor();
            }
            
            std::cout << msg.message;
        }
        
        // Reset cursor position
        COORD resetPos = { 0, 0 };
        SetConsoleCursorPosition(hConsole, resetPos);
    }
    
private:
    HANDLE hConsole;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    WORD originalAttributes;
    int messageAreaEnd;
    
    struct ServerMessage {
        std::string timestamp;
        std::string type;
        std::string message;
        bool isError;
    };
    
    std::deque<ServerMessage> messages;
    std::mutex messagesMutex;
    std::mutex statusMutex;
};

class UnifiedChatServer {
public:
    UnifiedChatServer(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          socket_(io_context),
          io_context_(io_context),
          running_(true) {
        
        // Initialize UI
        ui_ = std::make_unique<ConsoleUI>();
        
        // Initialize managers
        chatRoomManager_ = std::make_shared<ChatServer::ChatRoomManager>();
        userManager_ = std::make_shared<ChatServer::UserManager>();
        sessionManager_ = ChatServer::SessionManager::getInstance();
        
        // Initialize command manager
        commandManager_ = std::make_shared<ChatServer::CommandManager>(chatRoomManager_, userManager_);
        
        // Register commands
        ChatServer::registerCommands(*commandManager_, chatRoomManager_, userManager_, sessionManager_);
        
        // Create a default chat room
        chatRoomManager_->createChatRoom("general");
        
        // Log initialization
        Logging::info("Unified Chat Server initialized on port " + std::to_string(port));
        ui_->addMessage("INFO", "Server initialized on port " + std::to_string(port));
        
        // Start accepting connections
        doAccept();
        
        // Start status update thread
        status_thread_ = std::thread([this]() {
            while (running_) {
                ui_->updateStatus(stats_);
                ui_->drawStatusBar();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    }
    
    ~UnifiedChatServer() {
        running_ = false;
        if (status_thread_.joinable()) {
            status_thread_.join();
        }
        ui_->addMessage("SYSTEM", "Server shutting down...");
    }

private:
    void doAccept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec) {
                stats_.totalConnections++;
                stats_.activeConnections++;
                
                // Create a unique session ID
                std::string sessionId = "user_" + std::to_string(nextSessionId_++);
                
                ui_->addMessage("INFO", "New connection accepted: " + sessionId);
                
                // Create a new session
                auto session = std::make_shared<ChatServer::Session>(std::move(socket_), sessionId);
                
                // Set the command manager for the session
                session->setCommandManager(commandManager_);
                
                // Set the message handler
                session->setMessageHandler([this](const std::string& message, std::shared_ptr<ChatServer::Session> sender) {
                    handleMessage(message, sender);
                });
                
                // Add the session to the session manager
                sessionManager_->addSession(session);
                
                // Create a user for the session
                userManager_->createUser(sessionId);
                
                // Start the session
                session->start();
                
                // Send a welcome message
                session->sendMessage("Welcome to the Unified Chat Server! Your session ID is " + sessionId);
                session->sendMessage("Type /help to see available commands");
                
                // Add the user to the default chat room
                auto defaultRoom = chatRoomManager_->getChatRoom("general");
                if (defaultRoom) {
                    defaultRoom->addSession(sessionId);
                    session->sendMessage("You have been added to the 'general' chat room");
                }
            } else {
                ui_->addMessage("ERROR", "Accept error: " + ec.message(), true);
            }
            
            // Continue accepting connections
            doAccept();
        });
    }
    
    void handleMessage(const std::string& message, std::shared_ptr<ChatServer::Session> sender) {
        stats_.messagesProcessed++;
        stats_.bytesReceived += message.length();
        
        // Log the message
        ui_->addMessage("MESSAGE", sender->getSessionId() + ": " + message);
        
        // Find which rooms the user is in
        auto rooms = chatRoomManager_->getAllRooms();
        for (const auto& room : rooms) {
            auto sessions = room->getSessions();
            if (sessions.find(sender->getSessionId()) != sessions.end()) {
                // User is in this room, broadcast the message
                std::string formattedMessage = "[" + sender->getSessionId() + "]: " + message;
                room->broadcastMessage(formattedMessage, sender->getSessionId());
                
                // Update bytes sent
                stats_.bytesSent += formattedMessage.length() * (sessions.size() - 1);
            }
        }
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
    boost::asio::io_context& io_context_;
    
    std::unique_ptr<ConsoleUI> ui_;
    std::shared_ptr<ChatServer::ChatRoomManager> chatRoomManager_;
    std::shared_ptr<ChatServer::UserManager> userManager_;
    std::shared_ptr<ChatServer::SessionManager> sessionManager_;
    std::shared_ptr<ChatServer::CommandManager> commandManager_;
    
    int nextSessionId_ = 1;
    ServerStats stats_;
    
    std::thread status_thread_;
    std::atomic<bool> running_;
};

int main() {
    try {
        // Initialize logging
        Logging::init_logging("build/server.log");
        Logging::info("Starting Unified Chat Server");
        
        // Create and run the server
        boost::asio::io_context io_context;
        UnifiedChatServer server(io_context, 8080);
        
        // Run the server with multiple threads
        const int num_threads = 4;
        std::vector<std::thread> threads;
        
        for (int i = 0; i < num_threads - 1; ++i) {
            threads.emplace_back([&io_context]() {
                io_context.run();
            });
        }
        
        Logging::info("Server running with " + std::to_string(num_threads) + " threads");
        io_context.run();
        
        // Wait for all threads to complete
        for (auto& t : threads) {
            t.join();
        }
    }
    catch (std::exception& e) {
        Logging::fatal("Exception: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}
