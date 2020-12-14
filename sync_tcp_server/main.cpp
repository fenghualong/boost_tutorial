#include <iostream>
#include <ctime>
#include <string>
#include <boost/asio.hpp>
#include <boost/algorithm/string/case_conv.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
    using namespace std; //For time_t, time and ctime
    time_t now = time(0);
    return ctime(&now);
}

void daytime_server(boost::asio::io_context &io_context) {
    std::cout << "start daytime_server" << std::endl;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13));
    for(;;) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        std::string message = make_daytime_string();
        boost::system::error_code ec;
        boost::asio::write(socket, boost::asio::buffer(message), boost::asio::transfer_all(), ec);
        if(ec) {
            std::cerr << "Error " << ec.message() << std::endl;
        }
    }
}

void echo_server(boost::asio::io_context &io_context) {
    std::cout << "start echo_server" << std::endl;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1895));
    tcp::socket socket{io_context};
    acceptor.accept(socket);
    boost::system::error_code ec;
    std::string message;
    do {
        boost::asio::read_until(socket, boost::asio::dynamic_buffer(message), "\n");
        boost::algorithm::to_upper(message);
        boost::asio::write(socket, boost::asio::buffer(message), ec);
        if(message == "\n") {
            socket.close();
            return;
        }
        message.clear();
    }while(!ec);
}

int main(int argc, char* argv[]) {
    boost::asio::io_context io_context;
//    daytime_server(io_context);  //need sudo permission 1<13<1024
    if(argc == 2 && std::string(argv[1]) == "daytime") {
        daytime_server(io_context);
    }
    echo_server(io_context);
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
