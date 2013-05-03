// MutexSession.cpp

#include "mutex/server/MutexSession.h"
#include "mutex/server/UdpServer.h"
#include "mutex/server/MutexServer.h"
#include "mutex/server/HttpSessionManager.h"
using namespace mutex;

#include <util/archive/ArchiveBuffer.h>
#include <util/protocol/pptv/CheckSum.h>
using namespace util::archive;

#include <boost/lexical_cast.hpp>

namespace mutexserver
{
    FRAMEWORK_LOGGER_DECLARE_MODULE("MutexSession");

    MutexSession* MutexSession::inst_(new MutexSession());

    // 返回true 则HTTP立即返回
    bool MutexSession:: KeepAlive(
        protocol::KeepaliveRequestPackage & request,
        protocol::KeepaliveResponePackage & response,
        boost::shared_ptr<boost::asio::ip::udp::endpoint>& remote_endpoint,
        ProtocolType pType)
    {
        bool re = false;
#ifdef _STDLOG_
        std::cout<<"keepalive =============================\n";
        std::cout<<"client_id:"<<request.client_id<<"\n";
        std::cout<<"session_id:"<<byteArray2Str(request.session_id,16)<<"\n";
        std::cout<<"time:"<<request.time<<"\n";
        std::cout<<"enc:"<<byteArray2Str(request.enc,16)<<"\n";
#endif
        ClientContainerHashedIndex & client_index = container_.get<0>();

        ClientContainer::iterator iter = client_index.find(request.client_id);
        response.interval = get_config_interval();

        if (iter == client_index.end()) {
            //校验MEV
#ifdef _MEV_
            //std::string mev = byteArray2Str(request.enc, 16);
            //if(!check_md5(request.time,request.client_id,mev)){
            //	response.passstate=KEEPALIVE_MEV_FAILED;	
            //	LOG_F(Logger::kLevelDebug, ("new box[%s] mev(%s) time(%ld) mev check failed",request.client_id.c_str(),mev.c_str(),(long)request.time));
            //	return true;
            //}
#endif
            //未找到对应的client_id 插入
            ClientInfo pInfo = ClientInfo(request.client_id,request.session_id,request.time,*remote_endpoint.get(),pType);
            response.passstate=KEEPALIVE_PASS;
          
            LOG_F(Logger::kLevelDebug, ("new box[%s] keepalive",request.client_id.c_str()));
            insert(pInfo);
            re=true;
        } else {
            //判断sessionID是否相等
            //pInfo = const_cast<ClientInfo*>(&(*iter));
            ClientInfo clientInfo = *iter;

            if (session_id_equal(iter->session_id_,request.session_id)) {
                response.passstate = KEEPALIVE_PASS;
                //更新lastactive
                client_index.modify(iter,ChangeClientLastActive(get_last_time()));
                LOG_F(Logger::kLevelInfor, ("box[%s] keepalive ok",request.client_id.c_str()));

                insert(clientInfo);
            } else {
                if (request.time>iter->loginTime_) {
                    //校验MEV
#ifdef _MEV_
                    std::string mev =byteArray2Str(request.enc, 16);
                    if(!check_md5(request.time,request.client_id,mev)){
                        response.passstate=KEEPALIVE_MEV_FAILED;	
                        LOG_F(Logger::kLevelDebug, ("new box[%s] keepalive mev check failed",request.client_id.c_str()));
                        return true;
                    }
#endif
                    //发生踢人
                    if(clientInfo.protocolType_ == UDP){ 
                        protocol::Head headResponse;
                        protocol::KickoutPackage kickOutResponse;
                        headResponse.action=kickOutResponse.action;
                        headResponse.sequece=0;
                        kickOutResponse.client_id=clientInfo.clientID_;
                        for(int i=15;i>=0;i--){
                            kickOutResponse.session_id[i]=clientInfo.session_id_[i];
                        }
                        boost::shared_array<char> send_buf(new char[1024]);

                        ArchiveBuffer<char> aoBuf(send_buf.get(), 1024);
                        protocol::OUdpArchive oa(aoBuf);
                        oa<<headResponse;
                        oa<<kickOutResponse;
                     
                        boost::uint16_t cs_out =  pptv::check_sum((boost::uint16_t*)(send_buf.get()+2),(aoBuf.size()-2));
                        boost::uint16_t * pcs = (boost::uint16_t*)send_buf.get();
                        *pcs = cs_out;
                        headResponse.checksum=cs_out;

                        getUdpServer()->UdpSendTo(aoBuf.data(1024),clientInfo.remote_endpoint_);
                    } else {
                        std::string sessionID= byteArray2Str(clientInfo.session_id_,16);
                        HttpSessionManager::Inst()->kickOutSession(sessionID);
                    }
#ifdef _STDLOG_
                    std::cout<<"KickOut =============================\n";
                    std::cout<<"client_id:"<<request.client_id<<"\n";
                    std::cout<<"session_id:"<<byteArray2Str(clientInfo.session_id_,16)<<"\n";
#endif
                    LOG_F(Logger::kLevelInfor, ("box[%s] be kicked! sessionID[%s]",request.client_id.c_str(),byteArray2Str(clientInfo.session_id_,16).c_str()));
                    //更新ClientInfo
                    client_index.modify(iter,ChangeClientSession(get_last_time(),request.time,*remote_endpoint.get(),request.session_id,pType));

                    response.passstate=KEEPALIVE_PASS;

                    insert(clientInfo);
                } else {
                    //当前 KEEPALIVE 失败 不发生更新
                    response.passstate=KEEPALIVE_FAILED;
                    LOG_F(Logger::kLevelInfor, ("box[%s] keepalive failed!",request.client_id.c_str()));
                    return true;
                }
                re=true;
            }
        }
        return re;
    }

    void MutexSession::Leave(
        protocol::LeavePackage &request)
    {
#ifdef _STDLOG_
        std::cout<<"leave =============================\n";
        std::cout<<"client_id:"<<request.client_id<<"\n";
        std::cout<<"session_id:"<<byteArray2Str(request.session_id,16)<<"\n";
#endif

        ClientContainerHashedIndex & client_index = container_.get<0>();
        ClientContainer::iterator iter = client_index.find(request.client_id);
        if (iter != client_index.end()){
            if(session_id_equal(iter->session_id_,request.session_id)){
                ClientInfo info = *iter;
                LOG_F(Logger::kLevelInfor, ("box[%s] leave",info.clientID_.c_str()));
                container_.erase(iter);

                //delete pInfo;//内存清理
            }
        }
    }

    bool MutexSession::session_id_equal(
        const boost::uint8_t *first, 
        const boost::uint8_t * second)
    {
        for(int i=15;i>=0;i--){
            if(first[i]!=second[i]){
                return false;
            }
        }
        return true;
    }

    void MutexSession::ClearExpiredSession()
    {
        ClientContainerSequencedIndex & seq_index = get<1>(container_);

        boost::uint64_t now = get_last_time();
        while(true){
            ClientContainerSequencedIndex::iterator iter=seq_index.end();
            if(iter==seq_index.begin()){
                break;
            }
            iter--;
            if(now - iter->lastActive_<MAX_KEEPALIVE) {
                break;
            } else {
                ClientInfo info = *iter;
                LOG_F(Logger::kLevelInfor, ("clear box[%s]",info.clientID_.c_str()));
                seq_index.erase(iter);
                //delete pInfo;//内存清理
            }
        }
    }

    void MutexSession::insert(
        ClientInfo & info)
    {
        ClientContainerSequencedIndex & s_index = container_.get<1>();
        std::pair<ClientContainerSequencedIndex::iterator,bool> p=s_index.push_front(info);
        if(!p.second){                     /* duplicate item */
            s_index.relocate(s_index.begin(),p.first); /* put in front */
        }
    }

    std::string MutexSession::QueryCount()
    {
        return "Query Count: " + boost::lexical_cast<std::string>(container_.size());
    }

    std::string MutexSession::QueryNew()
    {
        ClientContainerSequencedIndex & s_index = container_.get<1>();
        ClientContainerSequencedIndex::iterator iter=s_index.begin();
        int count = 0;
        std::string str;
        while(iter!=s_index.end()){
            count++;
            if(count>=100){
                break;
            }
            str.append(iter->clientID_.c_str());
            str.append("\t");
            str.append(byteArray2Str(iter->session_id_,16));
            str.append("\t");
            str.append(boost::lexical_cast<std::string>(iter->lastActive_));
            str.append("\n");

            iter++;
        }
        return str;
    }

    std::string MutexSession::QueryOld()
    {
        ClientContainerSequencedIndex & s_index = container_.get<1>();
        ClientContainerSequencedIndex::iterator iter=s_index.end();
        int count = 0;
        std::string str;
        str.append("QueryOld\n");
        if(container_.size()==0){
            return str;
        }
        iter--;
        while(true){
            count++;
            if(count>=100){
                break;
            }
            str.append(iter->clientID_.c_str());
            str.append("\t");
            str.append(byteArray2Str(iter->session_id_,16));
            str.append("\t");
            str.append(boost::lexical_cast<std::string>(iter->lastActive_));
            str.append("\n");
            if(iter==s_index.begin()){
                break;
            }
            iter--;
        }
        return str;
    }

    std::string MutexSession::QueryByID(std::string clientID)
    {
        std::string str;
        str.append("QueryByID:" +clientID + "\r\n" );

        ClientContainerHashedIndex & client_index = container_.get<0>();
        ClientContainer::iterator iter = client_index.find(clientID);
        if (iter == client_index.end()){
            str.append("Not Found :" + clientID + "\n");
        }else{
            str.append("SessionID: " +  byteArray2Str(iter->session_id_,16) + "\n");
            str.append("LastActive: " + boost::lexical_cast<std::string>(iter->lastActive_) + "\n");
            str.append("LoginTime: " +  boost::lexical_cast<std::string>(iter->loginTime_) + "\n");
            std::string pro = iter->protocolType_ == HTTP ? "http" : "udp";
            str.append("ProtocolType: "+  pro + "\n");
        }
        return str;
    }
}
