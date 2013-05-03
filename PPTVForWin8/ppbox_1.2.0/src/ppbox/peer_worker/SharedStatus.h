// SharedStatus.h

#ifndef _PPBOX_PEER_WORKER_SHARED_STATUS_H_
#define _PPBOX_PEER_WORKER_SHARED_STATUS_H_

namespace ppbox
{
    namespace peer_worker
    {

        class SharedStatus
        {
        public:
            struct Block
            {
                void const * ptr;
                boost::uint32_t size;
                template <typename Archive>
                void serialize(Archive & ar)
                {
                    ar & size;
                }
            };

        public:
            SharedStatus();

            ~SharedStatus();

        public:
            boost::system::error_code get_memory_blocks(
                std::map<std::string, Block> & blocks);

        private:
            struct Cache;
            Cache * cache_;
        };

    } // namespace peer_worker
} // namespace ppbox

#endif // _PPBOX_peer_worker_SHARED_STATUS_H_
