// TcpTransferClient.h

#ifndef _TOOLS_STATUS_TCP_TRANSFER_CLIENT_H_
#define _TOOLS_STATUS_TCP_TRANSFER_CLIENT_H_

#include <framework/Framework.h>
#include <framework/network/NetName.h>
using namespace framework::configure;
using namespace framework::network;

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/deadline_timer.hpp>
using namespace boost::system;
using namespace boost::asio;
using namespace boost::asio::ip;


namespace tools
{
    namespace status
    {

        class TcpTransferClient
        {
        public:
            TcpTransferClient(
                 boost::asio::io_service & io_svc)
            {
                fd_ = new tcp::socket(io_svc);
            }

            bool start(
                framework::network::NetName const & addr)
            {
                boost::uint32_t count = 0;
                boost::xtime xt;
                while(count++ < 3)
                try
                {
                    fd_->open(tcp::v4());
                    fd_->connect(tcp::endpoint(address::from_string(addr.host()), addr.port()));
                    return true;
                }
                catch(...)
                {
                    fd_->close();
                    boost::xtime_get(&xt, boost::TIME_UTC); 
                    xt.sec += 2;
                    boost::thread::sleep(xt);
                }
                return false;
            }

            void close()
            {
                boost::system::error_code ec;
                if (fd_)
                    fd_->close(ec);
            }

            template <typename Buffer>
            boost::uint32_t send_buf(
                Buffer const &buf)
            {
                return boost::asio::write(*fd_, buf);
            }

            template <typename Buffer>
            boost::uint32_t receive_buf(
                Buffer const & buf)
            {
                return boost::asio::read(*fd_, buf);
            }

            boost::uint32_t send_buf(
                std::string & buf)
            {
                boost::uint32_t len;
                boost::uint32_t send_len = 0;
                boost::uint32_t total_len = 0;
                len = buf.size();
                if (fd_) {
                    fd_->send(buffer(&len, sizeof(boost::uint32_t)));
                    while(total_len < len) {
                        send_len = fd_->send(buffer(&buf[total_len], (len-total_len)));
                        std::cout << "send buf len:" << send_len << std::endl;
                        total_len += send_len;
                    }
                }
                return total_len;
            }

            boost::uint32_t receive_buf(
                std::string & buf)
            {
                boost::uint32_t len;
                boost::uint32_t receive_len =  0;
                boost::uint32_t total_len =  0;
                std::string temp;
                temp.clear();
                temp.resize(1024);
                
                if (fd_) {
                    fd_->receive(buffer(&len, sizeof(boost::uint32_t)));
                    while(total_len < len) {
                         receive_len = (boost::uint32_t)fd_->receive(buffer(&temp[0], temp.size()));
                         std::cout << "recv buf len:" << receive_len << std::endl;
                         if (receive_len < 0) {
                            std::cout<< "receive_buf client read a litter data!" << std::endl;
                            break;
                         }
                         if (total_len + receive_len > buf.size()) {
                             buf.resize(total_len + receive_len);
                             std::cout<< "receive_buf buf is not enough!" << std::endl;
                         }
                         total_len += receive_len;
                         buf += temp;
                         temp.clear();
                         temp.resize(1024);
                    }
                    return total_len;
                }
            }

            ~TcpTransferClient()
            {
                close();

                if (fd_) {
                    delete fd_;
                }
            }

        private:
            tcp::socket *fd_;
        };
    }
}

#endif // _TOOLS_STATUS_TCP_TRANSFER_CLIENT_H_
