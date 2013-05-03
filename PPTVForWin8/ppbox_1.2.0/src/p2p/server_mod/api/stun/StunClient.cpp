#include <boost/asio/basic_datagram_socket.hpp>
#include "StunClient.h"
#include <util/buffers/BufferCopy.h>


namespace ns_pplive
{
	namespace ns_stun
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

		StunClient::StunClient()
		{

		};
		StunClient::~StunClient()
		{

		};
		int StunClient::StunHandShake(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint endpoint_, boost::asio::io_service& ios, double timeout)
		{
			err_msg_.str("");
			UdpBuffer send_buffer;
			int ret = pack_.PackStunHandShake(transaction_id, endpoint_, send_buffer);
			if (ret != 0)
			{
				err_msg_<<"failed in PackStunHandShake:"<<pack_.GetErrMsg();
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
			ret = pack_.UnPackStunHandShake(recv_buffer, transaction_id, error_code);
			if (ret != 0)
			{
				err_msg_<<"failed in UnPackStunHandShake:"<<pack_.GetErrMsg();
				return ret;				
			}

			return error_code;

		};

		int StunClient::StunKPL(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint endpoint_, boost::asio::io_service& ios, double timeout)
		{
			err_msg_.str("");
			UdpBuffer send_buffer;
			int ret = pack_.PackStunKPL(transaction_id, endpoint_, send_buffer);
			if (ret != 0)
			{
				err_msg_<<"failed in PackStunHandShake:"<<pack_.GetErrMsg();
				return ret;
			}

			UdpBuffer recv_buffer;

			send_buffer.end_point(endpoint_);			
			ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
			return 0;
		}
        //int StunInvoke(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint endpoint_, boost::asio::io_service& ios, double timeout);
		int StunClient::StunInvoke(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint& endpoint_, boost::asio::io_service& ios, double timeout)
		{
			err_msg_.str("");
			UdpBuffer send_buffer;
			int ret = pack_.PackStunInvoke(transaction_id, endpoint_, send_buffer);
			if (ret != 0)
			{
				err_msg_<<"failed in PackStunInvoke:"<<pack_.GetErrMsg();
				return ret;
			}

			UdpBuffer recv_buffer;

			send_buffer.end_point(endpoint_);			
			ret = DoNetWork(send_buffer,recv_buffer,ios,err_msg_,timeout);
			return 0;
		}

		string StunClient::GetErrMsg()
		{
			return err_msg_.str();
		};

		void StunClient::SetTraceLog(bool trace)
		{
		};


	};

};