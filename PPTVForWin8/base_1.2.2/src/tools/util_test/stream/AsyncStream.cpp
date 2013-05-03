// AsyncStream.cpp

#include "tools/util_test/Common.h"
#include "util/stream/AsyncIStream.h"
#include "util/stream/AsyncOStream.h"
#include "util/stream/TcpSocket.h"
using namespace util::stream;

using namespace framework::configure;

#include <boost/asio.hpp>
#include <boost/iostreams/filter/gzip.hpp>
using namespace boost::asio;

class xor_output_filter {
public:
    typedef char char_type;
    typedef boost::iostreams::output_filter_tag category;

    xor_output_filter(char_type k)
        : k_(k) {}

    template<typename Sink>
    bool put(Sink & dst, int c) {
        return boost::iostreams::put(dst, c ^ k_);
    }
private:
    char_type k_;

};

class xor_input_filter {
public:
    typedef char char_type;
    typedef boost::iostreams::input_filter_tag category;

    xor_input_filter(char_type k)
        : k_(k) {}

    template<typename Source>
    int get(Source & src) {
        int c = boost::iostreams::get(src);
        if (c != EOF && c != boost::iostreams::WOULD_BLOCK) {
            c ^= k_;
        }
        return c;
    }
private:
    char_type k_;
};

class add_output_filter
{
public:
    typedef char char_type;
    typedef boost::iostreams::output_filter_tag category;

    add_output_filter(int num)
        : num_(num) {}

    template<typename Sink>
    bool put(Sink & dst, int c) {
        return boost::iostreams::put(dst, c + num_);
    }

private:
    int num_;
};

class sub_input_filter {
public:
    typedef char char_type;
    typedef boost::iostreams::input_filter_tag category;

    sub_input_filter(int num)
        : num_(num) {}

    template<typename Source>
    int get(Source & src) {
        int c = boost::iostreams::get(src);
        if (c != EOF && c != boost::iostreams::WOULD_BLOCK) {
            c -= num_;
        }
        return c;
    }
private:
    int num_;
};

boost::asio::streambuf buf1, buf2;

class TestClient
{
public:
    TestClient(boost::asio::io_service & ios,
        boost::asio::ip::tcp::resolver::iterator edp_iter)
        : m_ios_(ios)
        , m_tcpsocket_(ios)
        , m_socket_(m_tcpsocket_)
        , m_afo_(ios)
        , m_afi_(ios)
    {
            m_afo_.push(xor_output_filter('a'));
            m_afo_.push(add_output_filter(1));
            m_afo_.push(boost::iostreams::gzip_compressor());
            m_afo_.push((Sink &)m_socket_);

            m_afi_.push(xor_input_filter('a'));
            m_afi_.push(sub_input_filter(1));
            m_afi_.push(boost::iostreams::gzip_decompressor());
            m_afi_.push((Source &)m_socket_);

            m_tcpsocket_.async_connect(*edp_iter,
                boost::bind(&TestClient::handle_connect, this, boost::asio::placeholders::error));
    }

    void write(char * msg) {
        m_ios_.post(boost::bind(&TestClient::do_write, this, msg));
    }

    void close() {
        m_ios_.post(boost::bind(&TestClient::do_close, this));
    }

private:
    void handle_connect(const boost::system::error_code & error) {
        if (!error) {
            m_afi_.async_read_some(m_recv_buf_.prepare(1024),
                boost::bind(&TestClient::handle_read_msg, this, _1, _2));
        }
    }

    void handle_read_msg(const boost::system::error_code & ec,
        size_t bytes_transferred) {
            if (!ec) {
                if (bytes_transferred) {
                    std::cout.write((const char *)boost::asio::detail::buffer_cast_helper(m_recv_buf_.data()), bytes_transferred);
                    std::cout << std::endl;
                }

                m_recv_buf_.consume(1024);

                m_afi_.async_read_some(m_recv_buf_.prepare(1024),
                    boost::bind(&TestClient::handle_read_msg, this, _1, _2));
            } else  {
                do_close();
            }
    }

    void do_write(char * msg) {
        bool write_in_progress = !m_write_msgs_.empty();
        m_write_msgs_.push_back(msg);
        if (!write_in_progress) {
            m_afo_.async_write_some(
                boost::asio::buffer(m_write_msgs_.front(), m_write_msgs_.front().size()),
                boost::bind(&TestClient::handle_write, this, _1, _2));
        }
    }

    void handle_write(const boost::system::error_code & ec,
        size_t bytes_transferred) {
            if (!ec) {
                m_write_msgs_.pop_front();
                if (!m_write_msgs_.empty()) {
                    m_afo_.async_write_some(
                        boost::asio::buffer(m_write_msgs_.front(), m_write_msgs_.front().size()),
                        boost::bind(&TestClient::handle_write, this, _1, _2));
                }
            } else {
                do_close();
            }
    }

    void do_close() {
        m_tcpsocket_.close();
    }

private:
    async_filtering_istream         m_afi_;
    async_filtering_ostream         m_afo_;
    boost::asio::io_service &       m_ios_;
    TcpSocket                       m_socket_;
    boost::asio::ip::tcp::socket    m_tcpsocket_;
    char                            m_read_msg_[1024];
    boost::asio::streambuf          m_recv_buf_;
    std::deque<std::string>         m_write_msgs_;
};

void test_async_stream(Config & conf)
{
    try {
        boost::asio::io_service io_svr;
        boost::asio::ip::tcp::resolver resolver(io_svr);
        boost::asio::ip::tcp::resolver::query query("127.0.0.1", "4567");
        boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);

        TestClient tc(io_svr, iter);

        boost::thread t(boost::bind(&boost::asio::io_service::run, &io_svr));
        char msg[1024];
        while (std::cin.getline(msg, sizeof(msg))) {
            tc.write(msg);
        }

        t.join();
        tc.close();

    } catch (std::exception & e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

static TestRegister test("async_stream", test_async_stream);
