#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

void print(const boost::system::error_code& /*e*/) {
    std::cout << "Hello, World!" << std::endl;
}

int main() {
    boost::asio::io_context io;
    boost::asio::deadline_timer t{io, boost::posix_time::seconds(5)};
    t.async_wait(print);
    io.run();
    return 0;
}
