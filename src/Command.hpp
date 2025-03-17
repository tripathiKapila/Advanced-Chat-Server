#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace ChatServer {

// Forward declarations
class Session;
class ChatRoomManager;
class UserManager;

/**
 * @brief Base class for all commands
 */
class Command {
public:
    virtual ~Command() = default;
    
    /**
     * @brief Execute the command
     * @param session The session that issued the command
     * @param args Command arguments
     * @return Response message
     */
    virtual std::string execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) = 0;
    
    /**
     * @brief Get command usage information
     * @return Usage string
     */
    virtual std::string getUsage() const = 0;
};

/**
 * @brief Command factory and registry
 */
class CommandManager {
public:
    CommandManager(std::shared_ptr<ChatRoomManager> chatRoomManager, 
                  std::shared_ptr<UserManager> userManager);
    
    /**
     * @brief Process a command string
     * @param session The session that issued the command
     * @param commandStr The raw command string
     * @return Response message
     */
    std::string processCommand(std::shared_ptr<Session> session, const std::string& commandStr);
    
    /**
     * @brief Register a new command
     * @param name Command name
     * @param command Command implementation
     */
    void registerCommand(const std::string& name, std::shared_ptr<Command> command);
    
    /**
     * @brief Get help information for all commands
     * @return Help text
     */
    std::string getHelp() const;

private:
    std::unordered_map<std::string, std::shared_ptr<Command>> commands;
    std::shared_ptr<ChatRoomManager> chatRoomManager;
    std::shared_ptr<UserManager> userManager;
    
    /**
     * @brief Parse a command string into command name and arguments
     * @param commandStr The raw command string
     * @return Pair of command name and arguments vector
     */
    std::pair<std::string, std::vector<std::string>> parseCommand(const std::string& commandStr);
};

} // namespace ChatServer

#endif // COMMAND_HPP 