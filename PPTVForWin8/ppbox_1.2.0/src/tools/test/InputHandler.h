// InputHandler.h

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/bind.hpp>

#include <framework/network/NetName.h>
#include <framework/string/Slice.h>

class InputHandler
{
public:
    InputHandler(
        framework::network::NetName const & addr, 
        bool get = true)
        : socket_(io_svc_)
    {
        try {
            ep_.address(boost::asio::ip::address::from_string(addr.host()));
            ep_.port(addr.port());
            socket_.open(ep_.protocol());
            if (get) {
                socket_.bind(ep_);
                boost::asio::socket_base::non_blocking_io cmd(true);
                socket_.io_control(cmd);
            } else {
                socket_.connect(ep_);
            }
        } catch (std::exception & e) {
            std::cout << "socket: " << e.what() << std::endl;
        }
    }

    size_t get_one(
        std::vector<std::string> & cmd)
    {
        std::string line;
        boost::system::error_code ec;
        line.resize(1024);
        size_t n = socket_.receive(boost::asio::buffer(&line[0], line.size()), 0, ec);
        if (ec && ec != boost::asio::error::would_block) {
            std::cout << "get_one: " << ec.message() << std::endl;
        }
        line.resize(n);
        framework::string::slice<std::string>(line, std::inserter(cmd, cmd.end()), " ");
        return cmd.size();
    }

    size_t put_one(
        std::string const & line)
    {
        boost::system::error_code ec;
#ifdef BOOST_WINDOWS_API
        size_t n = socket_.send_to(boost::asio::buffer(line), ep_, 0, ec);
#else
        size_t n = socket_.send(boost::asio::buffer(line), 0, ec);
#endif
        if (ec) {
            std::cout << "put_one: " << ec.message() << std::endl;
        }
        return n;
    }

private:
    boost::asio::io_service io_svc_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint ep_;
};

