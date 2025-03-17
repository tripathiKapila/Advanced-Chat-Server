#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Command.hpp"
#include "ChatRoom.hpp"
#include "UserManager.hpp"
#include "SessionManager.hpp"

namespace ChatServer {

/**
 * @brief Command to join a chat room
 */
class JoinCommand : public Command {
public:
    JoinCommand(std::shared_ptr<ChatRoomManager> chatRoomManager);
    
    std::string execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) override;
    std::string getUsage() const override;
    
private:
    std::shared_ptr<ChatRoomManager> chatRoomManager;
};

/**
 * @brief Command to leave a chat room
 */
class LeaveCommand : public Command {
public:
    LeaveCommand(std::shared_ptr<ChatRoomManager> chatRoomManager);
    
    std::string execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) override;
    std::string getUsage() const override;
    
private:
    std::shared_ptr<ChatRoomManager> chatRoomManager;
};

/**
 * @brief Command to list available chat rooms
 */
class ListRoomsCommand : public Command {
public:
    ListRoomsCommand(std::shared_ptr<ChatRoomManager> chatRoomManager);
    
    std::string execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) override;
    std::string getUsage() const override;
    
private:
    std::shared_ptr<ChatRoomManager> chatRoomManager;
};

/**
 * @brief Command to create a new chat room
 */
class CreateRoomCommand : public Command {
public:
    CreateRoomCommand(std::shared_ptr<ChatRoomManager> chatRoomManager);
    
    std::string execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) override;
    std::string getUsage() const override;
    
private:
    std::shared_ptr<ChatRoomManager> chatRoomManager;
};

/**
 * @brief Command to send a private message to another user
 */
class WhisperCommand : public Command {
public:
    WhisperCommand(std::shared_ptr<SessionManager> sessionManager);
    
    std::string execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) override;
    std::string getUsage() const override;
    
private:
    std::shared_ptr<SessionManager> sessionManager;
};

/**
 * @brief Command to list users in a chat room
 */
class ListUsersCommand : public Command {
public:
    ListUsersCommand(std::shared_ptr<ChatRoomManager> chatRoomManager, 
                    std::shared_ptr<SessionManager> sessionManager);
    
    std::string execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) override;
    std::string getUsage() const override;
    
private:
    std::shared_ptr<ChatRoomManager> chatRoomManager;
    std::shared_ptr<SessionManager> sessionManager;
};

/**
 * @brief Command to change user nickname
 */
class NicknameCommand : public Command {
public:
    NicknameCommand(std::shared_ptr<UserManager> userManager);
    
    std::string execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) override;
    std::string getUsage() const override;
    
private:
    std::shared_ptr<UserManager> userManager;
};

/**
 * @brief Initialize and register all commands
 * @param commandManager The command manager to register commands with
 * @param chatRoomManager Chat room manager instance
 * @param userManager User manager instance
 * @param sessionManager Session manager instance
 */
void registerCommands(CommandManager& commandManager,
                     std::shared_ptr<ChatRoomManager> chatRoomManager,
                     std::shared_ptr<UserManager> userManager,
                     std::shared_ptr<SessionManager> sessionManager);

} // namespace ChatServer

#endif // COMMANDS_HPP 