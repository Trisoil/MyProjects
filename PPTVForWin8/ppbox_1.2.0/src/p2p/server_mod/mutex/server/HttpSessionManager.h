// HttpSessionManager.h

#ifndef _MUTEX_SERVER_HTTP_SESSION_MANAGER_H_
#define _MUTEX_SERVER_HTTP_SESSION_MANAGER_H_

#include "mutex/server/Common.h"
#include "mutex/server/HttpServer.h"

//#include <boost/multi_index_container.hpp>
//#include <boost/multi_index/hashed_index.hpp>
//#include <boost/multi_index/indexed_by.hpp>
//using namespace boost::multi_index;

namespace mutexserver
{
    class HttpServer;

    class HttpSessionManager
    {
    private:
        struct HttpSessionInfo
        {
            std::string sessionID_;
            HttpServer* pHttp_;
            boost::uint64_t lastActive_;

            HttpSessionInfo(
                std::string sessionID,
                HttpServer* pHttp)
                : sessionID_(sessionID)
                , pHttp_(pHttp)
            {
                lastActive_ = get_last_time();
            }
        };

        typedef multi_index_container<
            HttpSessionInfo,
            indexed_by<
            hashed_unique<
            BOOST_MULTI_INDEX_MEMBER(HttpSessionInfo, std::string, sessionID_)
            >,
            sequenced<>
            >
        > SessionContainer;
        typedef SessionContainer::nth_index<0>::type SessionContainerHashedIndex;
        typedef nth_index<SessionContainer,1>::type SessionContainerSequencedIndex;

    private:
        static HttpSessionManager * inst_;
        SessionContainer container_;

        HttpSessionManager(){}

    public:
        static HttpSessionManager * Inst() { return inst_; }

        void addSession(
            std::string sessionID, 
            HttpServer* pHttp_);

        void clearSession(
            std::string sessionID);

        void kickOutSession(
            std::string sessionID);

        void timeOutSessionReponse();
    };
}

#endif // _MUTEX_SERVER_HTTP_SESSION_MANAGER_H_
