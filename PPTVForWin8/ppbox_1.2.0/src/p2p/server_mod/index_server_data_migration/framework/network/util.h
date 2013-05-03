#pragma once

/**
* note: 程序内部 全部采用 本地字节序，所有 NET API 得到的 网络字节序 全部要转换 本地字节序 ntoh
*/

namespace framework
{
	namespace network
	{
		const u_int LOG_TYPE_NETWORK = 2001;

		// endpoint 和 SOCKADDR 的
		//boost::asio::ip::udp::endpoint SockAddrToUdpEndpoint(const SOCKADDR& pSockAddr);
		//boost::asio::ip::tcp::endpoint SockAddrToTcpEndpoint(const SOCKADDR& pSockAddr);
		//SOCKADDR EndpointToSockAddr(boost::asio::ip::udp::endpoint end_point);
		//SOCKADDR EndpointToSockAddr(boost::asio::ip::tcp::endpoint end_point);

		// endpoinit 和 ip,port 之间的转换
		boost::asio::ip::udp::endpoint IpPortToUdpEndpoint(u_long ip, u_short port);
		boost::asio::ip::tcp::endpoint IpPortToTcpEndpoint(u_long ip, u_short port);
		boost::asio::ip::udp::endpoint IpPortToUdpEndpoint(const string& ip, u_short port);
		boost::asio::ip::tcp::endpoint IpPortToTcpEndpoint(const string& ip, u_short port);
		bool EndpointToIpPort(boost::asio::ip::udp::endpoint end_point, u_long& ip, u_short& port);
		bool EndpointToIpPort(boost::asio::ip::tcp::endpoint end_point, u_long& ip, u_short& port);

        boost::asio::ip::udp::endpoint ParseUdpEndpoint(const std::string& host);

		// 获得本机的IP地址
		void LoadLocalIPs(vector<u_long>& ipArray);
	}
}