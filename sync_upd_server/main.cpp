#include <iostream>
#include <ctime>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string() {
    using namespace std;
    time_t now = time(0);
    return ctime(&now);
}

int main() {
    boost::asio::io_context io_context;
    boost::system::error_code ec;

    udp::socket socket{io_context, udp::endpoint(udp::v4(), 13)};

    for (;;) {
        boost::array<char, 1> recv_buf;
        udp::endpoint remote_endpoint;
        socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint, 0, ec);
        if(ec && ec != boost::asio::error::message_size) {
            std::cerr << "Error:" << ec.message() << std::endl;
            return 1;
        }

        std::string message = make_daytime_string();

        socket.send_to(boost::asio::buffer(message), remote_endpoint, 0, ec);

    }

}
