#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <boost/asio.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <deque>
#include <windows.h>
#include <thread>
#include <atomic>
#include <vector>
#include <random>

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
                 SERVER EDITION
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

class ConsoleUI {
public:
    ConsoleUI() {
        // Get console handle
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        
        // Save original attributes
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        originalAttributes = csbi.wAttributes;
        
        // Set console title
        SetConsoleTitle(TEXT("Kapil's Advanced Chat Server"));
        
        // Clear screen and set up UI
        clearScreen();
        drawHeader();
        drawStatusBar();
    }
    
    ~ConsoleUI() {
        // Restore original console attributes
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
            hConsole, originalAttributes,
            screen.dwSize.X * screen.dwSize.Y, topLeft, &written
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
        // Display ASCII art
        setColor(Color::CYAN);
        std::cout << SERVER_ART << std::endl;
        
        setColor(Color::YELLOW);
        std::cout << "╔════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                       KAPIL'S CHAT SERVER v1.0                             ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        resetColor();
        
        setColor(Color::GRAY);
        std::cout << " Server is running. Press Ctrl+C to stop." << std::endl;
        
        // Show current time
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);
        std::stringstream timeStr;
        timeStr << std::put_time(&tm, "%H:%M:%S %d-%m-%Y");
        
        setColor(Color::DARK_CYAN);
        std::cout << " Server started at: " << timeStr.str() << std::endl;
        
        setColor(Color::GRAY);
        std::cout << "════════════════════════════════════════════════════════════════════════════" << std::endl;
        resetColor();
    }
    
    void drawStatusBar() {
        // Move cursor to status bar position
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        
        // Save the current position for message display area
        messageAreaEnd = csbi.dwCursorPosition.Y;
        
        // Draw status bar at the bottom
        setColor(Color::GRAY);
        std::cout << "════════════════════════════════════════════════════════════════════════════" << std::endl;
        resetColor();
    }
    
    void updateStatus(const ServerStats& stats) {
        std::lock_guard<std::mutex> lock(statusMutex);
        
        // Save current cursor position
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        COORD originalPos = csbi.dwCursorPosition;
        
        // Move to status bar
        COORD statusPos = {0, messageAreaEnd};
        SetConsoleCursorPosition(hConsole, statusPos);
        
        // Clear status bar
        DWORD written;
        FillConsoleOutputCharacterA(
            hConsole, ' ', csbi.dwSize.X, statusPos, &written
        );
        
        // Update status - first line
        setColor(Color::GRAY);
        std::cout << " Uptime: ";
        setColor(Color::GREEN);
        std::cout << stats.getUptime();
        
        setColor(Color::GRAY);
        std::cout << " | Active clients: ";
        setColor(Color::GREEN);
        std::cout << stats.activeConnections;
        
        setColor(Color::GRAY);
        std::cout << " | Total connections: ";
        setColor(Color::GREEN);
        std::cout << stats.totalConnections;
        
        // Move to next line
        COORD statusPos2 = {0, messageAreaEnd + 1};
        SetConsoleCursorPosition(hConsole, statusPos2);
        
        // Second line of stats
        setColor(Color::GRAY);
        std::cout << " Messages: ";
        setColor(Color::GREEN);
        std::cout << stats.messagesProcessed;
        
        setColor(Color::GRAY);
        std::cout << " | Data received: ";
        setColor(Color::GREEN);
        std::cout << (stats.bytesReceived / 1024) << " KB";
        
        setColor(Color::GRAY);
        std::cout << " | Data sent: ";
        setColor(Color::GREEN);
        std::cout << (stats.bytesSent / 1024) << " KB";
        
        resetColor();
        
        // Restore cursor position
        SetConsoleCursorPosition(hConsole, originalPos);
    }
    
    void addMessage(const std::string& type, const std::string& message, bool isError = false) {
        std::lock_guard<std::mutex> lock(messagesMutex);
        
        // Format timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);
        std::stringstream timeStr;
        timeStr << std::put_time(&tm, "[%H:%M:%S]");
        
        // Add message to history
        messages.push_back({timeStr.str(), type, message, isError});
        
        // Keep only the last 100 messages
        if (messages.size() > 100) {
            messages.pop_front();
        }
        
        // Redraw messages
        redrawMessages();
    }
    
    void redrawMessages() {
        // Save current cursor position
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        COORD originalPos = csbi.dwCursorPosition;
        
        // Move to message area start
        COORD messageAreaStart = {0, 9}; // After header
        SetConsoleCursorPosition(hConsole, messageAreaStart);
        
        // Clear message area
        DWORD written;
        int messageAreaHeight = messageAreaEnd - messageAreaStart.Y;
        FillConsoleOutputCharacterA(
            hConsole, ' ', csbi.dwSize.X * messageAreaHeight, messageAreaStart, &written
        );
        FillConsoleOutputAttribute(
            hConsole, originalAttributes,
            csbi.dwSize.X * messageAreaHeight, messageAreaStart, &written
        );
        
        // Redraw messages
        SetConsoleCursorPosition(hConsole, messageAreaStart);
        
        // Calculate how many messages we can display
        int maxMessages = messageAreaHeight;
        int startIdx = messages.size() > maxMessages ? messages.size() - maxMessages : 0;
        
        for (size_t i = startIdx; i < messages.size(); i++) {
            const auto& msg = messages[i];
            
            // Draw timestamp
            setColor(Color::GRAY);
            std::cout << msg.timestamp << " ";
            
            // Draw type
            if (msg.isError) {
                setColor(Color::RED);
                std::cout << "[" << msg.type << "] ";
            } else if (msg.type == "INFO") {
                setColor(Color::CYAN);
                std::cout << "[" << msg.type << "] ";
            } else if (msg.type == "CLIENT") {
                setColor(Color::GREEN);
                std::cout << "[" << msg.type << "] ";
            } else if (msg.type == "SYSTEM") {
                setColor(Color::MAGENTA);
                std::cout << "[" << msg.type << "] ";
            } else if (msg.type == "SECURITY") {
                setColor(Color::YELLOW);
                std::cout << "[" << msg.type << "] ";
            } else {
                setColor(Color::YELLOW);
                std::cout << "[" << msg.type << "] ";
            }
            
            // Draw message
            resetColor();
            std::cout << msg.message << std::endl;
        }
        
        // Restore cursor position
        SetConsoleCursorPosition(hConsole, originalPos);
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

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, int id, ConsoleUI& ui, ServerStats& stats)
        : socket_(std::move(socket)), id_(id), ui_(ui), stats_(stats) {
    }

    void start() {
        std::stringstream ss;
        ss << "Client " << id_ << " connected from " 
           << socket_.remote_endpoint().address().to_string() 
           << ":" << socket_.remote_endpoint().port();
        
        ui_.addMessage("INFO", ss.str());
        
        // Increment connection counters
        stats_.totalConnections++;
        stats_.activeConnections++;
        
        // Send welcome message
        std::string welcome = "Welcome to the simple chat server! Your session ID is " + std::to_string(id_) + "\n";
        boost::asio::write(socket_, boost::asio::buffer(welcome));
        stats_.bytesSent += welcome.length();
        
        // Start reading
        do_read();
    }

    ~Session() {
        // Decrement active connections when session is destroyed
        stats_.activeConnections--;
    }

private:
    void do_read() {
        auto self(shared_from_this());
        
        boost::asio::async_read_until(
            socket_,
            buffer_,
            '\n',
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    // Process the message
                    std::string message;
                    std::istream is(&buffer_);
                    std::getline(is, message);
                    
                    // Update stats
                    stats_.bytesReceived += length;
                    stats_.messagesProcessed++;
                    
                    std::stringstream ss;
                    ss << "Client " << id_ << ": " << message;
                    ui_.addMessage("CLIENT", ss.str());
                    
                    // Echo the message back with the client ID
                    std::string response = "Client " + std::to_string(id_) + " said: " + message + "\n";
                    do_write(response);
                    
                    // Continue reading
                    do_read();
                } else {
                    std::stringstream ss;
                    ss << "Client " << id_ << " disconnected: " << ec.message();
                    ui_.addMessage("INFO", ss.str());
                }
            });
    }

    void do_write(const std::string& message) {
        auto self(shared_from_this());
        
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(message),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (ec) {
                    std::stringstream ss;
                    ss << "Error writing to client " << id_ << ": " << ec.message();
                    ui_.addMessage("ERROR", ss.str(), true);
                } else {
                    // Update stats
                    stats_.bytesSent += length;
                }
            });
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    int id_;
    ConsoleUI& ui_;
    ServerStats& stats_;
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          next_id_(1),
          ui_(),
          io_context_(io_context) {
        
        std::stringstream ss;
        ss << "Server started on port " << port;
        ui_.addMessage("SYSTEM", ss.str());
        
        // Display server information
        ui_.addMessage("INFO", "Server version: 1.0.0");
        ui_.addMessage("INFO", "Boost version: " + std::to_string(BOOST_VERSION / 100000) + "." + 
                              std::to_string(BOOST_VERSION / 100 % 1000) + "." + 
                              std::to_string(BOOST_VERSION % 100));
        ui_.addMessage("INFO", "Maximum connections: Unlimited");
        ui_.addMessage("INFO", "Protocol: TCP/IP");
        
        // Start the status update thread
        status_thread_ = std::thread([this]() {
            while (running_) {
                ui_.updateStatus(stats_);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
        
        do_accept();
    }
    
    ~Server() {
        running_ = false;
        if (status_thread_.joinable()) {
            status_thread_.join();
        }
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    // Create a new session and start it
                    std::make_shared<Session>(std::move(socket), next_id_++, ui_, stats_)->start();
                } else {
                    ui_.addMessage("ERROR", "Accept error: " + ec.message(), true);
                }
                
                // Continue accepting
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    int next_id_;
    ConsoleUI ui_;
    boost::asio::io_context& io_context_;
    std::thread status_thread_;
    std::atomic<bool> running_{true};
    ServerStats stats_;
};

int main(int argc, char* argv[]) {
    try {
        short port = 8080;
        if (argc > 1) {
            port = static_cast<short>(std::stoi(argv[1]));
        }
        
        boost::asio::io_context io_context;
        Server server(io_context, port);
        
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
} 