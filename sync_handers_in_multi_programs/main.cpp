#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <thread>
#include <boost/bind/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class printer {
public:
    printer(boost::asio::io_context& io) : strand_{io},
    timer1_{io, boost::posix_time::seconds(1)},
    timer2_{io, boost::posix_time::seconds(1)},
    count_(0) {
        timer1_.async_wait(boost::bind(&printer::print1, this));
        timer2_.async_wait(boost::bind(&printer::print2, this));
    }

    ~printer() {
        std::cout << "Final count is " << count_ << std::endl;
    }

    void print1() {
        if(count_ < 10) {
            std::cout << "Timer 1: " << count_ << std::endl;
            ++count_;

            timer1_.expires_at(timer1_.expires_at() + boost::posix_time::seconds(1));
            timer1_.async_wait(boost::asio::bind_executor(strand_,boost::bind(&printer::print1, this)));
        }
    }

    void print2() {
        if(count_ < 10) {
            std::cout << "Timer 2: " << count_ << std::endl;
            ++count_;

            timer2_.expires_at(timer2_.expires_at() + boost::posix_time::seconds(1));
            timer2_.async_wait(boost::asio::bind_executor(strand_,boost::bind(&printer::print2, this)));
        }
    }


private:
    boost::asio::deadline_timer timer1_;
    boost::asio::deadline_timer timer2_;
    int count_;
    boost::asio::io_context::strand strand_;

};

int main() {
    boost::asio::io_context io;
    printer p{io};
    std::thread t(boost::bind(&boost::asio::io_context::run, &io));
    io.run();
    t.join();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
