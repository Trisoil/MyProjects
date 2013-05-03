// HttpSessionManager.cpp

#include "mutex/server/HttpSessionManager.h"
#include "mutex/server/HttpServer.h"

namespace mutexserver
{
    FRAMEWORK_LOGGER_DECLARE_MODULE("MutexSession");

    HttpSessionManager * HttpSessionManager::inst_(new HttpSessionManager());

    void HttpSessionManager::addSession(
        std::string sessionID, 
        HttpServer* pHttp)
    {
        SessionContainerSequencedIndex & s_index = container_.get<1>();
        HttpSessionInfo info(sessionID, pHttp);

        std::pair<SessionContainerSequencedIndex::iterator,bool> p = s_index.push_front(info);
        if (!p.second) {                     /* duplicate item */
            s_index.relocate(s_index.begin(), p.first); /* put in front */
        }
    }

    void HttpSessionManager::timeOutSessionReponse()
    {
        SessionContainerSequencedIndex & seq_index = get<1>(container_);

        boost::uint64_t now = get_last_time();
        while(true) {
            SessionContainerSequencedIndex::iterator iter = seq_index.end();
            if (iter == seq_index.begin()) {
                break;
            }
            iter--;
            if (now - iter->lastActive_ < 18) {
                break;
            } else {
                HttpSessionInfo info = *iter;
                LOG_F(Logger::kLevelInfor, ("session[%s] return", info.sessionID_.c_str()));
                info.pHttp_->return_now(true);
                seq_index.erase(iter);
            }
        }
    }

    void HttpSessionManager::clearSession(
        std::string sessionID)
    {
        SessionContainerHashedIndex & client_index = container_.get<0>();
        SessionContainerHashedIndex::iterator iter = client_index.find(sessionID);
        if (iter != client_index.end()) {
            HttpSessionInfo info = *iter;
            LOG_F(Logger::kLevelInfor, ("session[%s] clear", info.sessionID_.c_str()));
            container_.erase(iter);
            //delete pInfo;//内存清理
        }
    }

    void HttpSessionManager::kickOutSession(
        std::string sessionID)
    {
        SessionContainerHashedIndex & client_index = container_.get<0>();
        SessionContainerHashedIndex::iterator iter = client_index.find(sessionID);
        if (iter != client_index.end()) {
            HttpSessionInfo info = *iter;
            LOG_F(Logger::kLevelInfor, ("session[%s] kickedOut", info.sessionID_.c_str()));
            info.pHttp_->return_now(false);
            container_.erase(iter);
            //delete pInfo;//内存清理
        }
    }
}
