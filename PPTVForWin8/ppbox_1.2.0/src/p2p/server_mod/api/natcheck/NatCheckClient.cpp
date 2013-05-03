//-------------------------------------------------------------
//     Copyright (c) 2012 PPLive Inc.  All rights reserved.
//-------------------------------------------------------------

#include "NatCheckPack.h"
#include "NatCheckClient.h"

using  boost::asio::ip::udp;

namespace ns_pplive
{
    namespace ns_natcheck
    {
		int DoNetWork(const UdpBuffer& send_buffer,UdpBuffer& recv_buffer,boost::asio::io_service& ios,boost::uint16_t local_port_, std::ostringstream& errmsg,double timeout=0.1)
        {
            // send
            boost::system::error_code ec;           

            //boost::asio::ip::udp::socket sock(ios,boost::asio::ip::udp::v4());
			boost::asio::ip::udp::socket sock(ios, udp::endpoint(boost::asio::ip::udp::v4(), local_port_));  
     
			

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

        NatCheckClient::NatCheckClient( void )
        {
        }

        NatCheckClient::~NatCheckClient( void )
        {
        }

        int NatCheckClient::SameRouteDetect(boost::asio::io_service& ios, boost::asio::ip::udp::endpoint& nat_check_endpoint,
            boost::uint32_t transaction_id,boost::uint16_t query_times,boost::uint32_t local_ip,
            boost::uint16_t local_port,boost::asio::ip::udp::endpoint& detect_endpoint,double timeout)
        {
            err_msg_.str("");
            UdpBuffer send_buffer;
            int ret = pack_.PackSamRoute(transaction_id,query_times,local_ip,local_port,PEER_VERSION,nat_check_endpoint,send_buffer);
            if (ret != 0)
            {
                err_msg_<<"failed in packlist:"<<pack_.GetErrMsg();
                return ret;
            }
            //cout << "pkt assemble done." << endl;

            UdpBuffer recv_buffer;

            send_buffer.end_point(nat_check_endpoint);            
            ret = DoNetWork(send_buffer,recv_buffer,ios,local_port_, err_msg_,timeout);
            //cout << "network io done" << endl;
            if (ret)
            {
                return ret;
            }

            boost::uint8_t error_code;
            ret = pack_.UnPackSamRoute(recv_buffer,transaction_id,error_code,query_times,detect_endpoint);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackList:"<<pack_.GetErrMsg();
                return ret;             
            }
            if(error_code != 0 )
            {
                return error_code;
            }

            return 0;
        }

        int NatCheckClient::DiffIpDetect(boost::asio::io_service& ios, boost::asio::ip::udp::endpoint& nat_check_endpoint,
            boost::uint32_t transaction_id,boost::uint16_t query_times,boost::uint32_t local_ip,
            boost::uint16_t local_port,boost::asio::ip::udp::endpoint& detect_endpoint,boost::uint32_t& send_nc_ip,boost::uint16_t& send_nc_port, double timeout)
        {
            err_msg_.str("");
            UdpBuffer send_buffer;
            int ret = pack_.PackDiffIp(transaction_id,query_times,local_ip,local_port,PEER_VERSION,nat_check_endpoint,send_buffer);
            if (ret != 0)
            {
                err_msg_<<"failed in packlist:"<<pack_.GetErrMsg();
                return ret;
            }
            //cout << "pkt assemble done." << endl;

            UdpBuffer recv_buffer;

            send_buffer.end_point(nat_check_endpoint);            
            ret = DoNetWork(send_buffer,recv_buffer,ios,local_port_, err_msg_,timeout);
            //cout << "network io done" << endl;
            if (ret)
            {
                return ret;
            }

            boost::uint8_t error_code;
            ret = pack_.UnPackDiffIp(recv_buffer,transaction_id,error_code,query_times,detect_endpoint,send_nc_ip,send_nc_port);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackList:"<<pack_.GetErrMsg();
                return ret;             
            }
            if(error_code != 0 )
            {
                return error_code;
            }

            return 0;
        }

        int NatCheckClient::DiffPortDetect(boost::asio::io_service& ios, boost::asio::ip::udp::endpoint& nat_check_endpoint,
            boost::uint32_t transaction_id,boost::uint16_t query_times,boost::uint32_t local_ip,
            boost::uint16_t local_port,boost::asio::ip::udp::endpoint& detect_endpoint,boost::uint16_t& send_nc_port, double timeout)
        {
            err_msg_.str("");
            UdpBuffer send_buffer;
            int ret = pack_.PackDiffPort(transaction_id,query_times,local_ip,local_port,PEER_VERSION,nat_check_endpoint,send_buffer);
            if (ret != 0)
            {
                err_msg_<<"failed in packlist:"<<pack_.GetErrMsg();
                return ret;
            }
            //cout << "pkt assemble done." << endl;

            UdpBuffer recv_buffer;

            send_buffer.end_point(nat_check_endpoint);            
            ret = DoNetWork(send_buffer,recv_buffer,ios,local_port_, err_msg_,timeout);
            //cout << "network io done" << endl;
            if (ret)
            {
                return ret;
            }

            boost::uint8_t error_code;
            ret = pack_.UnPackDiffPort(recv_buffer,transaction_id,error_code,query_times,detect_endpoint,send_nc_port);
            if (ret != 0)
            {
                err_msg_<<"failed in UnPackList:"<<pack_.GetErrMsg();
                return ret;             
            }
            if(error_code != 0 )
            {
                return error_code;
            }

            return 0;
        }


        std::string NatCheckClient::GetErrMsg()
        {
            return err_msg_.str();
        }
		uint32_t NatCheckClient::LoadIPs(uint32_t maxCount, uint32_t ipArray[], const hostent& host)
		{
			uint32_t count = 0;
			for (int index = 0; host.h_addr_list[index] != NULL; index++)
			{
				if (count >= maxCount)
					break;
				const in_addr* addr = reinterpret_cast<const in_addr*> (host.h_addr_list[index]);
				uint32_t v =
#ifdef BOOST_WINDOWS_API
					addr->S_un.S_addr;
#else
					addr->s_addr;
#endif
				ipArray[count++] = ntohl(v);
			}
			std::sort(ipArray, ipArray + count, std::greater<uint32_t>());
			return count;
		}

		uint32_t NatCheckClient::LoadLocalIPs(uint32_t maxCount, uint32_t ipArray[])
		{
			char hostName[256] = { 0 };
			if (gethostname(hostName, 255) != 0)
			{
				uint32_t error =
#ifdef BOOST_WINDOWS_API
					::WSAGetLastError();
#else
					0;
#endif
				(void)error;
			//	LOG4CPLUS_ERROR_LOG(logger_stun, "CIPTable::LoadLocal: gethostname failed, ErrorCode=" << error);
				return 0;
			}

			//LOG4CPLUS_ERROR_LOG(logger_stun, "CIPTable::LoadLocal: gethostname=" << hostName);
			struct hostent* host = gethostbyname(hostName);
			if (host == NULL)
			{
				uint32_t error =
#ifdef BOOST_WINDOWS_API
					::WSAGetLastError();
#else
					0;
#endif
				(void)error;
				//LOG4CPLUS_ERROR_LOG(logger_stun, "CIPTable::LoadLocal: gethostbyname(" << hostName 
				//	<< ") failed, ErrorCode=" << error);
				return 0;
			}
			uint32_t count = LoadIPs(maxCount, ipArray, *host);
			if (count == 0)
			{
				//LOG4CPLUS_ERROR_LOG(logger_stun, "CIPTable::LoadLocal: No hostent found.");
			}
			return count;
		}

		void NatCheckClient::LoadLocalIPs(std::vector<uint32_t>& ipArray)
		{
#ifdef BOOST_WINDOWS_API
			const uint32_t max_count = 32;
			ipArray.clear();
			ipArray.resize(max_count);
			uint32_t count = LoadLocalIPs(max_count, &ipArray[0]);
			assert(count <= max_count);
			if (count < max_count)
			{
				ipArray.resize(count);
			}
#else

			// PPBox获取本地IP的实现
			std::vector<framework::network::Interface> infs;
			enum_interface(infs);
			for (size_t i = 0; i < infs.size(); ++i) {
				framework::network::Interface const & inf = infs[i];

				if (inf.flags & framework::network::Interface::loopback)
					continue;
				if (!(inf.flags & framework::network::Interface::up))
					continue;

				if (inf.addr.is_v4())
				{
					ipArray.push_back(inf.addr.to_v4().to_ulong());
				}
			}
#endif
		}
    }
}