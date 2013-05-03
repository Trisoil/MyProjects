#ifndef _RECORDER_CHANNEL_H_
#define _RECORDER_CHANNEL_H_

#include "tools/recorder/ChannelListManage.h"

#include "tools/recorder/FileManager.h"

namespace server_mod
{
    namespace live
    {           
        namespace live_recorder
        {

            enum Channel_State
            {
                cs_init =0,
                cs_running,
                cs_pause,
                cs_threadexit,
                cs_exit,

            };

            class Channel
            {
            public:
                Channel(boost::asio::io_service& ,channel&,Conf_Param&);
                virtual ~Channel();  
                
                //获取频道
                std::string GetChannelId();
                //退出线程
                void Exit();    
                
                //用于查找比较
                friend bool operator==(const Channel& str1,std::string psz2);
 
            private:
                boost::asio::io_service& io_serv;
                //频道信息
                channel m_channel; 
                //频道状态
                Channel_State m_state;

                //文件管理
                FileManager m_fileMan;

                boost::thread * th_;
 
                
                //ts文件的长度
                long m_iTsTimers;

                std::string core_addr_;

                boost::asio::io_service::work work_;

            private:
                void close();
                void StartChannel();
                void handle_exit();

                void SetThreadState(Channel_State state);
                Channel_State GetChannelState();
            };

        }
    }
}
#endif
