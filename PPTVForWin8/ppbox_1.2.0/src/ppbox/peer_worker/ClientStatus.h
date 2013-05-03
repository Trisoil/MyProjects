// ClientStatus.h

#ifndef _PPBOX_PEER_WORKER_CLIENT_STATUS_H_
#define _PPBOX_PEER_WORKER_CLIENT_STATUS_H_

#include <framework/container/List.h>
#include <framework/memory/SharedMemoryPointer.h>
#include <framework/memory/MemoryPoolObject.h>

namespace ppbox
{
    namespace peer_worker
    {

#define SHARED_OBJECT_ID_DEMUX 2

        class ClientStatus
            : public framework::container::ListHook<
                ClientStatus, 
                framework::memory::SharedMemoryPointerTraits<ClientStatus, SHARED_MEMORY_INST_ID>
            >::type
                , public framework::memory::MemoryPoolObjectNoThrow<
                ClientStatus, 
                framework::memory::BigFixedPool
            >
        {
        public:
            ClientStatus()
                : buffer_time_(0)
                , adv_expire_(0)
            {
                ::memset(url_, 0, sizeof(url_));
            };

            ~ClientStatus()
            {
            };

        public:
            std::string current_url()
            {
                return url_;
            }

            size_t buffer_time()
            {
                if ((boost::uint32_t)time(NULL) >= adv_expire_)
                    return buffer_time_;
                else
                    return buffer_time_ + size_t(adv_expire_ - time(NULL)) * 1000;
            }

        public:
            void set_adv_duration(
                boost::uint32_t duration)
            {
                adv_expire_ = (boost::uint32_t)time(NULL) + duration;
            }

            void set_current_url(
                std::string const & url)
            {
                strncpy(url_, url.c_str(), sizeof(url_));
            }

            void update_buffer_time(
                boost::uint32_t buffer_time)
            {
                buffer_time_ = buffer_time;
            }

        private:
            boost::uint32_t buffer_time_;
            boost::uint32_t adv_expire_;
            char url_[1024];
        };

    } // namespace peer_worker
} // namespace ppbox

#endif // _PPBOX_PEER_WORKER_CLIENT_STATUS_H_
