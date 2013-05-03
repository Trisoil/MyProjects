#include "stdafx.h"
#include "framework/network/uri.h"
#include "framework/log.h"

#define UTIL_DEBUG(s) LOG(__DEBUG, "util", s)
#define UTIL_INFO(s) LOG(__INFO, "util", s)
#define UTIL_EVENT(s) LOG(__EVENT, "util", s)
#define UTIL_WARN(s) LOG(__WARN, "util", s)
#define UTIL_ERROR(s) LOG(__ERROR, "util", s)

namespace framework
{
	namespace network
	{
		boost::asio::ip::udp::endpoint SockAddrToUdpEndpoint(const SOCKADDR& pSockAddr)
		{
			SOCKADDR_IN* pSockAddrIn = (SOCKADDR_IN*)(&pSockAddr);
			boost::asio::ip::address_v4::bytes_type b_t;
			b_t[0] = pSockAddrIn->sin_addr.S_un.S_un_b.s_b1;
			b_t[1] = pSockAddrIn->sin_addr.S_un.S_un_b.s_b2;
			b_t[2] = pSockAddrIn->sin_addr.S_un.S_un_b.s_b3;
			b_t[3] = pSockAddrIn->sin_addr.S_un.S_un_b.s_b4;
			boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4(b_t);
			u_short port = htons(pSockAddrIn->sin_port);
			boost::asio::ip::udp::endpoint endpoint(addr, port );
			//string target_string = addr.to_string();
			return endpoint;
		}

		boost::asio::ip::tcp::endpoint SockAddrToTcpEndpoint(const SOCKADDR& pSockAddr)
		{
			SOCKADDR_IN* pSockAddrIn = (SOCKADDR_IN*)(&pSockAddr);
			boost::asio::ip::address_v4::bytes_type b_t;
			b_t[0] = pSockAddrIn->sin_addr.S_un.S_un_b.s_b1;
			b_t[1] = pSockAddrIn->sin_addr.S_un.S_un_b.s_b2;
			b_t[2] = pSockAddrIn->sin_addr.S_un.S_un_b.s_b3;
			b_t[3] = pSockAddrIn->sin_addr.S_un.S_un_b.s_b4;
			boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4(b_t);
			u_short port = htons(pSockAddrIn->sin_port);
			boost::asio::ip::tcp::endpoint endpoint(addr, port );
			//string target_string = addr.to_string();
			return endpoint;
		}

		static SOCKADDR EndpointToSockAddr(boost::asio::ip::udp::endpoint end_point)
		{
			SOCKADDR_IN    addr;

			ZeroMemory( &addr, sizeof(addr) );

			addr.sin_family = AF_INET;
			addr.sin_port = htons(end_point.port());    
			addr.sin_addr.S_un.S_addr = inet_addr(end_point.address().to_string().c_str());

			return *(SOCKADDR*)(&addr);
		}

		static SOCKADDR EndpointToSockAddr(boost::asio::ip::tcp::endpoint end_point)
		{
			SOCKADDR_IN    addr;

			ZeroMemory( &addr, sizeof(addr) );

			addr.sin_family = AF_INET;
			addr.sin_port = htons(end_point.port());    
			addr.sin_addr.S_un.S_addr = inet_addr(end_point.address().to_string().c_str());

			return *(SOCKADDR*)(&addr);
		}


		boost::asio::ip::udp::endpoint IpPortToUdpEndpoint(u_long ip, u_short port)
		{
			boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4(ip);
			boost::asio::ip::udp::endpoint endpoint(addr, port);
			return endpoint;
		}
		boost::asio::ip::tcp::endpoint IpPortToTcpEndpoint(u_long ip, u_short port)
		{
			boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4(ip);
			boost::asio::ip::tcp::endpoint endpoint(addr, port);
			return endpoint;
		}
		boost::asio::ip::udp::endpoint IpPortToUdpEndpoint(const string& ip, u_short port)
		{
            boost::system::error_code error;
			boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4::from_string(ip, error);
			boost::asio::ip::udp::endpoint endpoint(addr, port);
			return endpoint;
		}
		boost::asio::ip::tcp::endpoint IpPortToTcpEndpoint(const string& ip, u_short port)
		{
            boost::system::error_code error;
			boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4::from_string(ip, error);
			boost::asio::ip::tcp::endpoint endpoint(addr, port);
			return endpoint;
		}
		bool EndpointToIpPort(boost::asio::ip::udp::endpoint end_point, u_long& ip, u_short& port)
		{
			ip = end_point.address().to_v4().to_ulong();
			port = end_point.port();
			return true;
		}
		bool EndpointToIpPort(boost::asio::ip::tcp::endpoint end_point, u_long& ip, u_short& port)
		{
			ip = end_point.address().to_v4().to_ulong();
			port = end_point.port();
			return true;
		}

		size_t LoadIPs(size_t maxCount, u_long ipArray[], const hostent& host)
		{
			size_t count = 0;
			for (int index = 0; host.h_addr_list[index] != NULL; index++)
			{
				if (count >= maxCount)
					break;
				const in_addr* addr = reinterpret_cast<const in_addr*>(host.h_addr_list[index]);
				ipArray[count++] = ntohl(addr->S_un.S_addr);		// 将 API读到的  网络 字节序 ----> 本地 字节序
			}
			for (size_t i = 0; i< count; i++)
			{
				for (size_t j = i; j< count; j++)
				{
					if (ipArray[i] < ipArray[j])
					{
						std::swap(ipArray[i], ipArray[j]);
					}
				}
			}
			return count;
		}

		size_t LoadLocalIPs(size_t maxCount, u_long ipArray[])
		{
			char hostName[256] = { 0 };	// 存储主机名
			if (gethostname(hostName, 255) != 0)		//正确的情况
			{
				UTIL_ERROR("CIPTable::LoadLocal: gethostname failed, ErrorCode=" << ::WSAGetLastError());
				return 0;
			}
			UTIL_ERROR("CIPTable::LoadLocal: gethostname=" << hostName);
			struct hostent* host = gethostbyname(hostName);
			if (host == NULL)
			{
				UTIL_ERROR("CIPTable::LoadLocal: gethostbyname(" << hostName << ") failed, ErrorCode=" << ::WSAGetLastError());
				return 0;
			}
			size_t count = LoadIPs(maxCount, ipArray, *host);
			if (count == 0)
			{
				UTIL_ERROR("CIPTable::LoadLocal: No hostent found.");
				return 0;
			}
			return count;
		}

		void LoadLocalIPs(vector<u_long>& ipArray)
		{
			const int max_count = 32;
			ipArray.clear();
			ipArray.resize(max_count);
			size_t count = LoadLocalIPs(max_count, &ipArray[0]);
			assert(count <= max_count);
			if (count < max_count)
			{
				ipArray.resize(count);
			}
		}

        boost::asio::ip::udp::endpoint ParseUdpEndpoint(const std::string& host)
        {
            std::vector<std::string> ep;
            boost::algorithm::split(ep, host, boost::is_any_of(":"));
            boost::asio::ip::udp::endpoint server;
            if (ep.size() <= 2)
            {
                boost::system::error_code error;
                boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4::from_string(ep[0], error);
                u_short port = (ep.size() == 1 ? 0 : boost::lexical_cast<u_short>(ep[1]) );

                server = boost::asio::ip::udp::endpoint(addr, port);
            }
            return server;
        }
	}
}
