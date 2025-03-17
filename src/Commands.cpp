#include "Commands.hpp"
#include "Session.hpp"
#include "Logging.hpp"
#include <sstream>

namespace ChatServer {

// JoinCommand implementation
JoinCommand::JoinCommand(std::shared_ptr<ChatRoomManager> chatRoomManager)
    : chatRoomManager(chatRoomManager) {}

std::string JoinCommand::execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Usage: " + getUsage();
    }
    
    const std::string& roomName = args[0];
    auto room = chatRoomManager->getChatRoom(roomName);
    
    if (!room) {
        return "Chat room '" + roomName + "' does not exist. Use /createroom to create a new room.";
    }
    
    room->addSession(session->getSessionId());
    Logging::info("Session " + session->getSessionId() + " joined room " + roomName);
    
    return "You have joined the chat room: " + roomName;
}

std::string JoinCommand::getUsage() const {
    return "join <room_name> - Join a chat room";
}

// LeaveCommand implementation
LeaveCommand::LeaveCommand(std::shared_ptr<ChatRoomManager> chatRoomManager)
    : chatRoomManager(chatRoomManager) {}

std::string LeaveCommand::execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Usage: " + getUsage();
    }
    
    const std::string& roomName = args[0];
    auto room = chatRoomManager->getChatRoom(roomName);
    
    if (!room) {
        return "Chat room '" + roomName + "' does not exist.";
    }
    
    room->removeSession(session->getSessionId());
    Logging::info("Session " + session->getSessionId() + " left room " + roomName);
    
    return "You have left the chat room: " + roomName;
}

std::string LeaveCommand::getUsage() const {
    return "leave <room_name> - Leave a chat room";
}

// ListRoomsCommand implementation
ListRoomsCommand::ListRoomsCommand(std::shared_ptr<ChatRoomManager> chatRoomManager)
    : chatRoomManager(chatRoomManager) {}

std::string ListRoomsCommand::execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) {
    auto rooms = chatRoomManager->getAllRooms();
    
    if (rooms.empty()) {
        return "No chat rooms available. Use /createroom to create a new room.";
    }
    
    std::stringstream ss;
    ss << "Available chat rooms:\n";
    
    for (const auto& room : rooms) {
        ss << "- " << room->getName() << " (" << room->getSessions().size() << " users)\n";
    }
    
    return ss.str();
}

std::string ListRoomsCommand::getUsage() const {
    return "listrooms - List all available chat rooms";
}

// CreateRoomCommand implementation
CreateRoomCommand::CreateRoomCommand(std::shared_ptr<ChatRoomManager> chatRoomManager)
    : chatRoomManager(chatRoomManager) {}

std::string CreateRoomCommand::execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Usage: " + getUsage();
    }
    
    const std::string& roomName = args[0];
    
    if (chatRoomManager->getChatRoom(roomName)) {
        return "Chat room '" + roomName + "' already exists.";
    }
    
    auto room = chatRoomManager->createChatRoom(roomName);
    Logging::info("Session " + session->getSessionId() + " created room " + roomName);
    
    return "Chat room '" + roomName + "' created successfully. Use /join " + roomName + " to join.";
}

std::string CreateRoomCommand::getUsage() const {
    return "createroom <room_name> - Create a new chat room";
}

// WhisperCommand implementation
WhisperCommand::WhisperCommand(std::shared_ptr<SessionManager> sessionManager)
    : sessionManager(sessionManager) {}

std::string WhisperCommand::execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "Usage: " + getUsage();
    }
    
    const std::string& targetSessionId = args[0];
    auto targetSession = sessionManager->getSession(targetSessionId);
    
    if (!targetSession) {
        return "User with session ID '" + targetSessionId + "' not found.";
    }
    
    // Combine remaining args into the message
    std::stringstream messageStream;
    for (size_t i = 1; i < args.size(); ++i) {
        if (i > 1) messageStream << " ";
        messageStream << args[i];
    }
    
    std::string message = messageStream.str();
    std::string whisperMessage = "[Whisper from " + session->getSessionId() + "]: " + message;
    
    targetSession->sendMessage(whisperMessage);
    Logging::info("Session " + session->getSessionId() + " whispered to " + targetSessionId);
    
    return "Whisper sent to " + targetSessionId + ": " + message;
}

std::string WhisperCommand::getUsage() const {
    return "whisper <session_id> <message> - Send a private message to another user";
}

// ListUsersCommand implementation
ListUsersCommand::ListUsersCommand(std::shared_ptr<ChatRoomManager> chatRoomManager, 
                                 std::shared_ptr<SessionManager> sessionManager)
    : chatRoomManager(chatRoomManager), sessionManager(sessionManager) {}

std::string ListUsersCommand::execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Usage: " + getUsage();
    }
    
    const std::string& roomName = args[0];
    auto room = chatRoomManager->getChatRoom(roomName);
    
    if (!room) {
        return "Chat room '" + roomName + "' does not exist.";
    }
    
    auto sessionIds = room->getSessions();
    
    if (sessionIds.empty()) {
        return "No users in chat room '" + roomName + "'.";
    }
    
    std::stringstream ss;
    ss << "Users in chat room '" << roomName << "':\n";
    
    for (const auto& sessionId : sessionIds) {
        auto userSession = sessionManager->getSession(sessionId);
        if (userSession) {
            ss << "- " << sessionId << "\n";
        }
    }
    
    return ss.str();
}

std::string ListUsersCommand::getUsage() const {
    return "listusers <room_name> - List all users in a chat room";
}

// NicknameCommand implementation
NicknameCommand::NicknameCommand(std::shared_ptr<UserManager> userManager)
    : userManager(userManager) {}

std::string NicknameCommand::execute(std::shared_ptr<Session> session, const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Usage: " + getUsage();
    }
    
    const std::string& newNickname = args[0];
    
    // In a real implementation, you would update the user's nickname in the UserManager
    // For now, we'll just log it
    Logging::info("Session " + session->getSessionId() + " changed nickname to " + newNickname);
    
    return "Your nickname has been changed to: " + newNickname;
}

std::string NicknameCommand::getUsage() const {
    return "nickname <new_nickname> - Change your display name";
}

// Register all commands
void registerCommands(CommandManager& commandManager,
                     std::shared_ptr<ChatRoomManager> chatRoomManager,
                     std::shared_ptr<UserManager> userManager,
                     std::shared_ptr<SessionManager> sessionManager) {
    
    commandManager.registerCommand("join", std::make_shared<JoinCommand>(chatRoomManager));
    commandManager.registerCommand("leave", std::make_shared<LeaveCommand>(chatRoomManager));
    commandManager.registerCommand("listrooms", std::make_shared<ListRoomsCommand>(chatRoomManager));
    commandManager.registerCommand("createroom", std::make_shared<CreateRoomCommand>(chatRoomManager));
    commandManager.registerCommand("whisper", std::make_shared<WhisperCommand>(sessionManager));
    commandManager.registerCommand("listusers", std::make_shared<ListUsersCommand>(chatRoomManager, sessionManager));
    commandManager.registerCommand("nickname", std::make_shared<NicknameCommand>(userManager));
    
    Logging::info("Registered all commands");
}

} // namespace ChatServer 