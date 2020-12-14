#include <iostream>
#include <ctime>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <utility>
#include <memory>
#include <boost/algorithm/string/case_conv.hpp>

using boost::asio::ip::tcp;

/*------------daytime server----------*/
std::string make_daytime_string() {
    using namespace std;
    time_t now = time(0);
    return ctime(&now);
}


class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_context& io_context) {
        return pointer(new tcp_connection(io_context));
    }

    tcp::socket& socket() {
        return socket_;
    }

    void start() {
        message_ = make_daytime_string();
        boost::asio::async_write(socket_, boost::asio::buffer(message_),
                boost::bind(&tcp_connection::handle_write, shared_from_this(),
                        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

private:
    tcp_connection(boost::asio::io_context& io_context): socket_(io_context) {}

    void handle_write(const boost::system::error_code& ec, size_t len) {

    }

    tcp::socket socket_;
    std::string message_;
};

class tcp_server {
public:
    tcp_server()
             : acceptor_(io_context, tcp::endpoint(tcp::v4(), 1300)) {
        start_accept();
    }

    void run() {
        io_context.run();
    }

private:
    void start_accept() {
        tcp_connection::pointer new_connection = tcp_connection::create(io_context);

        acceptor_.async_accept(new_connection->socket(),
                boost::bind(&tcp_server::handle_accept, this, new_connection,
                boost::asio::placeholders::error));
    }
    void handle_accept(tcp_connection::pointer new_connection,
            const boost::system::error_code ec) {
        if(!ec) {
            new_connection->start();
            start_accept();
        }

    }
    boost::asio::io_context io_context;
    tcp::acceptor acceptor_;
};

/*------------echo server----------*/
class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket): socket(std::move(socket)) {}

    void read() {
        boost::asio::async_read_until(socket, boost::asio::dynamic_buffer(message), "\n",
                [self = shared_from_this()](boost::system::error_code ec, std::size_t len) {
            if(ec || self->message == "\n") {
                self->socket.close();
                return;
            }
            boost::algorithm::to_upper(self->message);
            self->write();
        });
    }

    void write() {
        boost::asio::async_write(socket, boost::asio::buffer(message),
                [self = shared_from_this()](boost::system::error_code ec, std::size_t len) {
            if(ec) return;
            self->message.clear();
            self->read();
        });
    }
private:
    tcp::socket socket;
    std::string message;
};

void echo_server(tcp::acceptor& acceptor) {
    acceptor.async_accept([&acceptor](boost::system::error_code ec, tcp::socket socket){
        echo_server(acceptor);
        if(ec) return;
        std::shared_ptr<Session> session;
        session = std::make_shared<Session>(std::move(socket));
        session->read();
    });
}


int main() {

//    tcp_server server;
//    server.run();

    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor{io_context, tcp::endpoint(tcp::v4(), 1895)};
        echo_server(acceptor);
        io_context.run();
    }catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    std::cout << "Hello, World!" << std::endl;



    return 0;
}
