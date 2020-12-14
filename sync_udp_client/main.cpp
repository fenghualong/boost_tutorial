#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>

using boost::asio::ip::udp;

int main() {
    boost::asio::io_context io_context;

    udp::resolver resolver{io_context};
//    udp::resolver::query query(udp::v4(),"localhost", "daytime");
//    udp::endpoint receiver_endpoint = *resolver.resolve(query);

    auto endpoints = resolver.resolve(udp::v4(),"localhost", "daytime");
    for(auto&& endpoint : endpoints) {
        std::cout << endpoint.service_name() << " "
                  << endpoint.host_name() << " "
                  << endpoint.endpoint()
                  << std::endl;
    }

    udp::socket socket{io_context};
    const auto connected_endpoint = boost::asio::connect(socket, endpoints);
    std::cout << connected_endpoint << std::endl;

    boost::array<char, 1> send_buf = {0};
    socket.send_to(boost::asio::buffer(send_buf), connected_endpoint);

    boost::array<char, 128> recv_buf;
    udp::endpoint sender_endpoint;
    size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
    std::cout.write(recv_buf.data(), len);
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
