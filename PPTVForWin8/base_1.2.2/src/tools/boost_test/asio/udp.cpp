// udp.cpp

#include "tools/boost_test/Common.h"

#include <framework/network/NetName.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
using namespace framework::configure;
using namespace framework::logger;
using namespace framework::network;

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
using namespace boost::system;
using namespace boost::asio;
using namespace boost::asio::ip;

typedef unsigned long UInt32;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("TestAsioUdp", framework::logger::Debug);

static inline void print_error(char const * title, error_code const & ec)
{
    LOG_ERROR(title << ": " << ec.message());
}

static inline void print_error(char const * title, system_error const & err)
{
    LOG_ERROR(title << ": " << err.what());
}

static void test_asio_udp_client(
                                 NetName const & addr)
{
    std::string input = "test_asio_udp";

    {
        LOG_DEBUG("input:" << input);
    }

    streambuf buf;
    {
        std::ostream os(&buf);
        os << input;
    }

    try {
        io_service io_svc;
        udp::socket fd(io_svc);
        fd.open(udp::v4());
        fd.connect(udp::endpoint(address::from_string(addr.host()), addr.port()));
        LOG_DEBUG("udp_client: connect: ok");
        size_t len = 0;
        len = fd.send(buf.data());
        LOG_DEBUG("udp_client: sent: len: " << len );
        buf.consume(buf.size());
        len = fd.receive(buf.prepare(1024));
        buf.commit(len);
        LOG_DEBUG("udp_client: receive: len: " << len);
    } catch (boost::system::system_error const & err) {
        print_error("udp_client", err);
        return;
    }

    std::string output;
    {
        std::istream is(&buf);
        is >> output;
    }

    {
        LOG_DEBUG("output:" << output);
    }
};

static void test_asio_tftp_client(
	NetName const & addr)
{
    std::string file = "jstvhdlive_3.flv";
    std::string mode = "octet";

    {
        LOG_DEBUG("file:" << file);
    }

    streambuf buf;
    {
        std::ostream os(&buf);
		os.put(0);
		os.put(1);
		os.write(file.c_str(), file.size() + 1);
		os.write(mode.c_str(), mode.size() + 1);
    }

    try {
        io_service io_svc;
        udp::socket fd(io_svc);
        fd.open(udp::v4());
        fd.bind(udp::endpoint(udp::v4(), 4000));
        LOG_DEBUG("tftp_client: bind: ok");
        size_t send_len = fd.send_to(buf.data(), udp::endpoint(address::from_string(addr.host()), addr.port()));
        LOG_DEBUG("tftp_client: sent: len: " << send_len);
        buf.consume(buf.size());

		address_v4::bytes_type v4_addr; // {192, 168, 35, block / 10};
		v4_addr[0] = 192;
		v4_addr[1] = 168;
		v4_addr[2] = 45;
		v4_addr[3] = 0;

		while (true) {
			udp::endpoint ep_from;
			size_t recv_len = fd.receive_from(buf.prepare(1024), ep_from);
			//LOG_DEBUG("tftp_client: receive: len: " << recv_len);
			buf.commit(recv_len);

			boost::uint16_t opcode = 0, block = 0;
			{
				std::istream is(&buf);
				opcode = (is.get() << 8) | is.get();
				block = (is.get() << 8) | is.get();
			}

			if (opcode != 3) {
				break;
			}

			buf.consume(buf.size());

			{
				std::ostream os(&buf);
				os.put(0);
				os.put(4);
				os.put((char)(block >> 8));
				os.put((char)(block & 0xff));
			}

			send_len = fd.send_to(buf.data(), ep_from);
			//LOG_DEBUG("tftp_client: sent: len: " << send_len);

			if ((block % 10) == 0) {
                LOG_DEBUG("tftp_client: receive: block: " << block);
				//v4_addr[3] = 40;//block / 10;
				//send_len = fd.send_to(buf.data(), udp::endpoint(address_v4(v4_addr), 4000));
				//LOG_DEBUG("tftp_client: sent2222: len: " << send_len);
			}

			buf.consume(buf.size());

			if (recv_len < 4 + 512) {
				break;
			}
		}
    } catch (boost::system::system_error const & err) {
        print_error("tftp_client", err);
        return;
    }
};

static void test_asio_udp_server(
                                 NetName const & addr)
{
    try {
        io_service io_svc;
        udp::socket fd(io_svc);
        fd.open(udp::v4());
        fd.bind(udp::endpoint(address::from_string(addr.host()), addr.port()));
        LOG_DEBUG("udp_server: bind: ok");
        streambuf buf;
        udp::endpoint endp;
        size_t len = fd.receive_from(buf.prepare(1024), endp);
        LOG_DEBUG("udp_server: receive_from: len: " << len);
        buf.commit(len);
        len = fd.send_to(buf.data(), endp);
        LOG_DEBUG("udp_server: send_to: len: " << len);
        buf.consume(buf.size());
    } catch (boost::system::system_error const & err) {
        print_error("udp_server", err);
        return;
    }
}


class test_asio_async_udp_client_t
{
public:
    test_asio_async_udp_client_t(
        io_service & io_svc)
        : fd(io_svc)
    {
    }

    void start(
        NetName const & addr)
    {
        std::string input = "test_asio_async_udp";
        LOG_DEBUG("input:" << input);
        {
            std::ostream os(&buf);
            os << input;
        }
        error_code ec;
        fd.open(udp::v4(), ec);
        if (!ec)
            fd.connect(udp::endpoint(address::from_string(addr.host()), addr.port()), ec);
        if (ec) {
            handle_error(ec);
        } else {
			LOG_DEBUG("async_udp_client: connect: ok");
            fd.async_send(buf.data(), 
                boost::bind(&test_asio_async_udp_client_t::handle_send, this, _1, _2));
        }
    }

private:
    void handle_send(
        error_code const & ec, 
        size_t len)
    {
        if (ec) {
            handle_error(ec);
        } else {
			LOG_DEBUG("async_udp_client: send: len: " << len);
            buf.consume(buf.size());
            fd.async_receive(buf.prepare(1024), 
                boost::bind(&test_asio_async_udp_client_t::handle_receive, this, _1, _2));
        }
    }

    void handle_receive(
        error_code const & ec, 
        size_t len)
    {
        if (ec) {
            handle_error(ec);
        } else {
			LOG_DEBUG("async_udp_client: receive: len: " << len);
            buf.commit(len);
            std::string output;
            {
                std::istream is(&buf);
                is >> output;
            }
            LOG_DEBUG("output:" << output);
            delete this;
        }
    }

    void handle_error(
        error_code const & ec)
    {
        print_error("async_udp_client", ec);
        delete this;
    }

private:
    udp::socket fd;
    streambuf buf;
};

static void test_asio_async_udp_client(
                                       io_service & io_svc, 
                                       NetName const & addr)
{
    (new test_asio_async_udp_client_t(io_svc))->start(addr);
};

class test_asio_async_udp_server_t
{
public:
    test_asio_async_udp_server_t(
        io_service & io_svc)
        : fd(io_svc)
    {
    }

    void start(
        NetName const & addr)
    {
        error_code ec;
        fd.open(udp::v4(), ec);
        if (!ec)
            fd.bind(udp::endpoint(address::from_string(addr.host()), addr.port()), ec);
        if (ec) {
            handle_error(ec);
        } else {
			LOG_DEBUG("async_udp_server: bind: ok");
            fd.async_receive_from(buf.prepare(1024), endp, 
                boost::bind(&test_asio_async_udp_server_t::handle_receive_from, this, _1, _2));
        }
    }

private:
    void handle_receive_from(
        error_code const & ec, 
        size_t len)
    {
        if (ec) {
            handle_error(ec);
        } else {
			LOG_DEBUG("async_udp_server: receive: len: " << len);
            buf.commit(len);
            fd.async_send_to(buf.data(), endp, 
                boost::bind(&test_asio_async_udp_server_t::handle_send_to, this, _1, _2));
        }
    }

    void handle_send_to(
        error_code const & ec, 
        size_t len)
    {
        if (ec) {
            handle_error(ec);
        } else {
			LOG_DEBUG("async_udp_server: send: len: " << len);
            buf.consume(buf.size());
            delete this;
        }
    }

    void handle_error(
        error_code const & ec)
    {
        print_error("async_udp_server", ec);
        delete this;
    }

private:
    udp::socket fd;
    streambuf buf;
    udp::endpoint endp;
};

static void test_asio_async_udp_server(
                                       io_service & io_svc, 
                                       NetName const & addr)
{
    (new test_asio_async_udp_server_t(io_svc))->start(addr);
}

void test_asio_udp(Config & conf)
{
    int mode = 2; // 1 - sync server, 2 - sync client
    // 4 - async server, 8 - async client
    NetName addr("192.168.30.91", 69);

    conf.register_module("TestAsioUdp") << CONFIG_PARAM_RDWR(mode) << CONFIG_PARAM_RDWR(addr);

    if (mode & 3) {
        LOG_SECTION();
        LOG_DEBUG(("[udp] start"));
        boost::thread * threads[2] = {NULL};
        if (mode & 1) {
            threads[0] = new boost::thread(boost::bind(test_asio_udp_server, addr));
        }
        if (mode & 2) {
            threads[1] = new boost::thread(boost::bind(test_asio_tftp_client, addr));
        }
        if (mode & 1) {
            threads[0]->join();
            delete threads[0];
        }
        if (mode & 2) {
            threads[1]->join();
            delete threads[1];
        }
        LOG_DEBUG(("[udp] end"));
    }

    if (mode & 12) {
        LOG_SECTION();
        LOG_DEBUG(("[async_udp] start"));
        io_service io_svc;
        if (mode & 4) {
            test_asio_async_udp_server(io_svc, addr);
        }
        if (mode & 8) {
            test_asio_async_udp_client(io_svc, addr);
        }
        io_svc.run();
        LOG_DEBUG(("[async_udp] end"));
    }
}

static TestRegister tr("asio_udp", test_asio_udp);
