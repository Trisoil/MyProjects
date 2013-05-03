// PeerWorkStatus.h

#ifndef _TOOLS_STATUS_PEER_WORK_STATUS_H_
#define _TOOLS_STATUS_PEER_WORK_STATUS_H_

#include <boost/asio/streambuf.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread/mutex.hpp>

namespace boost
{
    class thread;
}

namespace tools
{
    namespace status
    {

        struct Block
        {
            size_t size;
            const void * ptr;

            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & size;
            }
        };

        class PeerWorkStatus
        {
        public:
            PeerWorkStatus();
            ~PeerWorkStatus();

            void set_address(
                std::string const & address_server);
            bool start();
            bool swap_buffer(boost::asio::streambuf **first,
                boost::asio::streambuf **second);
            void status_transfer();
            int get_info(
                std::map<std::string, Block> & blocks);
            void terminate();

        private:
            boost::asio::io_service io_svc_;
            std::string address_server_;
            std::string peer_pid_;
            std::string recv_buf_;
            boost::thread * thread_;
            bool stopped_;
            //boost::shared_mutex mutex_;
            boost::mutex mutex_;
            //boost::mutex::scoped_lock *lock;
            boost::asio::streambuf head_buf_[3];
            boost::asio::streambuf * read_buf_;
            boost::asio::streambuf * middle_buf_;
            boost::asio::streambuf * write_buf_;
            std::map<std::string, Block> blocks_;
            
            std::vector<boost::asio::const_buffer> packet_buf_;
        };
    }
}

#endif // _TOOLS_STATUS_PEER_WORK_STATUS_H_
