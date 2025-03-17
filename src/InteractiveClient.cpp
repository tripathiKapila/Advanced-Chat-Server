#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <deque>
#include <windows.h>
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

// ASCII Art for the welcome screen
const std::string WELCOME_ART = R"(
  _  __          _ _ _       ___ _           _   
 | |/ /__ _ _ __(_) ( )___  / __| |__   __ _| |_ 
 | ' // _` | '_ \ | |// __| \__ \ '_ \ / _` | __|
 | . \ (_| | |_) | |  \__ \ ___) | | | (_| | |_ 
 |_|\_\__,_| .__/|_|  |___/|____/|_|  \__,_|\__|
           |_|                                   
             INTERACTIVE CLIENT
)";

// Motivational quotes for random display
const std::vector<std::string> QUOTES = {
    "The best way to predict the future is to create it.",
    "The only limit to our realization of tomorrow is our doubts of today.",
    "Success is not final, failure is not fatal: It is the courage to continue that counts.",
    "Believe you can and you're halfway there.",
    "The future belongs to those who believe in the beauty of their dreams."
};

class ConsoleUI {
public:
    ConsoleUI() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        originalAttributes = csbi.wAttributes;
        
        clearScreen();
        drawHeader();
        
        // Calculate message area dimensions
        CONSOLE_SCREEN_BUFFER_INFO screen;
        GetConsoleScreenBufferInfo(hConsole, &screen);
        
        int totalHeight = screen.dwSize.Y;
        int totalWidth = screen.dwSize.X;
        
        // Set up the layout
        serverPanelTop = 7;  // After header
        serverPanelHeight = (totalHeight - serverPanelTop - 5) / 2;  // Half the remaining space minus input area
        clientPanelTop = serverPanelTop + serverPanelHeight + 1;
        clientPanelHeight = serverPanelHeight;
        inputLineY = clientPanelTop + clientPanelHeight + 1;
        
        // Draw the initial layout
        drawLayout(totalWidth);
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
        
        // Set console buffer size to a reasonable value
        COORD bufferSize;
        bufferSize.X = 120;  // Width
        bufferSize.Y = 40;   // Height
        SetConsoleScreenBufferSize(hConsole, bufferSize);
        
        // Set console window size
        SMALL_RECT windowSize;
        windowSize.Left = 0;
        windowSize.Top = 0;
        windowSize.Right = 119;
        windowSize.Bottom = 39;
        SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
    }
    
    void setColor(Color color) {
        SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
    }
    
    void resetColor() {
        SetConsoleTextAttribute(hConsole, originalAttributes);
    }
    
    void drawHeader() {
        setColor(Color::CYAN);
        std::cout << WELCOME_ART << std::endl;
        resetColor();
        
        // Display a random quote
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, QUOTES.size() - 1);
        int quoteIndex = dis(gen);
        
        setColor(Color::YELLOW);
        std::cout << "  \"" << QUOTES[quoteIndex] << "\"" << std::endl;
        resetColor();
    }
    
    void drawLayout(int width) {
        // Draw server panel header
        COORD pos = { 0, static_cast<SHORT>(serverPanelTop - 1) };
        SetConsoleCursorPosition(hConsole, pos);
        
        setColor(Color::GREEN);
        std::cout << " SERVER MESSAGES ";
        setColor(Color::DARK_GREEN);
        for (int i = 15; i < width; i++) {
            std::cout << "=";
        }
        resetColor();
        
        // Draw client panel header
        pos = { 0, static_cast<SHORT>(clientPanelTop - 1) };
        SetConsoleCursorPosition(hConsole, pos);
        
        setColor(Color::BLUE);
        std::cout << " CLIENT ACTIVITY ";
        setColor(Color::DARK_BLUE);
        for (int i = 16; i < width; i++) {
            std::cout << "=";
        }
        resetColor();
        
        // Draw input area separator
        pos = { 0, static_cast<SHORT>(inputLineY - 1) };
        SetConsoleCursorPosition(hConsole, pos);
        
        setColor(Color::YELLOW);
        std::cout << " INPUT ";
        setColor(Color::DARK_YELLOW);
        for (int i = 7; i < width; i++) {
            std::cout << "=";
        }
        resetColor();
    }
    
    void addServerMessage(const std::string& message) {
        std::lock_guard<std::mutex> lock(serverMutex);
        
        // Get current time
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        
        // Add message to queue
        Message msg;
        msg.timestamp = ss.str();
        msg.content = message;
        
        serverMessages.push_back(msg);
        
        // Keep only the last N messages
        if (serverMessages.size() > static_cast<size_t>(serverPanelHeight)) {
            serverMessages.pop_front();
        }
        
        // Redraw server messages
        redrawServerMessages();
    }
    
    void addClientMessage(const std::string& message, bool isOutgoing = false) {
        std::lock_guard<std::mutex> lock(clientMutex);
        
        // Get current time
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        
        // Add message to queue
        Message msg;
        msg.timestamp = ss.str();
        msg.content = message;
        msg.isOutgoing = isOutgoing;
        
        clientMessages.push_back(msg);
        
        // Keep only the last N messages
        if (clientMessages.size() > static_cast<size_t>(clientPanelHeight)) {
            clientMessages.pop_front();
        }
        
        // Redraw client messages
        redrawClientMessages();
    }
    
    void redrawServerMessages() {
        CONSOLE_SCREEN_BUFFER_INFO screen;
        GetConsoleScreenBufferInfo(hConsole, &screen);
        int width = screen.dwSize.X;
        
        // Clear server panel
        for (int i = 0; i < serverPanelHeight; i++) {
            COORD pos = { 0, static_cast<SHORT>(serverPanelTop + i) };
            SetConsoleCursorPosition(hConsole, pos);
            for (int j = 0; j < width; j++) {
                std::cout << " ";
            }
        }
        
        // Display messages
        int startIdx = 0;
        if (serverMessages.size() > static_cast<size_t>(serverPanelHeight)) {
            startIdx = serverMessages.size() - serverPanelHeight;
        }
        
        for (size_t i = startIdx; i < serverMessages.size(); i++) {
            const auto& msg = serverMessages[i];
            int lineIdx = serverPanelTop + (i - startIdx);
            
            COORD linePos = { 0, static_cast<SHORT>(lineIdx) };
            SetConsoleCursorPosition(hConsole, linePos);
            
            // Timestamp
            setColor(Color::GRAY);
            std::cout << "[" << msg.timestamp << "] ";
            
            // Message content - limit to screen width
            setColor(Color::GREEN);
            std::string content = msg.content;
            if (content.length() > static_cast<size_t>(width - 12)) {
                content = content.substr(0, width - 15) + "...";
            }
            std::cout << content;
            resetColor();
        }
        
        // Reset cursor position to input line
        COORD inputPos = { 2, static_cast<SHORT>(inputLineY) };
        SetConsoleCursorPosition(hConsole, inputPos);
    }
    
    void redrawClientMessages() {
        CONSOLE_SCREEN_BUFFER_INFO screen;
        GetConsoleScreenBufferInfo(hConsole, &screen);
        int width = screen.dwSize.X;
        
        // Clear client panel
        for (int i = 0; i < clientPanelHeight; i++) {
            COORD pos = { 0, static_cast<SHORT>(clientPanelTop + i) };
            SetConsoleCursorPosition(hConsole, pos);
            for (int j = 0; j < width; j++) {
                std::cout << " ";
            }
        }
        
        // Display messages
        int startIdx = 0;
        if (clientMessages.size() > static_cast<size_t>(clientPanelHeight)) {
            startIdx = clientMessages.size() - clientPanelHeight;
        }
        
        for (size_t i = startIdx; i < clientMessages.size(); i++) {
            const auto& msg = clientMessages[i];
            int lineIdx = clientPanelTop + (i - startIdx);
            
            COORD linePos = { 0, static_cast<SHORT>(lineIdx) };
            SetConsoleCursorPosition(hConsole, linePos);
            
            // Timestamp
            setColor(Color::GRAY);
            std::cout << "[" << msg.timestamp << "] ";
            
            // Direction indicator
            if (msg.isOutgoing) {
                setColor(Color::MAGENTA);
                std::cout << "[SENT] ";
            } else {
                setColor(Color::CYAN);
                std::cout << "[RECV] ";
            }
            
            // Message content - limit to screen width
            if (msg.isOutgoing) {
                setColor(Color::MAGENTA);
            } else {
                setColor(Color::CYAN);
            }
            std::string content = msg.content;
            if (content.length() > static_cast<size_t>(width - 20)) {
                content = content.substr(0, width - 23) + "...";
            }
            std::cout << content;
            resetColor();
        }
        
        // Reset cursor position to input line
        COORD inputPos = { 2, static_cast<SHORT>(inputLineY) };
        SetConsoleCursorPosition(hConsole, inputPos);
    }
    
    void clearInputLine() {
        CONSOLE_SCREEN_BUFFER_INFO screen;
        GetConsoleScreenBufferInfo(hConsole, &screen);
        int width = screen.dwSize.X;
        
        COORD pos = { 0, static_cast<SHORT>(inputLineY) };
        SetConsoleCursorPosition(hConsole, pos);
        
        for (int i = 0; i < width; i++) {
            std::cout << " ";
        }
        
        pos = { 2, static_cast<SHORT>(inputLineY) };
        SetConsoleCursorPosition(hConsole, pos);
        
        setColor(Color::WHITE);
        std::cout << "> ";
        resetColor();
    }
    
    void showConnectionStatus(bool connected, const std::string& host, int port) {
        CONSOLE_SCREEN_BUFFER_INFO screen;
        GetConsoleScreenBufferInfo(hConsole, &screen);
        int width = screen.dwSize.X;
        
        COORD pos = { 0, static_cast<SHORT>(inputLineY + 1) };
        SetConsoleCursorPosition(hConsole, pos);
        
        for (int i = 0; i < width; i++) {
            std::cout << " ";
        }
        
        SetConsoleCursorPosition(hConsole, pos);
        
        if (connected) {
            setColor(Color::GREEN);
            std::cout << " Connected to " << host << ":" << port;
        } else {
            setColor(Color::RED);
            std::cout << " Disconnected";
        }
        resetColor();
        
        // Reset cursor position to input line
        COORD inputPos = { 2, static_cast<SHORT>(inputLineY) };
        SetConsoleCursorPosition(hConsole, inputPos);
    }
    
    void showTypingPrompt() {
        clearInputLine();
        
        COORD pos = { 0, static_cast<SHORT>(inputLineY) };
        SetConsoleCursorPosition(hConsole, pos);
        
        setColor(Color::WHITE);
        std::cout << "> ";
        resetColor();
    }
    
private:
    HANDLE hConsole;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    WORD originalAttributes;
    
    int serverPanelTop;
    int serverPanelHeight;
    int clientPanelTop;
    int clientPanelHeight;
    int inputLineY;
    
    struct Message {
        std::string timestamp;
        std::string content;
        bool isOutgoing = false;
    };
    
    std::deque<Message> serverMessages;
    std::deque<Message> clientMessages;
    std::mutex serverMutex;
    std::mutex clientMutex;
};

class InteractiveClient {
public:
    InteractiveClient(const std::string& host = "localhost", int port = 8080)
        : host_(host), port_(port), connected_(false), running_(false) {
        
        // Initialize UI
        ui_ = std::make_unique<ConsoleUI>();
        ui_->showConnectionStatus(false, host_, port_);
        ui_->addClientMessage("Client started. Type /connect to connect to the server.", false);
        ui_->addClientMessage("Type /help for available commands.", false);
    }
    
    ~InteractiveClient() {
        disconnect();
    }
    
    void run() {
        running_ = true;
        
        while (running_) {
            ui_->showTypingPrompt();
            std::string input;
            std::getline(std::cin, input);
            
            if (input.empty()) {
                continue;
            }
            
            // Process commands
            if (input[0] == '/') {
                processCommand(input);
            } else {
                // Regular message
                if (connected_) {
                    sendMessage(input);
                } else {
                    ui_->addClientMessage("Not connected to server. Type /connect to connect.", false);
                }
            }
        }
    }
    
private:
    void processCommand(const std::string& command) {
        std::string cmd = command;
        std::string args;
        
        size_t spacePos = command.find(' ');
        if (spacePos != std::string::npos) {
            cmd = command.substr(0, spacePos);
            args = command.substr(spacePos + 1);
        }
        
        if (cmd == "/connect") {
            if (connected_) {
                ui_->addClientMessage("Already connected to server.", false);
            } else {
                connect();
            }
        } else if (cmd == "/disconnect") {
            if (connected_) {
                disconnect();
                ui_->addClientMessage("Disconnected from server.", false);
            } else {
                ui_->addClientMessage("Not connected to server.", false);
            }
        } else if (cmd == "/exit" || cmd == "/quit") {
            ui_->addClientMessage("Exiting client...", false);
            disconnect();
            running_ = false;
        } else if (cmd == "/help") {
            showHelp();
        } else if (cmd == "/server") {
            if (args.empty()) {
                ui_->addClientMessage("Current server: " + host_ + ":" + std::to_string(port_), false);
            } else {
                // Parse host:port
                size_t colonPos = args.find(':');
                if (colonPos != std::string::npos) {
                    std::string newHost = args.substr(0, colonPos);
                    std::string portStr = args.substr(colonPos + 1);
                    
                    try {
                        int newPort = std::stoi(portStr);
                        if (newPort > 0 && newPort < 65536) {
                            if (connected_) {
                                disconnect();
                            }
                            
                            host_ = newHost;
                            port_ = newPort;
                            ui_->addClientMessage("Server set to " + host_ + ":" + std::to_string(port_), false);
                            ui_->showConnectionStatus(false, host_, port_);
                        } else {
                            ui_->addClientMessage("Invalid port number. Must be between 1 and 65535.", false);
                        }
                    } catch (const std::exception&) {
                        ui_->addClientMessage("Invalid port number format.", false);
                    }
                } else {
                    ui_->addClientMessage("Invalid format. Use /server host:port", false);
                }
            }
        } else {
            // Forward other commands to server if connected
            if (connected_) {
                sendMessage(command);
            } else {
                ui_->addClientMessage("Unknown command: " + cmd, false);
                ui_->addClientMessage("Type /help for available commands.", false);
            }
        }
    }
    
    void showHelp() {
        ui_->addClientMessage("Available client commands:", false);
        ui_->addClientMessage("/connect - Connect to the server", false);
        ui_->addClientMessage("/disconnect - Disconnect from the server", false);
        ui_->addClientMessage("/server [host:port] - Show or change server address", false);
        ui_->addClientMessage("/exit or /quit - Exit the client", false);
        ui_->addClientMessage("/help - Show this help message", false);
        ui_->addClientMessage("", false);
        ui_->addClientMessage("Server commands (when connected):", false);
        ui_->addClientMessage("/join <room_name> - Join a chat room", false);
        ui_->addClientMessage("/leave <room_name> - Leave a chat room", false);
        ui_->addClientMessage("/listrooms - List all available chat rooms", false);
        ui_->addClientMessage("/createroom <room_name> - Create a new chat room", false);
        ui_->addClientMessage("/whisper <session_id> <message> - Send a private message", false);
        ui_->addClientMessage("/listusers <room_name> - List all users in a chat room", false);
        ui_->addClientMessage("/nickname <new_nickname> - Change your display name", false);
    }
    
    void connect() {
        try {
            ui_->addClientMessage("Connecting to " + host_ + ":" + std::to_string(port_) + "...", false);
            
            // Create socket and connect
            io_context_ = std::make_unique<boost::asio::io_context>();
            socket_ = std::make_unique<tcp::socket>(*io_context_);
            
            tcp::resolver resolver(*io_context_);
            auto endpoints = resolver.resolve(host_, std::to_string(port_));
            
            boost::asio::connect(*socket_, endpoints);
            
            connected_ = true;
            ui_->showConnectionStatus(true, host_, port_);
            ui_->addClientMessage("Connected to server.", false);
            
            // Start receiving messages
            receive_thread_ = std::thread([this]() {
                receiveMessages();
            });
            
        } catch (const std::exception& e) {
            ui_->addClientMessage("Error connecting to server: " + std::string(e.what()), false);
            connected_ = false;
            ui_->showConnectionStatus(false, host_, port_);
        }
    }
    
    void disconnect() {
        if (connected_) {
            connected_ = false;
            
            if (socket_ && socket_->is_open()) {
                boost::system::error_code ec;
                socket_->shutdown(tcp::socket::shutdown_both, ec);
                socket_->close(ec);
            }
            
            if (receive_thread_.joinable()) {
                receive_thread_.join();
            }
            
            ui_->showConnectionStatus(false, host_, port_);
        }
    }
    
    void sendMessage(const std::string& message) {
        if (!connected_ || !socket_ || !socket_->is_open()) {
            ui_->addClientMessage("Not connected to server.", false);
            return;
        }
        
        try {
            boost::asio::write(*socket_, boost::asio::buffer(message + "\n"));
            ui_->addClientMessage(message, true);
        } catch (const std::exception& e) {
            ui_->addClientMessage("Error sending message: " + std::string(e.what()), false);
            disconnect();
        }
    }
    
    void receiveMessages() {
        try {
            boost::asio::streambuf buffer;
            while (connected_ && socket_ && socket_->is_open()) {
                boost::system::error_code ec;
                boost::asio::read_until(*socket_, buffer, '\n', ec);
                
                if (ec) {
                    if (ec != boost::asio::error::eof) {
                        ui_->addClientMessage("Error receiving message: " + ec.message(), false);
                    }
                    break;
                }
                
                std::istream is(&buffer);
                std::string message;
                std::getline(is, message);
                
                // Add to both client and server panels
                ui_->addClientMessage(message, false);
                ui_->addServerMessage(message);
            }
        } catch (const std::exception& e) {
            if (connected_) {
                ui_->addClientMessage("Connection error: " + std::string(e.what()), false);
            }
        }
        
        connected_ = false;
        ui_->showConnectionStatus(false, host_, port_);
        ui_->addClientMessage("Disconnected from server.", false);
    }
    
    std::string host_;
    int port_;
    bool connected_;
    bool running_;
    
    std::unique_ptr<ConsoleUI> ui_;
    std::unique_ptr<boost::asio::io_context> io_context_;
    std::unique_ptr<tcp::socket> socket_;
    std::thread receive_thread_;
};

int main(int argc, char* argv[]) {
    std::string host = "localhost";
    int port = 8080;
    
    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        try {
            port = std::stoi(argv[2]);
        } catch (const std::exception&) {
            std::cerr << "Invalid port number. Using default port 8080." << std::endl;
        }
    }
    
    try {
        InteractiveClient client(host, port);
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 