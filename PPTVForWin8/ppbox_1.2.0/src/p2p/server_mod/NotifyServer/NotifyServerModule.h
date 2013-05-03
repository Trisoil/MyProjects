#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/ip/udp.hpp>
#include <vector>
#include <map>
#include <set>
#include "network/HttpServer.h"
#include "protocol/UdpServer.h"
#include "util/protocol/http/HttpClient.h"
#include "MainThread.h"

#ifndef BOOST_WINDOWS_API
#include "iconv.h"
#endif

using namespace std;

static int utf8_to_ucs2(const char *in, int inbytes, wchar_t *out, int outwords)
{
    int newch, mask;
    int expect, eating;
    int ch;
    int ret = 0;

    while (inbytes && ret < outwords)
    {
        ch = (unsigned char)(*in++);
        if (!(ch & 0200))
        {
            /* US-ASCII-7 plain text */
            --inbytes;
            ret++;
            if (out)
                *(out++) = ch;
        }
        else
        {
            if ((ch & 0300) != 0300)
            {
                /* Multibyte Continuation is out of place */
                return -1;
            }
            else
            {
                /* Multibyte Sequence Lead Character
                *
                * Compute the expected bytes while adjusting
                * or lead byte and leading zeros mask.
                */
                mask = 0340;
                expect = 1;
                while ((ch & mask) == mask)
                {
                    mask |= mask >> 1;
                    if (++expect > 3) /* (truly 5 for ucs-4) */
                        return -1;
                }
                newch = ch & ~mask;
                eating = expect + 1;
                if (inbytes <= expect)
                    return ret;
                /* Reject values of excessive leading 0 bits
                * utf-8 _demands_ the shortest possible byte length
                */
                if (expect == 1)
                {
                    if (!(newch & 0036))
                        return -1;
                }
                else {
                    /* Reject values of excessive leading 0 bits */
                    if (!newch && !((unsigned char)*in & 0077 & (mask << 1)))
                        return -1;
                    if (expect == 2)
                    {
                        /* Reject values D800-DFFF when not utf16 encoded
                        * (may not be an appropriate restriction for ucs-4)
                        */
                        if (newch == 0015 && ((unsigned char)*in & 0040))
                            return -1;
                    }
                    else if (expect == 3)
                    {
                        /* Short circuit values > 110000 */
                        if (newch > 4)
                            return -1;
                        if (newch == 4 && ((unsigned char)*in & 0060))
                            return -1;
                    }
                }
                /* Where the boolean (expect > 2) is true, we will need
                * an extra word for the output.
                */
                if (ret + (expect > 2) + 1 >= outwords)
                    break; /* buffer full */
                while (expect--)
                {
                    /* Multibyte Continuation must be legal */
                    if (((ch = (unsigned char)*(in++)) & 0300) != 0200)
                        return -1;
                    newch <<= 6;
                    newch |= (ch & 0077);
                }
                inbytes -= eating;
                /* newch is now a true ucs-4 character
                *
                * now we need to fold to ucs-2
                */
                if (newch < 0x10000)
                {
                    ret++;
                    if (out)
                        *(out++) = (wchar_t) newch;
                }
                else
                {
                    ret += 2;
                    if (out)
                    {
                        newch -= 0x10000;
                        *(out++) = (wchar_t) (0xD800 | (newch >> 10));
                        *(out++) = (wchar_t) (0xDC00 | (newch & 0x03FF));
                    }
                }
            }
        }
    }
    /* Buffer full 'errors' aren't errors, the client must inspect both
    * the inbytes and outwords values
    */
    return ret;
}

inline wstring Utf8ToUtf16(const char* psz, int nLength = -1)
{
    if (nLength == -1)
        nLength = psz != NULL ? strlen(psz) : 0;

    wstring strUtf16;
    if (nLength == 0)
        return strUtf16;

    int nDstLen = nLength * sizeof(wchar_t) + 1;
    strUtf16.resize(nDstLen);
    wchar_t* pwsz = (wchar_t*)strUtf16.c_str();//GetBuffer(nDstLen);
    if (pwsz != NULL)
    {
        int nRetLen = utf8_to_ucs2(psz, nLength, pwsz, nDstLen);
        //strUtf16.ReleaseBuffer(nRetLen);
        //if (nRetLen < nDstLen - 1)
        //    strUtf16. FreeExtra();
    }
    return strUtf16;
}

inline string Utf16ToAnsi(const wchar_t* pwsz)
{
    string strAnsi;
    if (pwsz == NULL || *pwsz == '\0')
        return strAnsi;

#ifdef BOOST_WINDOWS_API
    int nLength = ::WideCharToMultiByte(CP_ACP, 0, pwsz, -1, NULL, 0, NULL, NULL);
    strAnsi.resize(nLength);
    nLength = ::WideCharToMultiByte(CP_ACP, 0, pwsz, -1, (char*)strAnsi.c_str(), nLength, NULL, NULL);
#else
    int nLength = wcstombs(NULL, pwsz, MB_CUR_MAX);
    char *tmp = (char *)malloc(nLength);
    wcstombs(tmp, pwsz, nLength);
    strAnsi = tmp;
    free(tmp);
#endif
    //strAnsi.ReleaseBuffer(nLength - 1);
    return strAnsi;
}


inline string Utf8ToAnsi(const char* psz)
{
    wstring strUtf16 = Utf8ToUtf16(psz);
    string strAnsi = Utf16ToAnsi(strUtf16.c_str());
    return strAnsi;
}


class PeerInfo
{
public:
    PeerInfo();
    ~PeerInfo();
    protocol::NodeInfo node_info_;
    time_t last_recv_time_;//上次收到数据的时�?
    map<boost::uint32_t, boost::uint32_t> complete_task_map_;//任务完成数的map,Key是task id,value是完成数
    boost::uint32_t recv_count_;//收到的统计完成个�?到时候web需要的时候就把所有的加起�?
    boost::uint32_t online_count_;//倒是的统计在线个�?
    //Guid peer_id_;
    boost::asio::ip::udp::endpoint end_point_;
};
typedef boost::shared_ptr<PeerInfo> PeerInfoHandle;

class CPushTask
{
public:
    CPushTask();
    ~CPushTask();

    vector<char> content_;
    boost::uint16_t task_type_;
    boost::uint32_t task_id_;
    boost::uint32_t start_time_;//起始时间
    boost::uint8_t  state_;//0是没有开始，1是正在进行，2是已经结�?3是已经取�?
    boost::uint32_t duration_;//Peer根据他去做延时下�?
    boost::uint32_t rest_time_;//ai 
    boost::uint32_t complete_count_;//完成以后的完成数会记录在这里
};
typedef boost::shared_ptr<CPushTask> CPushTaskHandle;

namespace network
{
    class HttpResponse;
}

namespace NotifyServer
{
    class NotifyHttpSvrConn
        : public boost::noncopyable
        , public boost::enable_shared_from_this<NotifyHttpSvrConn>
        , public network::IHttpServerListener
    {
    public:
        typedef boost::shared_ptr<NotifyHttpSvrConn> p;
        static p create(boost::shared_ptr<network::HttpServer> http_server_socket) { 
            return p(new NotifyHttpSvrConn(http_server_socket)); 
        }
        NotifyHttpSvrConn(boost::shared_ptr<network::HttpServer> http_server_socket);
        ~NotifyHttpSvrConn();
        // 方法
        void Start();
        void Stop();
        void WillStop();
    public:
        // 消息
        // HttpServer
        virtual void OnHttpRecvSucced(boost::shared_ptr<network::HttpRequest> http_request);
        virtual void OnHttpRecvFailed(boost::uint32_t error_code);
        virtual void OnHttpRecvTimeout();
        virtual void OnTcpSendSucced(size_t length);
        virtual void OnTcpSendFailed();
        virtual void OnTcpAck();
        virtual void OnNoTcpAck();
        virtual void OnTcpSendTimeout();
        virtual void OnClose();

        boost::shared_ptr<network::HttpRequest> http_request_demo_;
        boost::shared_ptr<network::HttpServer> http_server_socket_;
        volatile bool is_running_;
        bool will_stop_;
    };
    
    //////////////////////////////////////////////////////////////////////////

    class NotifySvrModule
        : public boost::noncopyable
        , public boost::enable_shared_from_this<NotifySvrModule>
        , public protocol::IUdpServerListener
        , public network::IHttpAcceptorListener
        /*, public util::protocol::HttpClient*/
    {
    public:
        typedef boost::shared_ptr<NotifySvrModule> p;
        static p Create() { return p(new NotifySvrModule); }
    public:
        void Start(boost::uint16_t local_udp_port, boost::uint16_t http_server_port);
        void Stop();
        bool IsRunning() const { return is_running_; }

        void OnUdpRecv(protocol::Packet const & packet);
        void OnHttpAccept(boost::shared_ptr<network::HttpServer> http_server_for_accept);
        void OnHttpAcceptFailed();
        void PrintNetInfo(bool is_timer);

        void ConsoleLogListPeerGuidCount() 
        {
            std::cout << "$Peer Count: " << peer_mgr_.size() << std::endl;
        }
        boost::uint32_t GetPeerReportCount()
        {
            boost::uint32_t size = 0;
            for(map<Guid, PeerInfoHandle>::iterator it = peer_mgr_.begin(); it != peer_mgr_.end(); it++)
            {
                size += it->second->online_count_;
            }
            return size;
        }

        void ConsoleLogListPeerReportCount() 
        {
            boost::uint32_t size = GetPeerReportCount();
            std::cout << "$Peer report Count: " << size << std::endl;
        }
        void ConsoleLogListPeerReportCompleteCount(boost::uint32_t task_id)
        {
            boost::uint32_t size = GetPeerReportCompleteCount(task_id);
            std::cout << "$Peer report Count: " << size << std::endl;
        }

        boost::uint32_t GetPeerReportCompleteCount(boost::uint32_t task_id)
        {
            boost::uint32_t size = 0;
            for(map<Guid, PeerInfoHandle>::iterator it = peer_mgr_.begin(); it != peer_mgr_.end(); it++)
            {
                map<boost::uint32_t, boost::uint32_t>::iterator itt = it->second->complete_task_map_.find(task_id);
                if (itt != it->second->complete_task_map_.end())
                {
                    size += itt->second;
                }
            }
            return size;
        }

        void ConsoleLogListTaskCount()
        {
            std::cout << "$Task Count: " << task_mgr_.size() << std::endl;
        }
        void ConsoleLogListTaskInfo(boost::uint32_t task_id);
        void TestATask(boost::uint32_t tid, vector<char>& data_vec, boost::uint32_t resttime, boost::uint32_t duration, boost::uint32_t type);

        void ConsoleLogListAllTask()
        {
            for (map<boost::uint32_t, CPushTaskHandle>::iterator it = task_mgr_.begin(); it != task_mgr_.end(); it++)
            {
                CPushTaskHandle handle = it->second;
                std::cout << "Task id: " << handle->task_id_ << " start time:" << handle->start_time_
                    << " tasktype:" << handle->task_type_ << std::endl <<
                    " state_:" << (boost::uint32_t)handle->state_ << 
                    " duration:" << handle->duration_ << std::endl << 
                    " resttime:" << handle->rest_time_ << std::endl;
                ConsoleLogListPeerReportCompleteCount(handle->task_id_);
            }
            for (map<boost::uint32_t, CPushTaskHandle>::iterator itt = complete_task_mgr_.begin(); itt != complete_task_mgr_.end(); itt++)
            {
                CPushTaskHandle handle = itt->second;
                std::cout << "Task id: " << handle->task_id_ << " start time:" << handle->start_time_
                    << " tasktype:" << handle->task_type_ << std::endl <<
                    " state_:" << (boost::uint32_t)handle->state_ << 
                    " duration:" << handle->duration_ << std::endl << 
                    " resttime:" << handle->rest_time_ << std::endl;
                ConsoleLogListPeerReportCompleteCount(handle->task_id_);
                //CONSOLE_OUTPUT("$Peer report Count: " << itt->second->complete_count_);
            }
        }

        void ConsoleReleaseTask(boost::uint32_t task_id)
        {
            if(ReleaseTask(task_id))
            {
                std::cout << "Success release task:" << task_id << std::endl;
            }
            else
            {
                std::cout << "Release task err, because there is no task:" << task_id << std::endl;
            }

        }

        bool ReleaseTask(boost::uint32_t task_id)
        {
            map<boost::uint32_t, CPushTaskHandle>::iterator it = task_mgr_.find(task_id);
            if(it != task_mgr_.end())
            {
                it->second->rest_time_ = 0;
                it->second->state_ = 3;
                complete_task_mgr_.insert(make_pair(task_id, it->second));
                task_mgr_.erase(it);
                return true;
            }
            return false;
        }


        void ConsoleLogListAllPeerNum()
        {
            std::cout << "$Nat Peer Count: " << peer_nat_vec_.size() << std::endl;
            std::cout << "$Public Peer Count: " << peer_mgr_.size() - peer_nat_vec_.size() << std::endl;
        }

        void ConsoleLogListPublicPeerInfo()
        {
            bool is_show;
            for (map<Guid, PeerInfoHandle>::iterator it = peer_mgr_.begin(); it != peer_mgr_.end(); it++)
            {
                is_show = true;
                map<Guid, PeerInfoHandle>::iterator itt = peer_nat_vec_.find(it->second->node_info_.PeerGuid);
                if(itt != peer_nat_vec_.end())
                {
                    is_show = false;
                }
                if(is_show)
                {
                    std::cout << "$Public Peer: " << it->second->end_point_.address().to_string()
                    << " guid is:" << it->second->node_info_.PeerGuid.to_string() << std::endl;
                }
            }
        }

        void ConsoleLogListNatPeerInfo()
        {
            for (map<Guid, PeerInfoHandle>::iterator it = peer_nat_vec_.begin(); it != peer_nat_vec_.end(); it++)
            {
                std::cout << "$Nat Peer: " << (it)->second->end_point_.address().to_string()
                    << " guid is:" << it->second->node_info_.PeerGuid.to_string() << std::endl;
            }
        }

        void LoadConfig();

    private:
        NotifySvrModule()
            : is_running_(false)
        {

        };

        NotifySvrModule(const NotifySvrModule & notify);

        bool is_running_;

        size_t keep_alive_time_;

        size_t time_start_;

        boost::shared_ptr<framework::timer::PeriodicTimer> print_net_info_timer_;

        static boost::shared_ptr<NotifySvrModule> inst_;
        boost::shared_ptr<protocol::UdpServer> udp_server_;


    public:
        static boost::shared_ptr<NotifySvrModule> Inst() { return inst_; };

        boost::shared_ptr<boost::asio::deadline_timer> check_alive_timer_;

        boost::shared_ptr<boost::asio::deadline_timer> check_task_timer_;//检查任务超�?

        //accept
        network::HttpAcceptor::pointer acceptor_;

        void RemoveNotifyConnection(boost::shared_ptr<NotifyHttpSvrConn> server_socket);
        
        set<boost::shared_ptr<NotifyHttpSvrConn> > notify_http_conn_set;

        
        virtual void OnConnectSucced();
        virtual void OnConnectFailed(u_int error_code);
        virtual void OnConnectTimeout();
        virtual void OnRecvHttpHeaderSucced(boost::shared_ptr<network::HttpResponse> http_response);
        virtual void OnRecvHttpHeaderFailed(u_int error_code);
        virtual void OnRecvHttpDataSucced(const boost::asio::streambuf & buffer, size_t file_offset, size_t content_offset);
        virtual void OnRecvHttpDataPartial(const boost::asio::streambuf &buffer, size_t file_offset, size_t content_offset);
        virtual void OnRecvHttpDataFailed(u_int error_code);
        virtual void OnRecvTimeout();
        virtual void OnComplete();

        void SendHttpRequest();
        
        void OnNotifyNewTask(CPushTaskHandle pt);
        void SendNotifyInfo(const boost::asio::ip::udp::endpoint& end_point, CPushTaskHandle pt);

        template<typename type>
        void DoSendPacket(type const & packet)
        {
            udp_server_->send_packet(packet, protocol::PEER_VERSION_V4);
        }

        protocol::UrlInfo url_info_;
        boost::shared_ptr<util::protocol::HttpClient> http_client_;
        bool is_connected_;

        boost::uint32_t max_peer_num_;//最大管理的节点数目
        map<Guid, PeerInfoHandle> peer_mgr_;//节点管理列表
        map<Guid, PeerInfoHandle> peer_nat_vec_;//内网节点列表
        //vector<PeerInfoHandle> peer_vec_;//节点的vecotr列表
        map<Guid, PeerInfoHandle>::iterator peer_mgr_loop_;//记录的一个当前迭代器位置
        Guid now_guid;
        map<boost::uint32_t, CPushTaskHandle> task_mgr_;//任务管理列表
        map<boost::uint32_t, CPushTaskHandle> complete_task_mgr_;//任务管理列表
        boost::uint8_t self_type_;
        boost::uint32_t notify_back_peer_;//返回节点个数
        boost::uint32_t udp_server_receive_count_;

        multimap<boost::uint32_t, boost::uint16_t> receive_web_map_;//接受的webmap，key是IP，value是port
        Guid server_guid_;

        static const std::string ConfigFileName;

    private:
        void OnCheckAliveTimer();
        void OnCheckTaskTimer();
        void CheckConfig();
        void OnJoinRequestPacket(const protocol::JoinRequestPacket & packet);
        void OnPeerLeavePacket(const protocol::PeerLeavePacket & packet);
        void OnNotifyKeepAliveRequestPacket(const protocol::NotifyKeepAliveRequestPacket & packet);
        void OnNotifyResponsePacket(const protocol::NotifyResponsePacket & packet);
        PeerInfoHandle GetPeerInfoHandle(const protocol::JoinRequestPacket & join_request);
        void RegisterAllPackets();
    };

#define OutputInfo(sstr, is_timer) do\
    {\
        if (is_timer == true)\
        {\
            RELEASE_OUTPUT(sstr);\
        }\
        CONSOLE_OUTPUT(sstr);\
    }while(false);
}