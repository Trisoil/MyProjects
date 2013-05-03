// MutexSession.h

#ifndef _MUTEX_SERVER_MUTEX_SESSION_H_
#define _MUTEX_SERVER_MUTEX_SESSION_H_

#include "mutex/server/Common.h"
#include "mutex/protocol/protocol.h"
using namespace mutex;

#include <framework/string/Md5.h>
#include <framework/string/Base16.h>
using namespace framework::string;

#include <boost/shared_ptr.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
using namespace boost::multi_index;

namespace mutexserver
{

    enum ProtocolType
    {
        UDP, HTTP
    };

    class MutexSession
        : public boost::noncopyable
        , public boost::enable_shared_from_this<MutexSession>
    {
    public:
        struct ClientInfo
        {
        public:
            std::string clientID_;
            boost::uint8_t	session_id_[16];
            boost::uint64_t lastActive_;
            boost::uint64_t loginTime_;
            boost::asio::ip::udp::endpoint remote_endpoint_;
            ProtocolType protocolType_;

        public:
            ClientInfo();

            ClientInfo(
                std::string & clientID, 
                boost::uint8_t* session_id,
                boost::uint64_t time, 
                boost::asio::ip::udp::endpoint& remote_endpoint,
                ProtocolType pType)
            {
                clientID_ = clientID;
                for (int i = 15; i >= 0; i--) {
                    session_id_[i] = session_id[i];
                }
                loginTime_ = time;
                remote_endpoint_ = remote_endpoint;
                lastActive_ = get_last_time();
                protocolType_ = pType;
            }
        };

        struct ChangeClientLastActive
        {
            ChangeClientLastActive(
                boost::uint64_t lastActive) 
                : lastActive_(lastActive)
            {}

            void operator () (ClientInfo& client_info)
            {
                client_info.lastActive_ = lastActive_;
            }

        private:
            boost::uint64_t lastActive_;
        };

        struct ChangeClientSession
        {
            ChangeClientSession(
                boost::uint64_t lastActive,
                boost::uint64_t loginTime,
                boost::asio::ip::udp::endpoint & endpoint,
                boost::uint8_t* session_id,
                ProtocolType pType)
            {
                lastActive_ = lastActive;
                loginTime_ = loginTime;
                remote_endpoint_ = endpoint;
                session_id_ = session_id;
                protocolType_ = pType;
            }

            void operator () (ClientInfo& client_info)
            {
                client_info.lastActive_ = lastActive_;
                client_info.loginTime_ = loginTime_;
                client_info.remote_endpoint_ = remote_endpoint_;
                for (int i = 15; i >= 0; i--) {
                    client_info.session_id_[i] = session_id_[i];
                }
                client_info.protocolType_ = protocolType_;
            }

        private:
            boost::uint64_t lastActive_;
            boost::uint64_t loginTime_;
            boost::asio::ip::udp::endpoint remote_endpoint_;
            boost::uint8_t* session_id_;
            ProtocolType protocolType_;
        };

    public:
        typedef multi_index_container<
            ClientInfo,
            indexed_by<
            hashed_unique<
            BOOST_MULTI_INDEX_MEMBER(ClientInfo, std::string, clientID_)
            >,
            sequenced<>
            >
        > ClientContainer;
        typedef ClientContainer::nth_index<0>::type ClientContainerHashedIndex;
        typedef nth_index<ClientContainer,1>::type ClientContainerSequencedIndex;

    public:
        //如果返回成功 则发送response，否则不发送
        bool KeepAlive(protocol::KeepaliveRequestPackage & request,protocol::KeepaliveResponePackage & response,
            boost::shared_ptr<boost::asio::ip::udp::endpoint>& remote_endpoint, ProtocolType pType);
        void Leave(protocol::LeavePackage & request);
        void ClearExpiredSession();

    public:
        //查询接口
        std::string QueryCount();
        std::string QueryNew();
        std::string QueryOld();
        std::string QueryByID(std::string clientID);

    private:
        /// const ///
        ClientContainerHashedIndex & GetClientHashedIndex()  { return container_.get<0>(); }
        ClientContainerSequencedIndex & GetClientSequencedIndex()  { return container_.get<1>(); }
        static MutexSession* inst_;

    private:
        bool session_id_equal(const boost::uint8_t * first,const boost::uint8_t* second);
        MutexSession(){}
        void insert(ClientInfo & info);

    private:
        ClientContainer container_;

    public:
        static MutexSession* Inst(){ return inst_; }
     
        static const int KEEPALIVE_PASS=0;
        static const int KEEPALIVE_FAILED=1;
        static const int KEEPALIVE_MEV_FAILED=2;
        static const boost::uint16_t MAX_KEEPALIVE=100;

        static bool check_md5(
            boost::uint64_t time, 
            std::string clientID,
            std::string mev)
        {
            Md5 md5;
            std::string key_md5_check;
            key_md5_check += format(time);
            key_md5_check += clientID;
            key_md5_check += "ajsd0*)(jasf*";
            md5.update((unsigned char *)&key_md5_check[0], key_md5_check.size());
            md5.final();
            std::string digest((char const *)md5.digest(), 16);
            if (Base16::encode(digest, false) != mev) {
                return false;
            } else {
                return true;
            }
        }
    };
}

#endif // _MUTEX_SERVER_MUTEX_SESSION_H_
