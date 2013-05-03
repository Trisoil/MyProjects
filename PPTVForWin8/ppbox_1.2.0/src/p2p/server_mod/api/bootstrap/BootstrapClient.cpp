//-------------------------------------------------------------
//     Copyright (c) 2011 PPLive Inc.  All rights reserved.
//-------------------------------------------------------------

#include <boost/asio/basic_datagram_socket.hpp>
#include "BootstrapClient.h"
#include <util/buffers/BufferCopy.h>

namespace ns_pplive
{
	namespace ns_bootstrap
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

		BootstrapClient::BootstrapClient()
		{
		};
		BootstrapClient::~BootstrapClient()
		{
		};

		int BootstrapClient::QueryTrackerList(std::vector<TRACKER_INFO>& tracker_info,boost::uint32_t transaction_id,const Guid& peer_guid, 
			                                  boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
		{
			err_msg_.str("");
			UdpBuffer send_buffer;
			int ret = pack_.PackQueryTrackerList(transaction_id,peer_guid,endpoint_,send_buffer);
			if (ret != 0)
			{
				err_msg_<<"failed in packquerytrackerlist:"<<pack_.GetErrMsg();
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
			ret = pack_.UnPackQueryTrackerList(recv_buffer,transaction_id,error_code,tracker_info);
			if (ret != 0)
			{
				err_msg_<<"failed in UnPackList:"<<pack_.GetErrMsg();
				return ret;				
			}

			return error_code;
		};

		int BootstrapClient::QueryTrackerForListing(std::vector<TRACKER_INFO>& tracker_info,boost::uint32_t transaction_id,const Guid& peer_guid, 
			boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
		{
			err_msg_.str("");
			UdpBuffer send_buffer;
			int ret = pack_.PackQueryTrackerForListing(transaction_id,peer_guid,endpoint_,send_buffer);
			if (ret != 0)
			{
				err_msg_<<"failed in packquerytrackerlist:"<<pack_.GetErrMsg();
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
			ret = pack_.UnPackQueryTrackerForListing(recv_buffer,transaction_id,error_code,tracker_info);
			if (ret != 0)
			{
				err_msg_<<"failed in UnPackList:"<<pack_.GetErrMsg();
				return ret;				
			}

			return error_code;
		};

		int BootstrapClient::QueryStunServerList(std::vector<STUN_SERVER_INFO>& stunserver_info,boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
		{
			err_msg_.str("");
			UdpBuffer send_buffer;
			int ret = pack_.PackQueryStunServerList(transaction_id, endpoint_, send_buffer);
			if (ret != 0)
			{
				err_msg_<<"failed in packquerystunserverlist:"<<pack_.GetErrMsg();
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
			ret = pack_.UnPackQueryStunServerList(recv_buffer, transaction_id, error_code, stunserver_info);
			return error_code;

		};

		int BootstrapClient::QueryIndexServerList(std::vector<INDEX_SERVER_INFO>& index_info,boost::uint32_t transaction_id,const Guid& peer_guid, 
			boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
		{
			err_msg_.str("");
			UdpBuffer send_buffer;
			int ret = pack_.PackQueryIndexServerList(transaction_id,peer_guid,endpoint_,send_buffer);
			if (ret != 0)
			{
				err_msg_<<"failed in packqueryindexserverlist:"<<pack_.GetErrMsg();
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
			ret = pack_.UnPackQueryIndexServerList(recv_buffer,transaction_id,error_code,index_info);
			if (ret != 0)
			{
				err_msg_<<"failed in UnPackList:"<<pack_.GetErrMsg();
				return ret;				
			}

			return error_code;
		};

		int BootstrapClient::QueryNotifyList(std::vector< NOTIFY_SERVER_INFO>& notify_info,boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
		{
			err_msg_.str("");
			UdpBuffer send_buffer;
			int ret = pack_.PackQueryNotifyList(transaction_id, endpoint_, send_buffer);
			if (ret != 0)
			{
				err_msg_<<"failed in packquerynotifylist:"<<pack_.GetErrMsg();
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
			ret = pack_.UnPackQueryNotifyList(recv_buffer,transaction_id,error_code,notify_info);
			if (ret != 0)
			{
				err_msg_<<"failed in UnPackList:"<<pack_.GetErrMsg();
				return ret;				
			}

			return error_code;
		};

		int BootstrapClient::QueryLiveTrackerList(std::vector< TRACKER_INFO>& livetracker_info,boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
		{
			err_msg_.str("");
			UdpBuffer send_buffer;
			int ret = pack_.PackQueyLiveTrackerList(transaction_id, endpoint_, send_buffer);
			if (ret != 0)
			{
				err_msg_<<"failed in packquerylivetrackerlist:"<<pack_.GetErrMsg();
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
			//ret = pack_.UnPackQueryNotifyList(recv_buffer,transaction_id,error_code,notify_info);
			ret = pack_.UnPackQueyLiveTrackerList(recv_buffer, transaction_id, error_code, livetracker_info);
			if (ret != 0)
			{
				err_msg_<<"failed in UnPackList:"<<pack_.GetErrMsg();
				return ret;				
			}

			return error_code;
		};

		int BootstrapClient::QueryConfigString(std::string& config_string,boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
		{
			err_msg_.str("");
			UdpBuffer send_buffer;
			int ret = pack_.PackQueryConfigString(transaction_id, endpoint_, send_buffer);
			if (ret != 0)
			{
				err_msg_<<"failed in packconfigstringlist:"<<pack_.GetErrMsg();
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
			ret = pack_.UnPackQueryConfigString(recv_buffer, transaction_id, error_code, config_string);
			if (ret != 0)
			{
				err_msg_<<"failed in UnPackList:"<<pack_.GetErrMsg();
				return ret;				
			}

			return error_code;
		};

		string BootstrapClient::GetErrMsg()
		{
			return err_msg_.str();
		}

		void BootstrapClient::SetTraceLog(bool trace)
		{
		};

	};
};
