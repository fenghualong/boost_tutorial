#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <string>
#include <istream>
#include <ostream>

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {

//    if(argc != 2) {
//        std::cerr << "Usage: "<< argv[0] <<" <host>" << std::endl;
//        return 1;
//    }

    std::string host = "www.baidu.com";

    boost::asio::io_context io_context;
    try{
        tcp::resolver resolver{io_context};
        tcp::socket socket(io_context);
        boost::system::error_code ec;
        auto endpoints = resolver.resolve(host, "http", ec);
        for(auto&& endpoint : endpoints) {
            std::cout << endpoint.service_name() << " "
                      << endpoint.host_name() << " "
                      << endpoint.endpoint()
                      << std::endl;
        }
        if(ec) {
            std::cout << "Error code: " << ec << std::endl;
            return 1;
        }
        const auto connected_endpoint = boost::asio::connect(socket, endpoints);
        std::cout << connected_endpoint << std::endl;

        std::stringstream request_stream;
        request_stream << "GET / HTTP/1.1\r\n"
                          "Host: " << host << "\r\n"
                          "Accept: text/html\r\n"
                          "Accept-Language: en-us\r\n"
                          "Accept-Encoding: identity\r\n"
                          "Connection: close\r\n\r\n";
        const auto request = request_stream.str();

        boost::asio::write(socket, boost::asio::buffer(request), ec);

        // method 1 to read
        for (;;) {
            boost::array<char, 128> buf;
            boost::system::error_code ec;
            size_t len = socket.read_some(boost::asio::buffer(buf), ec);
            if (ec == boost::asio::error::eof) {
                std::cout << "read EOF" << std::endl;
                break; //Connection closed cleanly by peer.
            } else if (ec) {
                throw boost::system::system_error(ec);
            }

            std::cout.write(buf.data(), len);
        }

        //method 2 to read
        std::string response;
        //boost::asio::connect(socket, endpoints);
        boost::asio::write(socket, boost::asio::buffer(request), ec);
        boost::asio::read(socket, boost::asio::dynamic_buffer(response), ec);
        if(ec && ec.value() != 2) throw boost::system::system_error(ec);
        std::cout << response << std::endl;


    }catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
