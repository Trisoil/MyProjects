//-------------------------------------------------------------
//     Copyright (c) 2012 PPLive Inc.  All rights reserved.
//-------------------------------------------------------------

#include "PushClient.h"
#include "PushPack.h"

unsigned int ns_pplive::ns_push::PushClient::recv_count_ = 0;
unsigned int ns_pplive::ns_push::PushClient::send_count_ = 0;

namespace ns_pplive
{
    namespace ns_push
    {
        int DoNetWork(const UdpBuffer& send_buffer,UdpBuffer& recv_buffer,boost::asio::io_service& ios,std::ostringstream& errmsg,double timeout=0.1)
        {
            // send
            boost::system::error_code ec;           

            boost::asio::ip::udp::socket sock(ios,boost::asio::ip::udp::v4());

            sock.send_to(send_buffer.data(),send_buffer.end_point(), 0, ec);

            //cout << "send to done" << endl;
            if (ec.value() != 0)
            {
                errmsg<<"send_to failed,ec msg:"<<ec.message();
                return ec.value();
            }

            if (timeout == 0.0) {
                //if timeout set to 0, do not expect reply.
                return 1;
            }

            //SOCKET native_sock = sock.native();
            unsigned native_sock = sock.native();               

            int result = 0; 
#ifndef BOOST_WINDOWS_API
            // 设为非阻塞 
            struct pollfd recv_client[1];
            int nfd = 1;
            memset(&recv_client[0],0,sizeof(pollfd));
            recv_client[0].fd = native_sock;
            recv_client[0].events = POLLIN ;
            if (::poll(recv_client, nfd, (int)(timeout*1000))<=0)
            {
                errmsg<<"receive timeout,time:"<<timeout<<" dst:"<<send_buffer.end_point();
                sock.close();          
                return -1; 
            }
#else
            int _timeout = timeout*1000; //s->ms
            result = setsockopt(native_sock, SOL_SOCKET,SO_RCVTIMEO,(char*)&_timeout,sizeof(_timeout)); 
            if (0 != result)
            {
                errmsg<<"set sockopt failed,result:"<<result;
                return result;
            }
#endif      
            //recv
            size_t bytetransfer = sock.receive_from(recv_buffer.prepare(),recv_buffer.end_point(),0,ec);
            if (ec.value() != 0)
            {
                errmsg<<"receive from failed,ec msg:"<<ec.message()<<" dst:"<<send_buffer.end_point();
                sock.close();
                return ec.value();
            } 
#ifndef BOOST_WINDOWS_API
            // 设回阻塞     
            //save_mode = fcntl( native_sock, F_GETFL, 0 );         
            //save_mode &= (~O_NONBLOCK);
            //fcntl( native_sock, F_SETFL, save_mode );
#endif
            recv_buffer.commit(bytetransfer);       

            return 0;
        }

        PushClient::PushClient( void )
        {
        }

        PushClient::~PushClient( void )
        {
        }

        int PushClient::QueryPushTask(int pversion, boost::asio::io_service& ios, boost::asio::ip::udp::endpoint& push_server_endpoint, 
            std::vector<PlayHistoryItem>& play_history_vec, boost::uint32_t transaction_id, const Guid& peer_guid, 
            boost::uint32_t avg_upload_speed_kbs, boost::uint32_t used_disk_size, 
            boost::uint32_t upload_bandwidth_kbs, boost::uint32_t total_disk_size, 
            boost::uint32_t nat_type, boost::uint32_t online_percent,
            std::vector<PushTaskItem>& push_task_vec,
            boost::uint8_t& error_code, double timeout)
        {
            err_msg_.str("");
            UdpBuffer send_buffer;
            int ret;
            ret = pack_.PackQueryPushTask(pversion, transaction_id, peer_guid, push_server_endpoint, avg_upload_speed_kbs, 
                    used_disk_size, upload_bandwidth_kbs, total_disk_size, nat_type, online_percent, send_buffer, play_history_vec);
            if (ret != 0)
            {
                err_msg_<<"failed in packlist:"<<pack_.GetErrMsg();
                return ret;
            }
            //cout << "pkt assemble done." << endl;

            UdpBuffer recv_buffer;

            send_buffer.end_point(push_server_endpoint);            
            ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
            //cout << "network io done" << endl;
            if (ret)
            {
                return ret;
            }

            ret = pack_.UnPackQueryPushTask(pversion, recv_buffer, transaction_id, error_code, push_task_vec);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackList:"<<pack_.GetErrMsg();
                return ret;             
            }

            return 0;
        }

        std::string PushClient::GetErrMsg()
        {
            return err_msg_.str();
        }
    }
}