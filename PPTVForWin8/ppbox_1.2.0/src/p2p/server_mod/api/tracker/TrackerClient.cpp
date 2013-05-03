//-------------------------------------------------------------
//     Copyright (c) 2011 PPLive Inc.  All rights reserved.
//-------------------------------------------------------------

#include <boost/asio/basic_datagram_socket.hpp>
#include "TrackerClient.h"
//#include <util/buffers/SubBuffers.h>
#include <util/buffers/BufferCopy.h>
//#include <util/archive/LittleEndianBinaryOArchive.h>

namespace ns_pplive
{
    namespace ns_tracker
    {
        int DoNetWork(const UdpBuffer& send_buffer,UdpBuffer& recv_buffer,boost::asio::io_service& ios,ostringstream& errmsg,double timeout=0.1)
        {
            // send
            boost::system::error_code ec;			

            boost::asio::ip::udp::socket sock(ios,boost::asio::ip::udp::v4());			

            sock.send_to(send_buffer.data(),send_buffer.end_point(), 0, ec);

            if (ec.value() != 0)
            {
                errmsg<<"send_to failed,ec msg:"<<ec.message();
                return ec.value();
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

		int UnpackDoNetWork(const UdpBuffer& send_buffer, void* buffer,int buflen, boost::asio::io_service& ios,ostringstream& errmsg,double timeout=0.1)
		{
			// send
			boost::system::error_code ec;			

			boost::asio::ip::udp::socket sock(ios,boost::asio::ip::udp::v4());			

			sock.send_to(send_buffer.data(),send_buffer.end_point(), 0, ec);

			if (ec.value() != 0)
			{
				errmsg<<"send_to failed,ec msg:"<<ec.message();
				return ec.value();
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

//#ifndef BOOST_WINDOWS_API
			// 设回阻塞      
			//save_mode = fcntl( native_sock, F_GETFL, 0 );			
			//save_mode &= (~O_NONBLOCK);
			//fcntl( native_sock, F_SETFL, save_mode );
			//struct sockaddr FAR *from, int FAR *fromlen 
			struct sockaddr  *from;
#ifndef BOOST_WINDOWS_API
			//socklen_t  *fromlen;
			socklen_t  fromlen = sizeof(sockaddr);
			size_t bytetransfer = recvfrom(native_sock, buffer, buflen, 0, from, &fromlen);
#else
			int  fromlen = sizeof(sockaddr);
            size_t bytetransfer = recvfrom(native_sock, (char*)buffer, buflen, 0, from, &fromlen);
#endif
			return bytetransfer;
//#else

	

	//		return 0;
		}

        TrackerClient::TrackerClient()
        {
        };
        TrackerClient::~TrackerClient()
        {
        };


        int TrackerClient::ListPeer(std::vector<CandidatePeerInfo>& candidate_peer_info,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
            boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
        {		
            err_msg_.str("");
            UdpBuffer send_buffer;
            int ret = pack_.PackList(transaction_id, resource_id, peer_guid, request_peer_count,endpoint_,send_buffer);
            if (ret != 0)
            {
                err_msg_<<"failed in packlist:"<<pack_.GetErrMsg();
                return ret;
            }

            UdpBuffer recv_buffer;

            send_buffer.end_point(endpoint_);			
            ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
            if (ret)
            {
                return ret;
            }

            boost::uint8_t error_code = 0 ;
            ret = pack_.UnPackList(recv_buffer,transaction_id,error_code,candidate_peer_info);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackList:"<<pack_.GetErrMsg();
                return ret;				
            }

            return error_code;
        };

        int TrackerClient::ListPeerTcp(std::vector<CandidatePeerInfo>& candidate_peer_info,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
            boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
        {		
            err_msg_.str("");

            UdpBuffer send_buffer;
            int ret = pack_.PackListTcp(transaction_id, resource_id, peer_guid, request_peer_count,endpoint_,send_buffer);
            if (ret != 0)
            {
                err_msg_<<"failed in packlist tcp:"<<pack_.GetErrMsg();
                return ret;
            }

            UdpBuffer recv_buffer;

            send_buffer.end_point(endpoint_);			
            ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
            if (ret)
            {
                return ret;
            }

            boost::uint8_t error_code = 0 ;
            ret = pack_.UnPackListTcp(recv_buffer,transaction_id,error_code,candidate_peer_info);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackList tcp:"<<pack_.GetErrMsg();
                return ret;				
            }

            return error_code;
        };

        int TrackerClient::ListPeerTcpWithIp(std::vector<CandidatePeerInfo>& candidate_peer_info,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
            unsigned request_ip,boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
        {
            err_msg_.str("");

            UdpBuffer send_buffer;
            int ret = pack_.PackListTcpWithIp(transaction_id, resource_id, peer_guid, request_peer_count,request_ip,endpoint_,send_buffer);
            if (ret != 0)
            {
                err_msg_<<"failed in packlist tcp with ip:"<<pack_.GetErrMsg();
                return ret;
            }

            UdpBuffer recv_buffer;

            send_buffer.end_point(endpoint_);			
            ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
            if (ret)
            {
                return ret;
            }

            boost::uint8_t error_code = 0 ;
            ret = pack_.UnPackListTcpWithIp(recv_buffer,transaction_id,error_code,candidate_peer_info);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackList tcp with ip:"<<pack_.GetErrMsg();
                return ret;				
            }

            return error_code;
        }
        
        int TrackerClient::ListPeerWithIp(std::vector<CandidatePeerInfo>& candidate_peer_info,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
            unsigned request_ip,boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
        {
            err_msg_.str("");

            UdpBuffer send_buffer;
            int ret = pack_.PackListWithIp(transaction_id, resource_id, peer_guid, request_peer_count,request_ip,endpoint_,send_buffer);
            if (ret != 0)
            {
                err_msg_<<"failed in packlist with ip:"<<pack_.GetErrMsg();
                return ret;
            }

            UdpBuffer recv_buffer;

            send_buffer.end_point(endpoint_);			
            ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
            if (ret)
            {
                return ret;
            }

            boost::uint8_t error_code = 0 ;
            ret = pack_.UnPackListWithIp(recv_buffer,transaction_id,error_code,candidate_peer_info);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackList with ip:"<<pack_.GetErrMsg();
                return ret;				
            }

            return error_code;
        }

		int TrackerClient::ListPeerWithIp(std::string& candidate_peer_string,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
			unsigned request_ip,boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
		{
			err_msg_.str("");

			UdpBuffer send_buffer;
			int ret = pack_.PackListWithIp(transaction_id, resource_id, peer_guid, request_peer_count,request_ip,endpoint_,send_buffer);
			if (ret != 0)
			{
				err_msg_<<"failed in packlist with ip:"<<pack_.GetErrMsg();
				return ret;
			}

			UdpBuffer recv_buffer;
			char buffer[2050] = {0};

			int bufflen = sizeof(buffer);
			send_buffer.end_point(endpoint_);			
			ret = UnpackDoNetWork(send_buffer,buffer,bufflen,ios,err_msg_,timeout);
			if (ret <= 0)
			{
				return ret;
			}
            
			candidate_peer_string.assign(buffer,ret);


			//ret = pack_.UnPackListWithIp(recv_buffer,transaction_id,error_code,candidate_peer_info);
			//if (ret != 0)
			//{
			//	err_msg_<<"failed in UnPackList with ip:"<<pack_.GetErrMsg();
			//	return ret;				
			//}

			return 0;
		}

        int TrackerClient::ReportPeer(boost::uint16_t& keepalive_interval,boost::uint32_t& detected_ip,boost::uint16_t& detected_udp_port,boost::uint16_t& resource_count,
            boost::uint32_t transaction_id,const Guid& peer_guid,boost::uint16_t local_resource_count,	boost::uint16_t server_resource_count,
            boost::uint16_t udp_port,boost::uint32_t stun_peer_ip,boost::uint16_t stun_peer_udpport,boost::uint32_t stun_detected_ip,boost::uint16_t stun_detected_udp_port,const std::vector<boost::uint32_t>& real_ips,
            const std::vector<REPORT_RESOURCE_STRUCT>& resource_ids,boost::uint8_t nat_type,boost::uint8_t upload_priority,boost::uint8_t idle_time_in_mins,
            boost::int32_t upload_bandwidth_kbs,boost::int32_t upload_limit_kbs,boost::int32_t upload_speed_kbs,boost::asio::ip::udp::endpoint endpoint_,boost::asio::io_service& ios, double timeout)
        {
            err_msg_.str("");

            UdpBuffer send_buffer;
            int ret = pack_.PackReoprt(transaction_id,peer_guid,local_resource_count,server_resource_count,udp_port,stun_peer_ip,stun_peer_udpport,
                stun_detected_ip, stun_detected_udp_port,real_ips,resource_ids,nat_type,upload_priority,idle_time_in_mins,upload_bandwidth_kbs,upload_limit_kbs,upload_speed_kbs,endpoint_,send_buffer);
            if (ret != 0)
            {			
                err_msg_<<"failed in ReportPeer:"<<pack_.GetErrMsg();
                return ret;
            }

            UdpBuffer recv_buffer;

            send_buffer.end_point(endpoint_);
            ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
            if (ret)
            {
                return ret;
            }

            boost::uint8_t error_code = 0 ;
            ret = pack_.UnPackReport(recv_buffer,transaction_id,error_code,keepalive_interval,detected_ip,detected_udp_port,resource_count);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackReport:"<<pack_.GetErrMsg();
                return ret;				
            }

            return error_code;
        };

        int TrackerClient::Leave(boost::uint32_t transaction_id,const Guid& peer_guid,const boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
        {
            err_msg_.str("");

            UdpBuffer send_buffer;
            int ret = pack_.PackLeave(transaction_id,peer_guid,endpoint_,send_buffer);
            if (ret != 0)
            {			
                err_msg_<<"failed in PackLeave:"<<pack_.GetErrMsg();
                return ret;
            }

            send_buffer.end_point(endpoint_);

            boost::system::error_code ec;
            boost::asio::ip::udp::socket sock(ios,boost::asio::ip::udp::v4());			
            sock.send_to(send_buffer.data(),send_buffer.end_point(), 0, ec);
            if (ec.value() != 0)
            {
                err_msg_<<"send_to failed,ec msg:"<<ec.message();
                return ec.value();
            }
            return 0;
        }

        int TrackerClient::QueryPeerCount(boost::uint32_t& peer_count,boost::uint32_t transaction_id,const RID& resource_id,const boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
        {
            err_msg_.str("");

            UdpBuffer send_buffer;
            int ret = pack_.PackQueryPeerCount(transaction_id,resource_id,endpoint_,send_buffer);
            if (ret != 0)
            {			
                err_msg_<<"failed in PackQueryPeerCount:"<<pack_.GetErrMsg();
                return ret;
            }

            UdpBuffer recv_buffer;

            send_buffer.end_point(endpoint_);
            ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
            if (ret)
            {
                return ret;
            }

            boost::uint8_t error_code = 0 ;
            ret = pack_.UnPackQueryPeerCount(recv_buffer,transaction_id,error_code,peer_count);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackQueryPeerCount:"<<pack_.GetErrMsg();
                return ret;				
            }

            return error_code;
        }

        int TrackerClient::QueryPeerResources(std::vector<RID>& resources,boost::uint32_t transaction_id,const Guid& peer_id,
            const boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
        {
            err_msg_.str("");

            UdpBuffer send_buffer;
            int ret = pack_.PackQueryPeerResources(transaction_id,peer_id,endpoint_,send_buffer);
            if (ret != 0)
            {			
                err_msg_<<"failed in PackQueryPeerResources:"<<pack_.GetErrMsg();
                return ret;
            }

            UdpBuffer recv_buffer;

            send_buffer.end_point(endpoint_);
            ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
            if (ret)
            {
                return ret;
            }

            boost::uint8_t error_code = 0 ;
            ret = pack_.UnPackQueryPeerResources(recv_buffer,transaction_id,error_code,resources);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackQueryPeerResources:"<<pack_.GetErrMsg();
                return ret;				
            }

            return error_code;
        }

        int TrackerClient::QueryTrackerStatistic(std::string& tracker_statistic,boost::uint32_t transaction_id,const boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
        {
            err_msg_.str("");

            UdpBuffer send_buffer;
            int ret = pack_.PackQueryTrackerStatistic(transaction_id,endpoint_,send_buffer);
            if (ret != 0)
            {			
                err_msg_<<"failed in PackQueryTrackerStatistic:"<<pack_.GetErrMsg();
                return ret;
            }

            UdpBuffer recv_buffer;

            send_buffer.end_point(endpoint_);
            ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
            if (ret)
            {
                return ret;
            }

            boost::uint8_t error_code = 0 ;
            ret = pack_.UnPackQueryTrackerStatistic(recv_buffer,transaction_id,error_code,tracker_statistic);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackQueryTrackerStatistic:"<<pack_.GetErrMsg();
                return ret;				
            }

            return error_code;
        }

        int TrackerClient::InternalCommand(boost::uint32_t transaction_id, 
            const boost::asio::ip::udp::endpoint& endpoint_,
            boost::uint16_t cmd, boost::uint32_t arg,
            boost::asio::io_service& ios, double timeout)
        {
            err_msg_.str("");

            UdpBuffer send_buffer;
            int ret = pack_.PackInternalCommand(transaction_id, endpoint_, send_buffer, cmd, arg);
            if (ret != 0)
            {			
                err_msg_<<"failed in PackInternalCommand:"<<pack_.GetErrMsg();
                return ret;
            }

            UdpBuffer recv_buffer;

            send_buffer.end_point(endpoint_);
            ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
            if (ret)
            {
                err_msg_ << "Send Message Error!";
                return ret;
            }

            boost::uint8_t error_code = 0 ;
            ret = pack_.UnPackInternalCommand(recv_buffer, transaction_id,error_code);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackInternalCommand:"<<pack_.GetErrMsg();
                return ret;				
            }

            return error_code;
        }

        string TrackerClient::GetErrMsg()
        {
            return err_msg_.str();
        };

        void TrackerClient::SetTraceLog(bool trace)
        {
        };

    };

};
