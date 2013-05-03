// PeerWorkStatus.cpp

#include "tools/status/Common.h"
#include "tools/status/PeerWorkStatus.h"
#include "tools/status/PpboxStatusInterface.h"

#include <util/serialization/stl/map.h>
#include <util/archive/LittleEndianBinaryIArchive.h>
#include <util/protocol/http/HttpClient.h>
using namespace util::protocol;
using namespace util::serialization;

#include <framework/logger/Logger.h>
#include <framework/configure/Config.h>
using namespace framework::network;
using namespace framework::logger;
using namespace framework::configure;

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio/read.hpp>
using namespace boost::system;

namespace tools
{
    namespace status
    {
      
        static Config & config()
        {
            static Config conf("ppbox_status.conf");
            return conf;
        }

        PeerWorkStatus::PeerWorkStatus()
        {
            global_logger().load_config(config());
            write_buf_ = &head_buf_[0];
            read_buf_ = &head_buf_[1];
            middle_buf_ = &head_buf_[2];
        }

        void PeerWorkStatus::set_address(
            std::string const & address_server)
        {
            address_server_ = address_server;
        }

        void PeerWorkStatus::status_transfer()
        {
            NetName addr(":1802");
            if (address_server_.empty())
                return;
            addr.from_string(address_server_);
            HttpClient client(io_svc_);
            HttpRequestHead head;
            head.path = "/status.dat";
            head.host.reset(addr.host_svc());
            head.connection.reset(http_field::Connection::keep_alive);
            while (!stopped_) {
                error_code ec;
                client.open(head, ec);
                if (!ec) {
                    size_t len = (size_t)client.response_head().content_length.get_value_or(0);
                    write_buf_->consume(write_buf_->size());
                    write_buf_->commit(boost::asio::read(
                        client, write_buf_->prepare(len), boost::asio::transfer_all(), ec));
                    assert(len == write_buf_->size());
                    client.read_finish(ec, write_buf_->size());
                    swap_buffer(&write_buf_, &middle_buf_);
                }
                client.close(ec);
                boost::this_thread::sleep(boost::posix_time::seconds(1));
            }
        }

        bool PeerWorkStatus::swap_buffer(
            boost::asio::streambuf **first, 
            boost::asio::streambuf **second)
        {
            boost::asio::streambuf *temp = NULL;
            if ((first == NULL) || (second == NULL))
                return false;
            mutex_.lock();
            temp = *first;
            *first = *second;
            *second = temp;
            mutex_.unlock();
            return true;
        }

        bool PeerWorkStatus::start()
        {
            thread_ = new boost::thread(boost::bind(&PeerWorkStatus::status_transfer, this));
            return true;
        }
 
        int PeerWorkStatus::get_info(
            std::map<std::string,Block> & blocks)
        {
            read_buf_->consume(read_buf_->size());
            if(!swap_buffer(&read_buf_ , &middle_buf_))
                return other_error;
            if(read_buf_->size() == 0)
                return no_update;
            std::istream os(read_buf_);
            util::archive::LittleEndianBinaryIArchive<> oa(os);
            if(!(oa >> blocks))
                return false;
            const char * p = boost::asio::buffer_cast<char const *>(read_buf_->data());
            for (std::map<std::string, Block>::iterator i = blocks.begin(); i != blocks.end(); ++i) {
                i->second.ptr = p;
                p += i->second.size;
            }
            return ok;
        }

        void PeerWorkStatus::terminate()
        {
            if(thread_) {
                stopped_ = true;
                thread_->join();
                delete thread_;
                thread_ = NULL;
            }
        }

        PeerWorkStatus::~PeerWorkStatus()
        {
        }
        
    }
}

#if 0
int main(int argc, char * argv[])
{
    std::string addr("192.168.1.109:1802");
    ppbox::status::PeerWorkStatus peer_work(addr);
	peer_work.start();
	boost::xtime xt;
	xt.sec = 20;
	while(1)
	{
	}
}
#endif
