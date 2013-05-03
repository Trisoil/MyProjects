// UdpServer.cpp

#include "mutex/server/UdpServer.h"
#include "mutex/server/MutexSession.h"
using namespace mutex;

#include <util/archive/ArchiveBuffer.h>
#include <util/protocol/pptv/CheckSum.h>
using namespace util::archive;
using namespace util::protocol;

#include <boost/lexical_cast.hpp>

namespace mutexserver
{
    FRAMEWORK_LOGGER_DECLARE_MODULE("UdpServer");

    bool UdpServer::Listen(boost::uint16_t port)
    {
        if (is_open_) {
            //LOG(__ERROR, LOG_TYPE_NETWORK, "this UdpServer is open, so can not listen again");
            return false;
        }
        boost::system::error_code error;
        socket_.open(boost::asio::ip::udp::v4(), error);
        if (error) {
            socket_.close(error);
            return false;
        }
        socket_.bind(
            boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port),        // endpoint
            error                                        // errorhandle
        );

        if (!error) {
            // 绑定成功
            is_open_ = true;
            port_ = port;
            return true;
        } else {
            // 出现错误
            socket_.close(error);
            return false;
        }
    }

    void UdpServer::StartRecv()
    {
        std::cout<<"start receive\n";

        socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_.data_.get(), UDPRecvBuffer::UDP_RECV_BUFFER_SIZE),
            *recv_buffer_.remote_endpoint_,
            boost::bind(
                &UdpServer::HandleUdpRecvFrom, 
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred,
                recv_buffer_
            )
        );
    }

    void UdpServer::HandleUdpRecvFrom(
        const boost::system::error_code& error, 
        std::size_t bytes_transferred,
        UDPRecvBuffer& recv_buf)
    {
        if( !error )
        {
            // UDP 收包正确，向上层次发送消息
            protocol::Head head;
            ArchiveBuffer<char> buf(recv_buf.data_.get(), bytes_transferred, bytes_transferred);
            boost::uint16_t cs = pptv::check_sum((boost::uint16_t *)(recv_buf.data_.get()+2), (bytes_transferred-2));

            protocol::IUdpArchive ia(buf);
            ia>>head;
#ifdef _CHECKSUM_
            if (cs != head.checksum) {
                LOG_F(framework::logger::Logger::kLevelDebug, ("checksum failed head.checksum=%d, checksum=%d", (int)head.checksum, (int)cs));
                // 继续收包
                socket_.async_receive_from(
                    boost::asio::buffer(recv_buffer_.data_.get(), UDPRecvBuffer::UDP_RECV_BUFFER_SIZE),
                    *recv_buffer_.remote_endpoint_,
                    boost::bind(&UdpServer::HandleUdpRecvFrom, 
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred,
                        recv_buffer_));
                return;
            }
#endif
#ifdef _STDLOG_
            std::cout<<"\nhead =================== UDP ==============================\n";
            printf("action:0x%x\n",head.action);
            printf("checksum:0x%x\n",head.checksum);
            printf("sequece:0x%x\n",head.sequece);
            printf("version:0x%x\n",head.version);
#endif

            switch(head.action) {
                case protocol::KeepaliveRequestPackage::action:
                    {
                        protocol::KeepaliveRequestPackage keepAliveRequest;
                        ia>>keepAliveRequest;
                        protocol::Head responseHead;
                        
                        protocol::KeepaliveResponePackage response;
                        responseHead.action=response.action;
                        responseHead.checksum=0;
                        responseHead.reverse=0;
                        responseHead.sequece=head.sequece;
                        responseHead.version=0;

                        MutexSession::Inst()->KeepAlive(keepAliveRequest,response,recv_buf.remote_endpoint_,UDP);
                        responseHead.action=response.action;
                        boost::shared_array<char> send_buf(new char[1024]);
                        
                        ArchiveBuffer<char> aoBuf(send_buf.get(), 1024);
                        protocol::OUdpArchive oa(aoBuf);
                        oa<<responseHead;
                        oa<<response;
            
                        boost::uint16_t cs_out =  pptv::check_sum((boost::uint16_t*)(send_buf.get()+2),(aoBuf.size()-2));
                        boost::uint16_t * pcs = (boost::uint16_t*)send_buf.get();
                        *pcs = cs_out;
                        responseHead.checksum=cs_out;
    #ifdef _STDLOG_
                        std::cout<<" ======== response head =============\n";
                        printf("action:0x%x\n",responseHead.action);
                        printf("checksum:0x%x\n",responseHead.checksum);
                        printf("sequece:0x%x\n",responseHead.sequece);
                        printf("version:0x%x\n",responseHead.version);

                        std::cout<<" ======== response body =============\n";
                        printf("interval:%d\n",response.interval);
                        printf("pass:%d\n",response.passstate);
    #endif
                        UdpSendTo(aoBuf.data(1024),*recv_buf.remote_endpoint_);
                        break;
                    }
                case protocol::LeavePackage::action:
                    {
                        protocol::LeavePackage leaveRequest;
                        ia>>leaveRequest;
                        MutexSession::Inst()->Leave(leaveRequest);
                        break;
                    }
                default:
                    {
                        LOG_F(framework::logger::Logger::kLevelDebug, ("未知协议类型 "));
                    }
            }
        }

        if (!is_open_)
            return;

        // 继续收包
        socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_.data_.get(), UDPRecvBuffer::UDP_RECV_BUFFER_SIZE),
            *recv_buffer_.remote_endpoint_,
            boost::bind(&UdpServer::HandleUdpRecvFrom, 
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred,
                recv_buffer_));
    }

    void UdpServer::HandleUdpSendTo(
        const boost::system::error_code & error, 
        std::size_t bytes_transferred)
    {
        framework::logger::LoggerSection ls;
        if (error) {
            LOG_F(framework::logger::Logger::kLevelAlarm, ("send error, msg:%s ", error.message().c_str()));
        } else {
            LOG_F(framework::logger::Logger::kLevelDebug, ("send ok"));
        }
    }

    void UdpServer::UdpSendTo(
        boost::asio::const_buffers_1 buffer, 
        const boost::asio::ip::udp::endpoint & endpoint)
    {
        socket_.async_send_to(
            buffer,
            endpoint,
            boost::bind(
                &UdpServer::HandleUdpSendTo,
                this, 
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }

    void UdpServer::Close()
    {
        socket_.close();
        is_open_ = false;
    }
}