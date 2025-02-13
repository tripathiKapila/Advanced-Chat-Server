#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>

class Server {
public:
    Server(boost::asio::io_context& io_context, unsigned short port);
    void start();
    void stop();
private:
    void do_accept();
    boost::asio::ip::tcp::acceptor acceptor_;
};

#endif // SERVER_HPP
