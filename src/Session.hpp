#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include <string>
#include <deque>
#include <set>
#include <mutex>
#include <nlohmann/json.hpp>

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket);
    void start();
    // Called by other modules to send a message to this session.
    void deliver(const std::string& message);
    // For SessionManager to get the username.
    std::string getUsername() const { return username_; }
    // (For the admin KICK command, we expose the socket for forced disconnect.)
    boost::asio::ip::tcp::socket& getSocket() { return socket_; }
private:
    void do_read();
    void process_read_buffer();
    void process_command(const std::string& command_line);
    void do_write_queue();
    void send_json(const nlohmann::json& j);
private:
    boost::asio::ip::tcp::socket socket_;
    // Use a strand (with any_io_executor) to serialize asynchronous writes.
    boost::asio::strand<boost::asio::any_io_executor> write_strand_;
    
    enum { max_length = 1024 };
    char data_[max_length];
    std::string read_buffer_;
    std::deque<std::string> write_msgs_;
    std::mutex write_mutex_;  // Not strictly needed with a strand, but kept for safety.
    bool authenticated_ = false;
    std::string username_;
    std::set<std::string> joined_rooms_; // Names of rooms this session has joined.
};

#endif // SESSION_HPP
