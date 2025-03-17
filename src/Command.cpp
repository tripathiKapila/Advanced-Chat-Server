#include "Command.hpp"
#include "ChatRoom.hpp"
#include "UserManager.hpp"
#include "Logging.hpp"
#include <sstream>
#include <algorithm>

namespace ChatServer {

CommandManager::CommandManager(std::shared_ptr<ChatRoomManager> chatRoomManager, 
                             std::shared_ptr<UserManager> userManager)
    : chatRoomManager(chatRoomManager), userManager(userManager) {
    
    // Register built-in commands here
}

std::string CommandManager::processCommand(std::shared_ptr<Session> session, const std::string& commandStr) {
    // Convert command to lowercase for case-insensitive matching
    auto [commandName, args] = parseCommand(commandStr);
    
    std::transform(commandName.begin(), commandName.end(), commandName.begin(), 
                  [](unsigned char c) { return std::tolower(c); });
    
    // Special case for help command
    if (commandName == "help") {
        return getHelp();
    }
    
    auto it = commands.find(commandName);
    if (it == commands.end()) {
        return "Unknown command: /" + commandName + ". Type /help for available commands.";
    }
    
    try {
        return it->second->execute(session, args);
    } catch (const std::exception& e) {
        Logging::error("Error executing command '" + commandName + "': " + e.what());
        return "Error executing command: " + std::string(e.what());
    }
}

void CommandManager::registerCommand(const std::string& name, std::shared_ptr<Command> command) {
    // Convert command name to lowercase for case-insensitive matching
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), 
                  [](unsigned char c) { return std::tolower(c); });
    
    commands[lowerName] = command;
}

std::string CommandManager::getHelp() const {
    std::stringstream ss;
    ss << "Available commands:\n";
    
    for (const auto& [name, cmd] : commands) {
        ss << "/" << name << " - " << cmd->getUsage() << "\n";
    }
    
    ss << "/help - Display this help message";
    return ss.str();
}

std::pair<std::string, std::vector<std::string>> CommandManager::parseCommand(const std::string& commandStr) {
    std::istringstream iss(commandStr);
    std::string commandName;
    iss >> commandName;
    
    std::vector<std::string> args;
    std::string arg;
    
    // Handle quoted arguments
    std::string buffer;
    bool inQuotes = false;
    
    while (iss >> std::ws) {
        char c = iss.get();
        
        if (c == '"') {
            if (inQuotes) {
                // End of quoted argument
                args.push_back(buffer);
                buffer.clear();
                inQuotes = false;
            } else {
                // Start of quoted argument
                inQuotes = true;
            }
        } else if (c == ' ' && !inQuotes) {
            // Space outside quotes - end of argument
            if (!buffer.empty()) {
                args.push_back(buffer);
                buffer.clear();
            }
        } else {
            buffer += c;
        }
        
        if (iss.eof()) {
            break;
        }
    }
    
    // Add the last argument if any
    if (!buffer.empty()) {
        args.push_back(buffer);
    }
    
    return {commandName, args};
}

} // namespace ChatServer 