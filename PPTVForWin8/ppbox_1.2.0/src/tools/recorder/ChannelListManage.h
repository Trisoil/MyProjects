#ifndef _PPBOX_LIVE_RECORDER_CHANNEL_LIST_MAN__H_
#define _PPBOX_LIVE_RECORDER_CHANNEL_LIST_MAN__H_

#include <boost/asio/deadline_timer.hpp>

#include <boost/function.hpp>
#include <util/serialization/NVPair.h>

#include <map>

namespace util
{
    namespace protocol
    {
        class HttpClient;
    }
}

namespace boost
{
    class thread_group;
}

namespace server_mod
{
    namespace live
    {
        namespace live_recorder
        {

            class Channel;

            struct Conf_Param
            {
                Conf_Param()
                {
                    m3u_size_ = 5;
                    file_t_long_ = 10000;
                    root_dir_ = "./";
                    core_addr_ = "127.0.0.1:9001";
                }
                long m3u_size_;  //m3u中文件个数
                long file_t_long_;  //单位毫秒
                std::string root_dir_;  //路径
                std::string core_addr_;
            };
            
            using util::protocol::HttpClient;
            typedef void* ChannelHandle;
            using boost::thread_group;

            struct channel 
            {
                std::string id;
                std::string url;
                boost::uint16_t tcp_port;
                boost::uint16_t udp_port;
                ChannelHandle handle;

                channel() : handle(NULL) {}

                template <
                    typename Archive
                >
                void serialize(
                Archive & ar)
                {
                    std::string port;
                    ar  & SERIALIZATION_NVP(id)
                        & SERIALIZATION_NVP(url)
                        & SERIALIZATION_NVP(port);

                    int pos = port.find_first_of(':');
                    tcp_port = atoi(port.substr(0, pos).c_str());
                    udp_port = atoi(port.substr(pos + 1, port.length()).c_str());
                }
            };
            

            class ChannelListManage 
                : public util::daemon::ModuleBase<ChannelListManage>
            {
            public:
                ChannelListManage(util::daemon::Daemon & daemon);
                virtual ~ChannelListManage();
            public:
                virtual boost::system::error_code startup();
                virtual void shutdown();

                

            private:

                void handle_timer( boost::system::error_code const & ec );
                void on_fetch_channels( boost::system::error_code const & ec, boost::shared_ptr<HttpClient> snc_client_ptr );
            private:
                boost::asio::deadline_timer timer_;
                std::string snc_url_;

                std::string failed_channels_;
                std::map<std::string,Channel*> ChannelList_;

                Conf_Param config_;
            };

        }
    }
}
#endif

