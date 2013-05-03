#include "stdafx.h"
#include "NotifyServerModule.h"
#include "Logger.h"
#include "Base64.h"
#include "network/UrlCodec.h"
#include "MainThread.h"
#include <framework/configure/Config.h>

static char s_base64[] = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static unsigned char s_base64_decode_table[256] = { 0 };

PeerInfo::PeerInfo()
{
    last_recv_time_ = time(NULL);
    recv_count_ = online_count_ = 0;
}


PeerInfo::~PeerInfo()
{

}

CPushTask::CPushTask()
{
    task_type_ = 0;
    task_id_ = 0;
    start_time_ = 0;//起始时间
    state_ = 0;//0是没有开始，1是正在进行，2是已经结束
    complete_count_ = 0;
}

CPushTask::~CPushTask()
{


}


namespace NotifyServer
{
    const std::string NotifySvrModule::ConfigFileName = "NotifyServer.config.txt";

    //////////////////////////////////////////////////////////////////////////
    NotifyHttpSvrConn::NotifyHttpSvrConn(boost::shared_ptr<network::HttpServer> http_server_socket) 
        : http_server_socket_(http_server_socket), is_running_(false), will_stop_(false)
    {
    }

    NotifyHttpSvrConn::~NotifyHttpSvrConn()
    {

    }

    void NotifyHttpSvrConn::Start()
    {
        if( is_running_ == true ) return;

        is_running_ = true;
        
        if (http_server_socket_) {
            http_server_socket_->SetListener(shared_from_this());
            http_server_socket_->SetRecvTimeout(5*1000);
            http_server_socket_->HttpRecv();
        }
    }

    void NotifyHttpSvrConn::Stop()
    {
        if( is_running_ == false ) return;
        if (http_server_socket_)
        {
            // set handler to null, do not need notice any more
            http_server_socket_->SetListener(IHttpServerListener::pointer());
            http_server_socket_->WillClose(); 
            http_server_socket_.reset(); 
        }
        is_running_ = false;
    }

    void NotifyHttpSvrConn::WillStop()
    {
        if( is_running_ == false ) return;
        will_stop_ = true;
        MainThread::Post(
            boost::bind( &NotifySvrModule::RemoveNotifyConnection, NotifySvrModule::Inst(), shared_from_this() )
            );
    }


    void NotifyHttpSvrConn::OnHttpRecvSucced(boost::shared_ptr<network::HttpRequest> http_request)
    {
        boost::asio::ip::tcp::endpoint tcp_point = http_server_socket_->GetEndPoint();
        boost::uint32_t ip = tcp_point.address().to_v4().to_ulong();
        boost::uint16_t port = tcp_point.port();
        bool ret = false;
        multimap<boost::uint32_t, boost::uint16_t>::iterator ip_it = NotifySvrModule::Inst()->receive_web_map_.find(ip);
        for (; ip_it != NotifySvrModule::Inst()->receive_web_map_.end(); ip_it++)
        {
            if(ip_it->first == ip)
            {
                if(ip_it->second == port || ip_it->second == 0)
                {
                    ret = true;
                    break;
                }
            }
            else
                break;
        }
        if(ret == false)
        {
            http_server_socket_->HttpSend403Header();
            return;
        }


        if( is_running_ == false ) {
            return;
        }
        LOG4CPLUS_DEBUG(Loggers::NotifyHttpSvrConn(), "[NotifyHttpSvrConn] " << "HttpRequest From Function, Data: " << *http_request);

        // save
        http_request_demo_ = http_request;

        //LOG(__DEBUG, "NotifyHttpSvrConn", __FUNCTION__<<":"<<__LINE__<<" save_mode = true, source_url = " << source_url_);


        //过滤掉含有非法字符的请求
        if (
            http_request->GetUrl().length() == 0 ||
            http_request->GetUrl().find("'") != string::npos||
            http_request->GetUrl().find("\"") != string::npos
            )
        {
            LOG4CPLUS_ERROR(Loggers::NotifyHttpSvrConn(), "OnHttpRecvSucced Url ERROR !! Contain '  \"");
            WillStop();
            return;
        }
        string request_path = http_request->GetPath();
        string request_url = http_request->GetUrl();
        string response;
        string command;
        char buffer[1024] = { 0 };
        if(request_path.empty())
        {
            http_server_socket_->HttpSend403Header();
            return;
        }

        //解析？号
        request_path = request_path.substr(1);
        int pos_ask = request_path.find("?");            
        if(pos_ask != string::npos)
        {
            command = request_path.substr(0, pos_ask);
            request_path = request_path.substr(pos_ask + 1);
        }
        else
            command = request_path;

        if(http_request->GetMethod() == "GET")
        {
            if(command == "online")
            {
                boost::uint32_t online_num = NotifySvrModule::Inst()->GetPeerReportCount();
                sprintf(buffer, "online=\"%u\"", online_num);
                response = buffer;
                //http_server_socket_->HttpSendContent( cross_domain_xml, "text/x-cross-domain-policy" );
            }
            else if(command == "release")
            {
                string tid = request_path;
                int tid_pos = tid.find("=");
                if(tid_pos != string::npos)
                {
                    string the_tid = tid.substr(tid_pos + 1);
                    boost::uint32_t the_uint_tid = strtoul(the_tid.c_str(), NULL, 10);
                    bool is_ok = NotifySvrModule::Inst()->ReleaseTask(the_uint_tid);
                    if(is_ok)
                    {
                        sprintf(buffer, "success=\"release %u sucess\"", the_uint_tid);
                    }
                    else
                    {
                        sprintf(buffer, "err=\"release %u err, no this task id\"", the_uint_tid);
                    }
                    response = buffer;
                }
            }
            else if(command == "stat")
            {
                string tid = request_path;//.substr(pos_ask + 1);
                int tid_pos = tid.find("=");
                if(tid_pos != string::npos)
                {
                    string the_tid = tid.substr(tid_pos + 1);
                    boost::uint32_t the_uint_tid = strtoul(the_tid.c_str(), NULL, 10);
                    map<boost::uint32_t, CPushTaskHandle>::iterator the_task_it = NotifySvrModule::Inst()->task_mgr_.find(the_uint_tid);
                    if(the_task_it != NotifySvrModule::Inst()->task_mgr_.end())
                    {
                        boost::uint32_t complete_num = NotifySvrModule::Inst()->GetPeerReportCompleteCount(the_task_it->first);
                        sprintf(buffer, "RestTime=\"%u\" CompleteNum=\"%u\"", the_task_it->second->rest_time_, complete_num);
                        response = buffer;
                    }
                    else
                    {
                        map<boost::uint32_t, CPushTaskHandle>::iterator the_cp_task_it = NotifySvrModule::Inst()->complete_task_mgr_.find(the_uint_tid);
                        if(the_cp_task_it != NotifySvrModule::Inst()->complete_task_mgr_.end())
                        {
                            boost::uint32_t complete_num = NotifySvrModule::Inst()->GetPeerReportCompleteCount(the_cp_task_it->first);
                            sprintf(buffer, "RestTime=\"%u\" CompleteNum=\"%u\"", 0, the_cp_task_it->second->complete_count_);
                            response = buffer;
                        }
                        else
                        {
                            sprintf(buffer, "err=\"%s\"", "no task");
                            response = buffer;
                        }
                    }
                }
            }
            else if(command == "task")
            {
                string tid, type, duration, content, rsttime;
                string now_data = request_path;
                int loop = 0;
                while(true)
                {
                    int pos_begin = now_data.find("=");
                    int pos_end = now_data.find("&");
                    if(pos_begin == string::npos)
                    {
                        break;
                    }
                    string value;
                    if(pos_end == string::npos)
                    {
                        value = now_data.substr(pos_begin + 1);
                    }
                    else
                    {
                        value = now_data.substr(pos_begin + 1, pos_end - pos_begin - 1);
                    }
                    switch(loop)
                    {
                    case 0:
                        tid = value;
                        break;
                    case 1:
                        type = value;
                        break;
                    case 2:
                        duration = value;
                        break;
                    case 3:
                        rsttime = value;
                        break;
                    case 4:
                        content = value;
                    default:
                        break;
                    }
                    loop++;
                    if(pos_end == string::npos)
                        break;
                    else
                        now_data = now_data.substr(pos_end + 1);
                }
                if(!tid.empty() && !type.empty() && !duration.empty() && !rsttime.empty() && !content.empty())
                {
                    boost::uint32_t the_tid = strtoul(tid.c_str(), NULL, 10);
                    boost::uint32_t the_type = strtoul(type.c_str(), NULL, 10);
                    boost::uint32_t the_duration = strtoul(duration.c_str(), NULL, 10);
                    boost::uint32_t the_rest_time = strtoul(rsttime.c_str(), NULL, 10);
                    string decode_str;
                    content = network::UrlCodec::Decode(content);
                    Base64::decode(content, decode_str);
                    //XBase64Init();
                    //DWORD dwDecodedLength = strlen(content.c_str());
                    vector<char> the_data;
                    the_data.resize(decode_str.length());
                    memcpy((unsigned char*)(&the_data[0]), decode_str.c_str(), decode_str.length());
                    //DWORD dwDataLength = 0;
                    //dwDataLength = XBase64Decode((unsigned char*)content.c_str(), dwDecodedLength, 
                    //    (unsigned char*)(&the_data[0]), dwDecodedLength);
#ifdef _DEBUG
                    //FILE* fp = fopen("D:\\1.dat", "w+b");
                    //fwrite((unsigned char*)(&the_data[0]), the_data.size(), 1, fp);
                    //fclose(fp);
#endif // _DEBUG
                    if(decode_str.length() > 900)
                    {
                        sprintf(buffer, "err=\"%s\"", "Task Send ERR, content is too long, big than 900");
                        response = buffer;
                    }
                    else
                    {
                        NotifySvrModule::Inst()->TestATask(the_tid, the_data, the_rest_time, the_duration, the_type);
                        sprintf(buffer, "success=\"%s\"","Task Send OK");
                        response = buffer;
                    }
                }
                else
                {
                    sprintf(buffer, "err=\"%s\"", "Param is ERR");
                    response = buffer;
                }
                //if(content.size() > 
                //http://192.168.43.141:15000/task?tid=id&type=tp& timeout=timeouttime&content=data
            }
        }
        else if(http_request->GetMethod() == "POST")
        {
            //只接受task的post方法
            string request = http_request->GetRequestString();
            //request = UrlCodec::Decode(request);
            request = Utf8ToAnsi(request.c_str());
            int start_pos = request.find("\r\n\r\n");
            bool is_ok = false;
            do 
            {
                if(start_pos == string::npos)
                    break;
                string data_str = request.substr(start_pos + 4);
                if(command == "task")
                {
                    string tid, type, duration, content, rsttime;
                    string now_data = data_str;
                    int loop = 0;
                    while(true)
                    {
                        int pos_begin = now_data.find("=");
                        int pos_end = now_data.find("&");
                        if(pos_begin == string::npos)
                        {
                            break;
                        }
                        string value;
                        if(pos_end == string::npos)
                        {
                            value = now_data.substr(pos_begin + 1);
                        }
                        else
                        {
                            value = now_data.substr(pos_begin + 1, pos_end - pos_begin - 1);
                        }
                        switch(loop)
                        {
                        case 0:
                            tid = value;
                            break;
                        case 1:
                            type = value;
                            break;
                        case 2:
                            duration = value;
                            break;
                        case 3:
                            rsttime = value;
                            break;
                        case 4:
                            content = value;
                        default:
                            break;
                        }
                        loop++;
                        if(pos_end == string::npos)
                            break;
                        else
                            now_data = now_data.substr(pos_end + 1);
                    }
                    if(!tid.empty() && !type.empty() && !duration.empty() && !rsttime.empty() && !content.empty())
                    {
                        boost::uint32_t the_tid = strtoul(tid.c_str(), NULL, 10);
                        boost::uint32_t the_type = strtoul(type.c_str(), NULL, 10);
                        boost::uint32_t the_duration = strtoul(duration.c_str(), NULL, 10);
                        boost::uint32_t the_rest_time = strtoul(rsttime.c_str(), NULL, 10);
                        boost::uint32_t dwDataLength = content.length();
                        vector<char> the_data;
                        the_data.resize(dwDataLength);
                        memcpy((unsigned char*)(&the_data[0]), content.c_str(), content.length());
                        if(dwDataLength > 900)
                        {
                            sprintf(buffer, "err=\"%s\"", "Task Send ERR, content is too long, big than 900");
                            response = buffer;
                        }
                        else
                        {
                            NotifySvrModule::Inst()->TestATask(the_tid, the_data, the_rest_time, the_duration, the_type);
                            sprintf(buffer, "success=\"%s\"","Task Send OK");
                            response = buffer;
                        }
                    }
                    else
                    {
                        sprintf(buffer, "err=\"%s\"", "Param is ERR");
                        response = buffer;
                    }
                }

            } while (false);
        }
        http_server_socket_->SetRecvTimeout(0);
        if(response.empty())
        {
            sprintf(buffer, "err=\"%s\"", "err command");
            response = buffer;
        }
        string response_xml = 
            //"<!-- NOTIFYSVR -->\n"
            "<?xml version=\"1.0\" ?>\n"
            "<response-value>\n";
        char buffer_xml[1024] = { 0 };
        sprintf(buffer_xml, "  <value %s/>\n", response.c_str());
        response_xml += buffer_xml;
        //"  <value response_value=\"*\" headers=\"*\"/>\n"
        response_xml +=    "</response-value>\n";
        //http_server_socket_->HttpSendHeader(response.size());//, response);
        http_server_socket_->HttpSendContent(response_xml, "text/xml");

    }

    void NotifyHttpSvrConn::OnHttpRecvFailed(boost::uint32_t error_code)
    {
        if( is_running_ == false ) return;
        LOG4CPLUS_DEBUG(Loggers::NotifyHttpSvrConn(), "Http receive failed, error code = " << error_code << " " << shared_from_this());
        WillStop();
    }

    void NotifyHttpSvrConn::OnHttpRecvTimeout()
    {
        if( is_running_ == false ) return;
        LOG4CPLUS_DEBUG(Loggers::NotifyHttpSvrConn(), "Http received timeout");
        WillStop();
    }

    void NotifyHttpSvrConn::OnTcpSendSucced(size_t length)
    {
        if( is_running_ == false ) return;
        LOG4CPLUS_INFO(Loggers::NotifyHttpSvrConn(), "OnTcpSendSucced " << http_server_socket_->GetEndPoint()
            << " length=" << length << " " <<shared_from_this());
    }

    void NotifyHttpSvrConn::OnTcpSendFailed()
    {
        if( is_running_ == false ) return;
        LOG4CPLUS_WARN(Loggers::NotifyHttpSvrConn(), "OnTcpSendFailed " << http_server_socket_->GetEndPoint() << " " << shared_from_this());
        WillStop();
    }

    void NotifyHttpSvrConn::OnTcpAck()
    {
        LOG4CPLUS_DEBUG(Loggers::NotifyHttpSvrConn(), "OnTcpAck()");
        if (false == is_running_)
        {
            LOG4CPLUS_WARN(Loggers::NotifyHttpSvrConn(), "On tcp ack, is running = false");
            return ;
        }
    }

    void NotifyHttpSvrConn::OnNoTcpAck()
    {
        LOG4CPLUS_DEBUG(Loggers::NotifyHttpSvrConn(), "OnNoTcpAck()");
        if (false == is_running_)
        {
            LOG4CPLUS_WARN(Loggers::NotifyHttpSvrConn(), "On no tcp ack, is running = false");
            return ;
        }
    }

    void NotifyHttpSvrConn::OnTcpSendTimeout()
    {
        if (false == is_running_) {
            return ;
        }
        LOG4CPLUS_WARN(Loggers::NotifyHttpSvrConn(), "OnTcpSendTimeout");
    }

    void NotifyHttpSvrConn::OnClose()
    {
        if( is_running_ == false ) return;
        WillStop();
    }

//////////////////////////////////////////////////////////////////////////

    boost::shared_ptr<NotifySvrModule> NotifySvrModule::inst_(new NotifySvrModule());

    void NotifySvrModule::Start(boost::uint16_t local_udp_port, boost::uint16_t http_server_port)
    {
        server_guid_.generate();
        self_type_ = 0;
        if (is_running_ == true)return;

        LoadConfig();

        udp_server_.reset(new ::protocol::UdpServer( MainThread::IOS() , shared_from_this() ));
        http_client_.reset(new util::protocol::HttpClient(MainThread::IOS()));

        if( udp_server_->Listen(local_udp_port) == false )
        {
            std::cout << "Failed to Listen to the UDP Port " << local_udp_port << std::endl;
            std::cout << "I Have Stopped to Work!!!!!" << std::endl;
            std::cout << "Please Type \"exit\" to Quit" << std::endl;
            MainThread::IOS().post(
                boost::bind(&NotifySvrModule::Stop, NotifySvrModule::Inst())
                );
        }
        else
        {
            std::cout << "NotifySvrModule::Start() Udp Listen To port " << local_udp_port << std::endl;
        }

        if (http_server_port > 0)
        {
            acceptor_ = network::HttpAcceptor::create(MainThread::IOS(), shared_from_this());
            assert( acceptor_ );

            boost::system::error_code error;
            boost::asio::ip::address localhost = boost::asio::ip::address::from_string("127.0.0.1", error);

            LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "Try Endpoint " << localhost.to_string() << ":" << http_server_port);
            acceptor_->Close();
            // try 127.0.0.1
            boost::asio::ip::tcp::endpoint ep(localhost, http_server_port);
            bool is_ok = false;
            if( acceptor_->Listen(ep) )
            {
                LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "OK, Try to Listen 0.0.0.0:" << http_server_port);
                // ok, change to listen all
                acceptor_->Close();
                if (acceptor_->Listen(http_server_port))
                {
                    acceptor_->TcpAccept();
                    LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "listen http port:" << http_server_port << "success");
                    is_ok = true;
                }
            }
        
            if( is_ok == false)
            {
                // 彻底失败
                acceptor_->Close();
                acceptor_.reset();
                LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "listen http port" << http_server_port << "Failed");
            }
        }

        // check alive timer
        check_alive_timer_.reset(new boost::asio::deadline_timer(MainThread::IOS()));
        check_alive_timer_->expires_from_now(boost::posix_time::seconds(10));
        check_alive_timer_->async_wait(boost::bind(&NotifySvrModule::OnCheckAliveTimer, shared_from_this()));

        //1分钟检查一次当前有否任务超时
        check_task_timer_.reset(new boost::asio::deadline_timer(MainThread::IOS()));
        check_task_timer_->expires_from_now(boost::posix_time::seconds(1));
        check_task_timer_->async_wait(boost::bind(&NotifySvrModule::OnCheckTaskTimer, shared_from_this()));

        //2s一次
        udp_server_->Recv(udp_server_receive_count_);
        RegisterAllPackets();
        is_running_ = true;
    }

    void NotifySvrModule::OnHttpAccept( boost::shared_ptr<network::HttpServer> http_server_for_accept)
    {
        if( is_running_ == false ) return;
        boost::shared_ptr<NotifyHttpSvrConn> pointer = NotifyHttpSvrConn::create(http_server_for_accept);
        pointer->Start();
        notify_http_conn_set.insert(pointer);
    }

    void NotifySvrModule::OnHttpAcceptFailed()
    {
        LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "OnHttpAcceptFailed");
    }

    void NotifySvrModule::RemoveNotifyConnection(boost::shared_ptr<NotifyHttpSvrConn> server_socket)
    {
        if (false == is_running_)
            return ;

        if( notify_http_conn_set.find(server_socket) == notify_http_conn_set.end() )
        {
            return;
        }
        notify_http_conn_set.erase(server_socket);
        server_socket->Stop();
    }


    void NotifySvrModule::Stop()
    {
        if (is_running_ == false)return;
        is_running_ = false;

        // 定时器
        if (check_alive_timer_) check_alive_timer_->cancel();
        if(check_task_timer_)  check_task_timer_->cancel();
        if (udp_server_)
        {
            udp_server_->Close();
            udp_server_.reset();
        }
    }

    void NotifySvrModule::OnUdpRecv(const protocol::Packet & packet)
    {
        if (is_running_ == false)
            return;

        // NotifySvr 相关协议
        if( packet.PacketAction >= 0xA0 && packet.PacketAction < 0xA9 )
        {
            switch(packet.PacketAction)
            {
            case protocol::JoinRequestPacket::Action:
                {
                    OnJoinRequestPacket((const protocol::JoinRequestPacket &)packet);
                }
                break;
            case protocol::PeerLeavePacket::Action:
                {
                    OnPeerLeavePacket((const protocol::PeerLeavePacket &)packet);
                }
                break;
            case protocol::NotifyKeepAliveRequestPacket::Action:
                {
                    OnNotifyKeepAliveRequestPacket((const protocol::NotifyKeepAliveRequestPacket &)packet);
                }
                break;
            case protocol::NotifyResponsePacket::Action:
                {
                    OnNotifyResponsePacket((const protocol::NotifyResponsePacket &)packet);
                }
                break;
            }
        }
        else
        {
            LOG4CPLUS_DEBUG(Loggers::NotifySvrModule(), "Unknown Action：0x" << std::hex << packet.PacketAction << ", from " << packet.end_point);
        }
    }

    void NotifySvrModule::OnNotifyNewTask(CPushTaskHandle pt)
    {
        if(peer_mgr_.empty())
        {
            LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "OnNotifyNewTask, task id is:" << pt->task_id_ << " but no peer in manager now");
        }
        for (map<Guid, PeerInfoHandle>::iterator  it = peer_mgr_.begin(); 
                                                  it != peer_mgr_.end(); it++)
        {
            SendNotifyInfo(it->second->end_point_, pt);
        }
    }

    void NotifySvrModule::SendNotifyInfo(const boost::asio::ip::udp::endpoint& end_point, CPushTaskHandle pt)//发送一个任务
    {
        boost::uint32_t transaction_id = protocol::Packet::NewTransactionID();
        string task_buf;
        task_buf.append((char*)(&pt->content_[0]), pt->content_.size());
        protocol::NotifyRequestPacket notify_packet(transaction_id, pt->task_id_, pt->duration_, pt->rest_time_,
            pt->task_type_, task_buf, end_point);

        DoSendPacket(notify_packet);
        //MsgSystemHandle ms;
        //ms.reset(new MsgSystem(notify_packet, end_point));
        //packet_map_.insert(make_pair(transaction_id_, ms));
        LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "OnNotifyNewTask, task id is:" << pt->task_id_ << 
            " packet id is:" << transaction_id << " Peer is: " << end_point.address().to_string() << 
            " content size is:" << pt->content_.size() << 
            " duration is:" << pt->duration_ << 
            " rest time is:" << pt->rest_time_);
    }

    void NotifySvrModule::PrintNetInfo(bool is_timer)
    {

    }

    void NotifySvrModule::OnConnectSucced()
    {
        if( is_running_ == false ) return;

        LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "OnConnectSucced");

        is_connected_ = true;
        SendHttpRequest();
    }

    void NotifySvrModule::SendHttpRequest()
    {
        if (false == is_running_)
            return ;
        //http_client_->HttpPostString();
        //http_client_->HttpGetByString();;
    }

    void NotifySvrModule::OnConnectFailed(u_int error_code)
    {
        if( is_running_ == false ) return;
        http_client_->close();
    }

    void NotifySvrModule::OnConnectTimeout()
    {
        if( is_running_ == false ) return;
        http_client_->close();
    }

    void NotifySvrModule::OnRecvHttpHeaderSucced(boost::shared_ptr<network::HttpResponse> http_response)
    {
        if( is_running_ == false ) return;
    }

    void NotifySvrModule::OnRecvHttpHeaderFailed(u_int error_code)
    {
        if( is_running_ == false ) return;
    }

    void NotifySvrModule::OnRecvHttpDataSucced(const boost::asio::streambuf & buffer, size_t file_offset, size_t content_offset)
    {

    }

    void NotifySvrModule::OnRecvHttpDataPartial(const boost::asio::streambuf & buffer, size_t file_offset, size_t content_offset)
    {

    }

    void NotifySvrModule::OnRecvHttpDataFailed(u_int error_code)
    {

    }

    void NotifySvrModule::OnRecvTimeout()
    {

    }

    void NotifySvrModule::OnComplete()
    {

    }

    void NotifySvrModule::ConsoleLogListTaskInfo(boost::uint32_t task_id)
    {
        map<boost::uint32_t, CPushTaskHandle>::iterator it = task_mgr_.find(task_id);
        if(it != task_mgr_.end())
        {
            CPushTaskHandle handle = it->second;
            std::cout << "Task id: " << task_id << " start time:" << handle->start_time_
                << " tasktype:" << handle->task_type_ << std::endl <<
                " state_:" << (boost::uint32_t)handle->state_ << 
                " duration:" << handle->duration_ << std::endl << 
                " resttime:" << handle->rest_time_ << std::endl;
        }
    }

    void NotifySvrModule::TestATask(boost::uint32_t tid, vector<char>& content, boost::uint32_t resttime, boost::uint32_t duration, boost::uint32_t type)
    {
        map<boost::uint32_t, CPushTaskHandle>::iterator it = task_mgr_.find(tid);
        LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "TestATask, task id is:" << tid);
        if(content.size() > 900)
        {
            LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "TestATask, Task content is too big, size is:" << content.size());
            return;
        }
        if(it == task_mgr_.end())
        {
            CPushTaskHandle pt;
            pt.reset(new CPushTask());
            //pt->channel_id_ = -1;
            pt->task_type_ = type;
            pt->task_id_ = tid;
            pt->start_time_ = (boost::uint32_t)time(NULL);//起始时间
            pt->state_ = 1;//0是没有开始，1是正在进行，2是已经结束
            pt->duration_ = duration;
            pt->rest_time_ = resttime;
            pt->content_.resize(content.size());
            memcpy((void*)(&pt->content_[0]), (void*)(&content[0]), content.size());
            OnNotifyNewTask(pt);
            task_mgr_.insert(make_pair(tid, pt));
        }
    }

    void NotifySvrModule::LoadConfig()
    {
        framework::configure::Config conf("NotifyServer.config.txt");

        boost::uint32_t web_count;
        conf.register_module("NotifyServer")
            << CONFIG_PARAM_NAME_RDONLY("MgrPeerCount", max_peer_num_)
            << CONFIG_PARAM_NAME_RDONLY("MaxNotifyPeer", notify_back_peer_)
            << CONFIG_PARAM_NAME_RDONLY("WebCount", web_count)
            << CONFIG_PARAM_NAME_RDONLY("udp_server_receive_count", udp_server_receive_count_)
            << CONFIG_PARAM_NAME_RDONLY("udp_keep_alive_time", keep_alive_time_);

        CheckConfig();

        receive_web_map_.clear();

        for (boost::uint32_t i = 1; i <= web_count; ++i)
        {
            std::stringstream ip_key;
            ip_key << "WebIP" << i;

            std::stringstream port_key;
            port_key << "WebPort" << i;

            std::string ip_str;
            boost::uint16_t port;
            conf.register_module("NotifyServer")
                << CONFIG_PARAM_NAME_RDONLY(ip_key.str(), ip_str)
                << CONFIG_PARAM_NAME_RDONLY(port_key.str(), port);

            boost::system::error_code ec;
            boost::asio::ip::address_v4 address(boost::asio::ip::address_v4::from_string(ip_str, ec));
            if (!ec)
            {
                receive_web_map_.insert(std::make_pair(address.to_ulong(), port));
            }
        }
    }

    void NotifySvrModule::OnCheckAliveTimer()
    {
        boost::uint32_t now = (boost::uint32_t)time(NULL);
        vector<Guid> erase_vec;
        for (map<Guid, PeerInfoHandle>::iterator it = peer_mgr_.begin(); 
            it != peer_mgr_.end();
            it++)
        {
            if(now - (boost::uint32_t)it->second->last_recv_time_ > 90)//超过90S，删除之
            {
                erase_vec.push_back(it->first);
            }
        }
        for (int i = 0; i < erase_vec.size(); i++)
        {
            peer_mgr_.erase(erase_vec[i]);
            map<Guid, PeerInfoHandle>::iterator it = peer_nat_vec_.find(erase_vec[i]);
            if(it != peer_nat_vec_.end())
            {
                peer_nat_vec_.erase(it);
            }
        }

        check_alive_timer_->expires_from_now(boost::posix_time::seconds(10));
        check_alive_timer_->async_wait(boost::bind(&NotifySvrModule::OnCheckAliveTimer, shared_from_this()));
    }

    void NotifySvrModule::OnCheckTaskTimer()
    {
        vector<boost::uint32_t> erase_vec;
        for (map<boost::uint32_t, CPushTaskHandle>::iterator it = task_mgr_.begin();
            it != task_mgr_.end();
            it++)
        {
            if(it->second->rest_time_ == 0)
            {
                erase_vec.push_back(it->first);
            }
            else
            {
                if(it->second->rest_time_ > 1)
                    it->second->rest_time_ -= 1;
                else
                {
                    it->second->rest_time_ = 0;
                    it->second->complete_count_ = GetPeerReportCompleteCount(it->first);
                    erase_vec.push_back(it->first);
                }
            }
        }
        //移动到过期任务中，把过期任务的状态设置为已经完成
        for (int i = 0; i < erase_vec.size(); i++)
        {
            CPushTaskHandle pt = task_mgr_[erase_vec[i]];
            pt->state_ = 2;
            complete_task_mgr_.insert(make_pair(erase_vec[i], pt));
            task_mgr_.erase(erase_vec[i]);
        }

        check_task_timer_->expires_from_now(boost::posix_time::seconds(1));
        check_task_timer_->async_wait(boost::bind(&NotifySvrModule::OnCheckTaskTimer, shared_from_this()));
    }

    void NotifySvrModule::CheckConfig()
    {
        if (max_peer_num_ == 0)
        {
            max_peer_num_ = 1000;
        }

        if (notify_back_peer_ == 0 || notify_back_peer_ > 20)
        {
            notify_back_peer_ = 20;
        }

        if (udp_server_receive_count_ == 0)
        {
            udp_server_receive_count_ = 1;
        }

        if (keep_alive_time_ == 0)
        {
            keep_alive_time_ = 10;
        }
    }

    void NotifySvrModule::OnJoinRequestPacket(const protocol::JoinRequestPacket & join_request)
    {
        vector<protocol::NodeInfo> node_vec;
        boost::uint16_t net_type = join_request.nat_type_;
        std::map<Guid, PeerInfoHandle>::iterator it = peer_mgr_.find(join_request.peer_guid_);
        boost::uint8_t ret = 0;//成功
        LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "UDP Received JoinRequestPacket: Peer Net Type is:" << net_type);
        if(it != peer_mgr_.end())
        {
            LOG4CPLUS_DEBUG(Loggers::NotifySvrModule(), "UDP Received JoinRequestPacket: Peer is already in mgr");
            it->second->last_recv_time_ = time(NULL);
        }
        else
        {
            if(net_type == -1)
            {
                if (join_request.end_point.address().to_v4().to_ulong() == join_request.internal_ip_)
                    net_type = 4;
                else
                    net_type = 0;
            }
            LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "UDP Received JoinRequestPacket: Peer Net Type2 is:" << net_type);

            PeerInfoHandle pi = GetPeerInfoHandle(join_request);

            if(peer_mgr_.size() < max_peer_num_)
            {
                //接受节点

                peer_mgr_.insert(make_pair(pi->node_info_.PeerGuid, pi));

                if(net_type != 4)//如果是内网节点
                {
                    LOG4CPLUS_DEBUG(Loggers::NotifySvrModule(), "UDP Received, JoinRequestPacket, recv new peer, peer is in NAT");
                    peer_nat_vec_.insert(make_pair(pi->node_info_.PeerGuid, pi));
                }
                else
                {
                    LOG4CPLUS_DEBUG(Loggers::NotifySvrModule(), "UDP Received, JoinRequestPacket, recv new peer, peer is public");
                }
            }
            else
            {
                bool need_ret_peer = true;//是否要返回节点列表
                if(net_type == 4 )//如果不是内网节点
                {
                    //检查自己管理的是否有内网节点
                    if(!peer_nat_vec_.empty())
                    {
                        LOG4CPLUS_DEBUG(Loggers::NotifySvrModule(), "UDP Received, JoinRequestPacket, let NAT peer out!!");
                        need_ret_peer = false;//把内网节点干掉,接收这个节点
                        map<Guid, PeerInfoHandle>::iterator begin_nat_it = peer_nat_vec_.begin();
                        PeerInfoHandle handle = begin_nat_it->second;
                        peer_mgr_.erase(handle->node_info_.PeerGuid);
                        //接受节点
                        peer_nat_vec_.erase(begin_nat_it);
                        boost::uint32_t transaction_id_ = protocol::Packet::NewTransactionID();
                        protocol::PeerLeavePacket leave_packet(transaction_id_, server_guid_, join_request.end_point);
                        DoSendPacket(leave_packet);
                        peer_mgr_.insert(make_pair(pi->node_info_.PeerGuid, pi));
                    }
                }

                if(need_ret_peer)
                {
                    //节点过多,从当前位置选择20个节点返回
                    ret = 1;
                    //随机选择20个节点返回，20*40 + 2 = 322
                    peer_mgr_loop_ = peer_mgr_.find(now_guid);
                    if(peer_mgr_loop_ == peer_mgr_.end())
                        peer_mgr_loop_ = peer_mgr_.begin();
                    else
                    {
                        peer_mgr_loop_++;
                        if(peer_mgr_loop_ == peer_mgr_.end())
                            peer_mgr_loop_ = peer_mgr_.begin();
                    }
                    map<Guid, PeerInfoHandle>::iterator loop_begin = peer_mgr_loop_;
                    for (; peer_mgr_loop_ != peer_mgr_.end(); peer_mgr_loop_++)
                    {
                        node_vec.push_back(peer_mgr_loop_->second->node_info_);
                        LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "UDP Received, JoinRequestPacket, replay1 other node ip is:" << 
                            peer_mgr_loop_->second->end_point_ << " guid is: " << peer_mgr_loop_->first
                            << " detect ip is:" << peer_mgr_loop_->second->node_info_.DetectIP 
                            << " detect port is:" << peer_mgr_loop_->second->node_info_.DetectPort
                            << " self ip is:" << peer_mgr_loop_->second->node_info_.InternalIP
                            << " self port is:" << peer_mgr_loop_->second->node_info_.InternalPort );
                        if(node_vec.size() >= notify_back_peer_)
                        {
                            now_guid = peer_mgr_loop_->first;
                            break;
                        }
                    }
                    if(node_vec.size() < notify_back_peer_)
                    {
                        //如果小于给定长度，检查是否当前Peer列表大于给定长度，如果大于从头开始再扫描,但是不重复
                        if(peer_mgr_.size() >= notify_back_peer_)
                        {
                            if(peer_mgr_loop_ == peer_mgr_.end())
                                peer_mgr_loop_ = peer_mgr_.begin();
                            for (; peer_mgr_loop_ != peer_mgr_.end() && peer_mgr_loop_ != loop_begin; peer_mgr_loop_++)
                            {
                                node_vec.push_back(peer_mgr_loop_->second->node_info_);
                                LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "UDP Received, JoinRequestPacket, replay2 other node ip is:" << 
                                    peer_mgr_loop_->second->end_point_ << " guid is: " << peer_mgr_loop_->first
                                    << " detect ip is:" << peer_mgr_loop_->second->node_info_.DetectIP 
                                    << " detect port is:" << peer_mgr_loop_->second->node_info_.DetectPort
                                    << " self ip is:" << peer_mgr_loop_->second->node_info_.InternalIP
                                    << " self port is:" << peer_mgr_loop_->second->node_info_.InternalPort );
                                if(node_vec.size() >= notify_back_peer_)
                                {
                                    now_guid = peer_mgr_loop_->first;
                                    break;
                                }
                            }
                        }
                    }
                    LOG4CPLUS_INFO(Loggers::NotifySvrModule(), "UDP Received, JoinRequestPacket, can not recv new peer, give peer other peer size is:" << node_vec.size());
                }                        
            }    
        }
        // response
        protocol::JoinResponsePacket response(join_request.transaction_id_, server_guid_, ret, node_vec, join_request.end_point);

        DoSendPacket(response);
        //如果这个节点接收，并且有没有完成的任务，推送给他

        //if(ret == 0 && task_mgr_.empty() == false)
        //{
        //    NOTIFY_DEBUG( "UDP Received, JoinRequestPacket, send old task size is:" << task_mgr_.size());
        //    for (map<boost::uint32_t, CPushTaskHandle>::iterator pt_it = task_mgr_.begin();
        //                                                pt_it != task_mgr_.end();
        //                                                pt_it++)
        //    {
        //        SendNotifyInfo(end_point, pt_it->second);
        //    }
        //}
    }

    void NotifySvrModule::OnPeerLeavePacket(const protocol::PeerLeavePacket & leave_packet)
    {
        map<Guid, PeerInfoHandle>::iterator it = peer_mgr_.find(leave_packet.peer_guid_);
        if(it != peer_mgr_.end())
        {
            peer_mgr_.erase(it);
        }
        it = peer_nat_vec_.find(leave_packet.peer_guid_);
        if(it != peer_nat_vec_.end())
        {
            peer_nat_vec_.erase(it);
        }
        LOG4CPLUS_DEBUG(Loggers::NotifySvrModule(), "UDP Received, PeerLeavePacket, erase peer, now peer size is:" << peer_mgr_.size());
    }

    void NotifySvrModule::OnNotifyKeepAliveRequestPacket(const protocol::NotifyKeepAliveRequestPacket & keepalive_packet)
    {
        LOG4CPLUS_DEBUG(Loggers::NotifySvrModule(), "UDP Received, NotifyKeepAliveRequestPacket");
        bool need_ret = true;

        map<Guid, PeerInfoHandle>::iterator it = peer_mgr_.find(keepalive_packet.peer_guid_);
        if(it != peer_mgr_.end())
        {
            it->second->last_recv_time_ = time(NULL);
            //通过Keepalive统计当前的人数
            it->second->online_count_ = keepalive_packet.peer_online_;

            for (int i = 0; i < keepalive_packet.task_info_.size(); i++)
            {
                map<boost::uint32_t, boost::uint32_t>::iterator itt = 
                    it->second->complete_task_map_.find(keepalive_packet.task_info_[i].TaskID);
                if(itt == it->second->complete_task_map_.end())
                {
                    it->second->complete_task_map_.insert(make_pair(keepalive_packet.task_info_[i].TaskID, 
                        keepalive_packet.task_info_[i].CompleteCount));
                }
                else
                {
                    itt->second = keepalive_packet.task_info_[i].CompleteCount;
                }

            }
            //检查是否有服务器已经有任务而对方没有完成的任务
            for(map<boost::uint32_t, CPushTaskHandle>::iterator task_it = task_mgr_.begin(); task_it != task_mgr_.end(); task_it++)
            {
                map<boost::uint32_t, boost::uint32_t>::iterator peer_have_task_it = 
                    it->second->complete_task_map_.find(task_it->first);
                if(peer_have_task_it == it->second->complete_task_map_.end())
                {
                    //通知Peer这个任务
                    SendNotifyInfo(keepalive_packet.end_point, task_it->second);
                }
            }
        }
        else
        {
            //不给他回，让他自动超时重新登录
            need_ret = false;
        }
        if(need_ret)
        {
            protocol::NotifyKeepAliveResponsePacket response(keepalive_packet.transaction_id_, keepalive_packet.end_point);
            DoSendPacket(response);
        }
    }

    void NotifySvrModule::OnNotifyResponsePacket(const protocol::NotifyResponsePacket & packet)
    {
    }

    PeerInfoHandle NotifySvrModule::GetPeerInfoHandle(const protocol::JoinRequestPacket & join_request)
    {
        PeerInfoHandle pi;
        pi.reset(new PeerInfo());

        pi->node_info_.PeerGuid = join_request.peer_guid_;
        pi->node_info_.InternalIP = join_request.internal_ip_;
        pi->node_info_.InternalPort = join_request.internal_port_;
        pi->node_info_.DetectIP = join_request.detect_ip_;
        if(pi->node_info_.DetectIP == 0)
            pi->node_info_.DetectIP = join_request.end_point.address().to_v4().to_ulong();
        pi->node_info_.DetectPort = join_request.detect_port_;
        if(pi->node_info_.DetectPort == 0)
            pi->node_info_.DetectPort = join_request.end_point.port();
        pi->node_info_.StunIP = join_request.stun_ip_;
        pi->node_info_.StunPort = join_request.stun_port_;
        pi->end_point_ = join_request.end_point;

        return pi;
    }

    void NotifySvrModule::RegisterAllPackets()
    {
        protocol::register_notify_packet(*udp_server_);
    }
}