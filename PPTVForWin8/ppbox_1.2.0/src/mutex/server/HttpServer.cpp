// HttpServer.cpp

#include "mutex/server/HttpServer.h"
#include "mutex/server/HttpSessionManager.h"
using namespace mutex;

#include <util/protocol/pptv/CheckSum.h>
using namespace util::protocol;

#include <boost/bind.hpp>

namespace mutexserver
{

    FRAMEWORK_LOGGER_DECLARE_MODULE("HttpServer");

    bool HttpServer::on_receive_request_head(
        HttpRequestHead & request_head)
    {
        // Server模式：返回false
        // Proxy模式：返回true，可以修改request_head

        request_head.get_content(std::cout);
        if(request_head.method==HttpRequestHead::get){
            if(request_head.path.find("/c")==0){
                responseData4Get_ =  MutexSession::Inst()->QueryCount();
            }else if(request_head.path.find("/n")==0){
                responseData4Get_ = MutexSession::Inst()->QueryNew();
            }else if(request_head.path.find("/o")==0){
                responseData4Get_ = MutexSession::Inst()->QueryOld();
            }else if(request_head.path.find("/id")==0){
                std::string clientID = request_head.path.substr(4);
                responseData4Get_ = MutexSession::Inst()->QueryByID(clientID);
            }
            contentLength_=responseData4Get_.size();
        }
        return false;
    }

    void HttpServer::on_receive_response_head( 
        HttpResponseHead & response_head)
    {
        // Server模式：设置response_head
        // Proxy模式：可以修改response_head

        response_head.version=0x101;
        response_head.err_msg="OK";
        response_head.set_field("Content-Length",boost::lexical_cast<std::string>(contentLength_));
        if (responseData4Get_.size() > 0 && !plainSeted_) {
            plainSeted_ = true;
            response_head.set_field("Content-Type","{text/plain}");
        }
        response_head.connection = util::protocol::http_filed::Connection::keep_alive;

        response_head.get_content(std::cout);
    }

    void HttpServer::on_receive_request_data(
        boost::asio::streambuf & request_data)
    {
        boost::uint16_t const *buffer_checksum = boost::asio::buffer_cast<boost::uint16_t const *>(request_data.data());
        boost::uint16_t checkvalue = pptv::check_sum(
            buffer_checksum + 1, request_data.size() - 2);

        std::cout << "computer check sum: " << checkvalue << std::endl;
        std::cout << "buffer_checksum: " << *buffer_checksum << std::endl;

        protocol::Head head;

        protocol::IUdpArchive ia(request_data);
        ia>>head;

    #ifdef _CHECKSUM_
        if (checkvalue != head.checksum) {
            LOG_F(framework::logger::Logger::kLevelDebug, ("checksum failed head.checksum=%d, checksum=%d ",(int)head.checksum,(int)checkvalue));
            return;
        }
    #endif
    #ifdef _STDLOG_
        std::cout<<"\nhead ====================== HTTP ===========================\n";
        printf("action:0x%x\n",head.action);
        printf("checksum:%d\n",head.checksum);
        printf("sequece:0x%x\n",head.sequece);
        printf("version:0x%x\n",head.version);
    #endif

        switch(head.action) {
            case protocol::KeepaliveRequestPackage::action:
                {
                    isKeepAlive_=true;
                    protocol::KeepaliveRequestPackage keepAliveRequest;
                    ia>>keepAliveRequest;

                    boost::shared_ptr<boost::asio::ip::udp::endpoint> endpoint(new boost::asio::ip::udp::endpoint);
                    bool newClient = MutexSession::Inst()->KeepAlive(keepAliveRequest,response_,endpoint,HTTP);
                    responseHead_.action=response_.action;
                    responseHead_.checksum=0;
                    responseHead_.reverse=0;
                    responseHead_.sequece=head.sequece;
                    responseHead_.version=0;
                    if (response_.passstate == MutexSession::KEEPALIVE_PASS) {
                        sessionID_= byteArray2Str(keepAliveRequest.session_id, 16);
                        if (!newClient){
                            blocked_ = true;
                        }
                    }

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
                    //std::cout<<boost::lexical_cast<std:string>(head.action)<<"\n";
                }
        }
    }

    void HttpServer::on_receive_response_data(
        boost::asio::streambuf & response_data)
    {
        // 写入应答内容
        std::ostream os(&response_data);
        if (responseData4Get_.size() > 0) {
            os.write(responseData4Get_.c_str(), responseData4Get_.size());
        } else {
            os.write(send_buf_.get(), contentLength_);
        }
        //int i = 0;
        //std::string str = "str";
        //os << i << str;
    }

    void HttpServer::on_error(
        boost::system::error_code const & ec)
    {
        HttpSessionManager::Inst()->clearSession(sessionID_);
        std::cout << "http error"<< ec.message() << std::endl;
    }

    void HttpServer::local_process(
        local_process_response_type const & resp)
    {
        resp_ = resp;
        if (isKeepAlive_) {
            if(blocked_) {
                //阻塞处理
                HttpSessionManager::Inst()->addSession(sessionID_, this);
            } else {
                return_now(true);
            }
        } else {
            return_now(true);
        }
    }

    void HttpServer::return_now(
        bool pass)
    {
        if (isKeepAlive_) {
            if (!pass) {
                response_.passstate = MutexSession::KEEPALIVE_FAILED;
            }
            ArchiveBuffer<char> aoBuf(send_buf_.get(), 1024);
            protocol::OUdpArchive oa(aoBuf);
            oa << responseHead_;
            oa << response_;
            boost::uint16_t cs_out =  pptv::check_sum((boost::uint16_t*)(send_buf_.get()+2), (aoBuf.size()-2));
            boost::uint16_t * pcs = (boost::uint16_t*)send_buf_.get();
            *pcs = cs_out;
            responseHead_.checksum = cs_out;

            contentLength_ = aoBuf.size();
        }
    #ifdef _STDLOG_
        std::cout<<" ======== response head =============\n";
        printf("action:0x%x\n",responseHead_.action);
        printf("checksum:%d\n",responseHead_.checksum);
        printf("sequece:0x%x\n",responseHead_.sequece);
        printf("version:0x%x\n",responseHead_.version);

        std::cout<<" ======== response body =============\n";
        printf("interval:%d\n",response_.interval);
        printf("pass:%d\n",response_.passstate);
    #endif
        resp_(boost::system::error_code());
    }

    /*
    virtual void transfer_request_data(
        transfer_response_type const & resp)
    {
        if (is_local()) {
            HttpProxy::transfer_request_data(resp); // 将会调用on_receive_request_data
        } else {
            HttpProxy::transfer_request_data(resp);
        }
    }

    virtual void transfer_response_data(
        transfer_response_type const & resp)
    {
        if (is_local()) {
            HttpProxy::transfer_response_data(resp); // 将会调用on_receive_response_data
        } else {
            HttpProxy::transfer_response_data(resp); // 将会调用on_receive_response_data
            //resp_ = resp;
            //size_t len = 0;
            //boost::asio::async_read(get_server_data_stream(), response_buffer_.prepare(len), 
            //    bind(boost::mem_fn(&MyHttpProxy::handle_receive_response_data), this, _1, _2));
        }
    }
    */
}
