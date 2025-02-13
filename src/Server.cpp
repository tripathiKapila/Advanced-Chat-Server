#include "Server.hpp"
#include "Session.hpp"
#include <boost/log/trivial.hpp>
#include <iostream>

Server::Server(boost::asio::io_context& io_context, unsigned short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    BOOST_LOG_TRIVIAL(info) << "Server created on port " << port;
}

void Server::start() {
    BOOST_LOG_TRIVIAL(info) << "Server is starting to accept connections...";
    do_accept();
}

void Server::stop() {
    boost::system::error_code ec;
    acceptor_.close(ec);
    if (ec)
        BOOST_LOG_TRIVIAL(error) << "Error closing acceptor: " << ec.message();
    else
        BOOST_LOG_TRIVIAL(info) << "Acceptor closed successfully.";
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            try {
                if (!ec) {
                    BOOST_LOG_TRIVIAL(info) << "Accepted new connection.";
                    std::make_shared<Session>(std::move(socket))->start();
                } else {
                    BOOST_LOG_TRIVIAL(error) << "Accept error: " << ec.message();
                }
            } catch (std::exception& e) {
                BOOST_LOG_TRIVIAL(error) << "Exception in do_accept lambda: " << e.what();
            }
            if (acceptor_.is_open())
                do_accept();
        }
    );
}
