#include "Session.hpp"
#include "ChatRoomManager.hpp"
#include "UserManager.hpp"
#include "SessionManager.hpp"
#include <boost/log/trivial.hpp>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Session::Session(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket))
    // Initialize the strand with the socket's executor.
    , write_strand_(boost::asio::make_strand(socket_.get_executor()))
{
    BOOST_LOG_TRIVIAL(debug) << "New session created.";
}

void Session::start() {
    try {
        BOOST_LOG_TRIVIAL(info) << "Session started with remote endpoint: "
                                << socket_.remote_endpoint().address().to_string() << ":"
                                << socket_.remote_endpoint().port();
    } catch (std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << "Error obtaining remote endpoint: " << e.what();
    }
    do_read();
}

void Session::do_read() {
    auto self = shared_from_this();
    socket_.async_read_some(
        boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                read_buffer_.append(data_, length);
                process_read_buffer();
                do_read();
            } else {
                BOOST_LOG_TRIVIAL(error) << "Read error: " << ec.message();
                if (authenticated_)
                    SessionManager::instance().removeSession(username_);
                // Leave any rooms that this session has joined.
                for (const auto& room : joined_rooms_) {
                    ChatRoomManager::instance().leaveRoom(room, self);
                }
            }
        }
    );
}

void Session::process_read_buffer() {
    size_t pos;
    while ((pos = read_buffer_.find('\n')) != std::string::npos) {
        std::string line = read_buffer_.substr(0, pos);
        read_buffer_.erase(0, pos + 1);
        process_command(line);
    }
}

void Session::process_command(const std::string& command_line) {
    BOOST_LOG_TRIVIAL(info) << "Received command: " << command_line;
    
    try {
        auto j = json::parse(command_line);
        std::string command = j["command"];

        if (command == "REGISTER") {
            std::string user = j["username"];
            std::string pass = j["password"];
            if (UserManager::instance().registerUser(user, pass)) {
                BOOST_LOG_TRIVIAL(info) << "User registered: " << user;
                send_json({{"response", "REGISTER_OK"}});
            } else {
                BOOST_LOG_TRIVIAL(info) << "Registration failed (user exists): " << user;
                send_json({{"response", "ERROR"}, {"message", "Username already exists"}});
                // Optionally, close the socket for duplicate registration.
                // socket_.close();
            }
        } else if (command == "LOGIN") {
            std::string user = j["username"];
            std::string pass = j["password"];
            if (UserManager::instance().authenticate(user, pass)) {
                authenticated_ = true;
                username_ = user;
                SessionManager::instance().addSession(user, shared_from_this());
                send_json({{"response", "LOGIN_OK"}});
            } else {
                send_json({{"response", "ERROR"}, {"message", "Invalid credentials"}});
            }
        } else if (command == "JOIN") {
            std::string room = j["room"];
            auto chatRoom = ChatRoomManager::instance().joinRoom(room, shared_from_this());
            joined_rooms_.insert(room);
            auto history = chatRoom->getChatHistory();
            send_json({{"response", "JOIN_OK"}, {"room", room}, {"history", history}}); 
        } else if (command == "CHAT") {
            std::string room = j["room"];
            std::string message = j["message"];
            if (joined_rooms_.find(room) == joined_rooms_.end()) {
                send_json({{"response", "ERROR"}, {"message", "Not in room " + room}}); 
                return;
            }
            std::string fullMessage = "CHAT from " + username_ + ": " + message;
            auto chatRoom = ChatRoomManager::instance().getRoom(room);
            if (chatRoom)
                chatRoom->broadcast(fullMessage);
            else
                send_json({{"response", "ERROR"}, {"message", "Room does not exist"}});
        } else if (command == "WHISPER") {
            std::string target = j["target"];
            std::string message = j["message"];
            auto targetSession = SessionManager::instance().getSession(target);
            if (targetSession) {
                std::string fullMessage = "WHISPER from " + username_ + ": " + message;
                targetSession->deliver(fullMessage);
                send_json({{"response", "WHISPER_OK"}});
            } else {
                send_json({{"response", "ERROR"}, {"message", "User not found"}});
            }
        } else if (command == "LIST") {
            auto users = SessionManager::instance().listSessions();
            send_json({{"response", "LIST_OK"}, {"users", users}}); 
        } else if (command == "KICK") {
            if (username_ != "admin") {
                send_json({{"response", "ERROR"}, {"message", "Not authorized"}});
                return;
            }
            std::string target = j["target"];
            auto targetSession = SessionManager::instance().getSession(target);
            if (targetSession) {
                targetSession->deliver("You have been kicked by admin");
                targetSession->getSocket().close();
                SessionManager::instance().removeSession(target);
                send_json({{"response", "KICK_OK"}});
            } else {
                send_json({{"response", "ERROR"}, {"message", "User not found"}});
            }
        } else {
            send_json({{"response", "ERROR"}, {"message", "Unknown command"}});
        }
    } catch (std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << "Exception in process_command: " << e.what();
        send_json({{"response", "ERROR"}, {"message", "Invalid command format"}});
    }
}

void Session::deliver(const std::string& message) {
    auto self = shared_from_this();
    json j;
    j["response"] = "MESSAGE";
    j["message"] = message;
    std::string msg = j.dump() + "\n";

    // Post the write operation on the strand to ensure serial execution.
    boost::asio::post(write_strand_,
        [this, self, msg]() {
            bool write_in_progress = !write_msgs_.empty();
            write_msgs_.push_back(msg);
            if (!write_in_progress)
                do_write_queue();
        }
    );
}

void Session::do_write_queue() {
    auto self = shared_from_this();

    if (write_msgs_.empty())
        return;

    std::string msg = write_msgs_.front();
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(msg),
        boost::asio::bind_executor(write_strand_,
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    write_msgs_.pop_front();
                    if (!write_msgs_.empty())
                        do_write_queue();
                } else {
                    BOOST_LOG_TRIVIAL(error) << "Write error: " << ec.message();
                    socket_.close();
                }
            }
        )
    );
}

void Session::send_json(const json& j) {
    std::string msg = j.dump() + "\n";
    auto self = shared_from_this();
    boost::asio::post(write_strand_,
        [this, self, msg]() {
            bool write_in_progress = !write_msgs_.empty();
            write_msgs_.push_back(msg);
            if (!write_in_progress)
                do_write_queue();
        }
    );
}
