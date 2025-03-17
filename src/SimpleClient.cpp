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
)";

// Motivational quotes for random display
const std::vector<std::string> QUOTES = {
    "The best way to predict the future is to create it.",
    "Innovation distinguishes between a leader and a follower.",
    "Technology is best when it brings people together.",
    "The only way to do great work is to love what you do.",
    "Success is not final, failure is not fatal: It is the courage to continue that counts.",
    "The future belongs to those who believe in the beauty of their dreams.",
    "The only limit to our realization of tomorrow is our doubts of today.",
    "Believe you can and you're halfway there.",
    "Your time is limited, don't waste it living someone else's life.",
    "The greatest glory in living lies not in never falling, but in rising every time we fall."
};

// Get a random quote
std::string getRandomQuote() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, QUOTES.size() - 1);
    return QUOTES[dis(gen)];
}

class ConsoleUI {
public:
    ConsoleUI() {
        // Get console handle
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        
        // Save original attributes
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        originalAttributes = csbi.wAttributes;
        
        // Set console title
        SetConsoleTitle(TEXT("Kapil's Advanced Chat Client"));
        
        // Clear screen and set up UI
        clearScreen();
        showWelcomeScreen();
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
    
    void showWelcomeScreen() {
        // Display ASCII art
        setColor(Color::CYAN);
        std::cout << WELCOME_ART << std::endl;
        
        // Display welcome message
        setColor(Color::YELLOW);
        std::cout << "╔════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                       WELCOME TO KAPIL'S CHAT ROOM                         ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        // Display quote
        setColor(Color::MAGENTA);
        std::string quote = getRandomQuote();
        std::cout << "  " << quote << std::endl << std::endl;
        
        // Display instructions
        setColor(Color::WHITE);
        std::cout << "  Connecting to server... Please wait" << std::endl;
        
        // Simulate loading
        for (int i = 0; i < 10; i++) {
            std::cout << ".";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << std::endl << std::endl;
        
        // Press any key to continue
        setColor(Color::GREEN);
        std::cout << "  Press Enter to continue..." << std::endl;
        resetColor();
        std::cin.get();
        
        // Clear screen and draw main UI
        clearScreen();
        drawHeader();
        drawInputBar();
    }
    
    void drawHeader() {
        setColor(Color::CYAN);
        std::cout << "╔════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                       KAPIL'S CHAT CLIENT v1.0                             ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        resetColor();
        
        setColor(Color::GRAY);
        std::cout << " Type your message and press Enter to send. Type '/help' for commands." << std::endl;
        
        // Show current time
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);
        std::stringstream timeStr;
        timeStr << std::put_time(&tm, "%H:%M:%S %d-%m-%Y");
        
        setColor(Color::DARK_CYAN);
        std::cout << " Session started at: " << timeStr.str() << std::endl;
        
        setColor(Color::GRAY);
        std::cout << "════════════════════════════════════════════════════════════════════════════" << std::endl;
        resetColor();
    }
    
    void drawInputBar() {
        // Move cursor to input position
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        
        // Save the current position for message display area
        messageAreaEnd = csbi.dwCursorPosition.Y;
        
        // Draw input bar at the bottom
        setColor(Color::GRAY);
        std::cout << "════════════════════════════════════════════════════════════════════════════" << std::endl;
        setColor(Color::GREEN);
        std::cout << " > ";
        resetColor();
    }
    
    void showHelp() {
        addMessage("SYSTEM", "Available Commands:", true);
        addMessage("HELP", "/help - Show this help message", true);
        addMessage("HELP", "/clear - Clear the screen", true);
        addMessage("HELP", "/exit - Exit the client", true);
        addMessage("HELP", "/about - Show information about this client", true);
        addMessage("HELP", "/quote - Show a random inspirational quote", true);
        addMessage("HELP", "/time - Show current time", true);
        addMessage("HELP", "/whisper <user> <message> - Send private message", true);
        addMessage("HELP", "/nick <name> - Change your nickname", true);
        addMessage("HELP", "/status - Show connection status", true);
    }
    
    void showAbout() {
        addMessage("ABOUT", "Kapil's Advanced Chat Client v1.0", true);
        addMessage("ABOUT", "Developed by: Kapil", true);
        addMessage("ABOUT", "Built with: C++, Boost.Asio", true);
        addMessage("ABOUT", "Features: Colored UI, Command System, Multi-client Support", true);
        addMessage("ABOUT", "GitHub: https://github.com/kapil/AdvancedChatServer", true);
    }
    
    void showQuote() {
        addMessage("QUOTE", getRandomQuote(), true);
    }
    
    void showTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);
        std::stringstream timeStr;
        timeStr << std::put_time(&tm, "%H:%M:%S %d-%m-%Y");
        addMessage("TIME", "Current time: " + timeStr.str(), true);
    }
    
    void addMessage(const std::string& sender, const std::string& message, bool isSystem = false) {
        std::lock_guard<std::mutex> lock(messagesMutex);
        
        // Format timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);
        std::stringstream timeStr;
        timeStr << std::put_time(&tm, "[%H:%M:%S]");
        
        // Add message to history
        messages.push_back({timeStr.str(), sender, message, isSystem});
        
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
        COORD messageAreaStart = {0, 7}; // After header
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
            
            // Draw sender with different colors based on type
            if (msg.isSystem) {
                if (msg.sender == "SYSTEM") {
                    setColor(Color::YELLOW);
                } else if (msg.sender == "ERROR") {
                    setColor(Color::RED);
                } else if (msg.sender == "HELP") {
                    setColor(Color::CYAN);
                } else if (msg.sender == "ABOUT") {
                    setColor(Color::MAGENTA);
                } else if (msg.sender == "QUOTE") {
                    setColor(Color::GREEN);
                } else if (msg.sender == "TIME") {
                    setColor(Color::DARK_CYAN);
                } else {
                    setColor(Color::YELLOW);
                }
                std::cout << "[" << msg.sender << "] ";
            } else if (msg.sender == "You") {
                setColor(Color::GREEN);
                std::cout << msg.sender << ": ";
            } else if (msg.sender.find("Server") != std::string::npos) {
                setColor(Color::YELLOW);
                std::cout << msg.sender << ": ";
            } else {
                // Random but consistent color for each user
                int colorHash = 0;
                for (char c : msg.sender) {
                    colorHash += c;
                }
                Color userColor = static_cast<Color>(9 + (colorHash % 6)); // Range from BLUE to WHITE
                setColor(userColor);
                std::cout << msg.sender << ": ";
            }
            
            // Draw message
            resetColor();
            std::cout << msg.message << std::endl;
        }
        
        // Restore cursor position
        SetConsoleCursorPosition(hConsole, originalPos);
    }
    
    std::string getInput() {
        std::string input;
        std::getline(std::cin, input);
        
        // Clear input line
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        COORD inputStart = {3, csbi.dwCursorPosition.Y - 1}; // Position after the prompt
        DWORD written;
        FillConsoleOutputCharacterA(
            hConsole, ' ', csbi.dwSize.X - 3, inputStart, &written
        );
        
        // Reset cursor position
        SetConsoleCursorPosition(hConsole, {3, csbi.dwCursorPosition.Y - 1});
        
        return input;
    }
    
private:
    HANDLE hConsole;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    WORD originalAttributes;
    int messageAreaEnd;
    
    struct ChatMessage {
        std::string timestamp;
        std::string sender;
        std::string message;
        bool isSystem;
    };
    
    std::deque<ChatMessage> messages;
    std::mutex messagesMutex;
};

class Client {
public:
    Client(boost::asio::io_context& io_context, const std::string& host, const std::string& port)
        : io_context_(io_context), socket_(io_context), running_(true), ui_() {
        
        ui_.addMessage("SYSTEM", "Connecting to " + host + ":" + port + "...", true);
        
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, port);
        
        try {
            boost::asio::connect(socket_, endpoints);
            ui_.addMessage("SYSTEM", "Connected to " + host + ":" + port, true);
            ui_.addMessage("SYSTEM", "Welcome to Kapil's Chat Room! Type /help for available commands.", true);
            
            // Start reading in a separate thread
            read_thread_ = std::thread([this]() {
                this->read_loop();
            });
            
            // Start the message input loop
            message_loop();
            
            // Clean up
            running_ = false;
            if (read_thread_.joinable()) {
                read_thread_.join();
            }
            
        } catch (std::exception& e) {
            ui_.addMessage("ERROR", e.what(), true);
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
    
    ~Client() {
        running_ = false;
        if (read_thread_.joinable()) {
            read_thread_.join();
        }
        if (socket_.is_open()) {
            boost::system::error_code ec;
            socket_.close(ec);
        }
    }
    
private:
    void read_loop() {
        try {
            while (running_ && socket_.is_open()) {
                boost::asio::streambuf buffer;
                boost::system::error_code ec;
                
                // Read until newline
                boost::asio::read_until(socket_, buffer, '\n', ec);
                
                if (ec) {
                    ui_.addMessage("ERROR", "Connection lost: " + ec.message(), true);
                    break;
                }
                
                std::string message;
                std::istream is(&buffer);
                std::getline(is, message);
                
                // Process the message
                if (message.find("Welcome") != std::string::npos) {
                    ui_.addMessage("Server", message, true);
                } else if (message.find("Client") != std::string::npos && message.find("said:") != std::string::npos) {
                    // Extract client ID and message
                    size_t colonPos = message.find(":");
                    if (colonPos != std::string::npos && colonPos + 1 < message.length()) {
                        std::string sender = message.substr(0, colonPos);
                        std::string content = message.substr(colonPos + 1);
                        ui_.addMessage(sender, content);
                    } else {
                        ui_.addMessage("Server", message);
                    }
                } else {
                    ui_.addMessage("Server", message);
                }
            }
        } catch (std::exception& e) {
            ui_.addMessage("ERROR", "Read error: " + std::string(e.what()), true);
        }
    }
    
    void message_loop() {
        try {
            while (running_ && socket_.is_open()) {
                std::string message = ui_.getInput();
                
                if (message.empty()) {
                    continue;
                }
                
                // Handle client-side commands
                if (message == "/clear") {
                    ui_.clearScreen();
                    ui_.drawHeader();
                    ui_.drawInputBar();
                    continue;
                } else if (message == "/help") {
                    ui_.showHelp();
                    continue;
                } else if (message == "/about") {
                    ui_.showAbout();
                    continue;
                } else if (message == "/quote") {
                    ui_.showQuote();
                    continue;
                } else if (message == "/time") {
                    ui_.showTime();
                    continue;
                } else if (message == "/status") {
                    ui_.addMessage("SYSTEM", "Connection status: Active", true);
                    continue;
                } else if (message == "exit" || message == "/exit") {
                    ui_.addMessage("SYSTEM", "Disconnecting...", true);
                    break;
                }
                
                // Add the message to our UI
                ui_.addMessage("You", message);
                
                // Send the message to the server
                message += '\n';  // Add newline for the server to parse
                boost::system::error_code ec;
                boost::asio::write(socket_, boost::asio::buffer(message), ec);
                
                if (ec) {
                    ui_.addMessage("ERROR", "Failed to send message: " + ec.message(), true);
                    break;
                }
            }
        } catch (std::exception& e) {
            ui_.addMessage("ERROR", "Message loop error: " + std::string(e.what()), true);
        }
    }
    
    boost::asio::io_context& io_context_;
    tcp::socket socket_;
    std::thread read_thread_;
    std::atomic<bool> running_;
    ConsoleUI ui_;
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: SimpleClient <host> <port>" << std::endl;
            return 1;
        }
        
        boost::asio::io_context io_context;
        Client client(io_context, argv[1], argv[2]);
        
    } catch (std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
    }
    
    return 0;
} 