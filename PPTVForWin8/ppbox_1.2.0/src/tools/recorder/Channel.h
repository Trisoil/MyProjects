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
                
                //��ȡƵ��
                std::string GetChannelId();
                //�˳��߳�
                void Exit();    
                
                //���ڲ��ұȽ�
                friend bool operator==(const Channel& str1,std::string psz2);
 
            private:
                boost::asio::io_service& io_serv;
                //Ƶ����Ϣ
                channel m_channel; 
                //Ƶ��״̬
                Channel_State m_state;

                //�ļ�����
                FileManager m_fileMan;

                boost::thread * th_;
 
                
                //ts�ļ��ĳ���
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
