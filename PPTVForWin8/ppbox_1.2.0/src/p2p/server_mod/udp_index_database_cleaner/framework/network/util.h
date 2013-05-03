#pragma once

/**
* note: �����ڲ� ȫ������ �����ֽ������� NET API �õ��� �����ֽ��� ȫ��Ҫת�� �����ֽ��� ntoh
*/

namespace framework
{
	namespace network
	{
		const u_int LOG_TYPE_NETWORK = 2001;

		// endpoint �� SOCKADDR ��
		//boost::asio::ip::udp::endpoint SockAddrToUdpEndpoint(const SOCKADDR& pSockAddr);
		//boost::asio::ip::tcp::endpoint SockAddrToTcpEndpoint(const SOCKADDR& pSockAddr);
		//SOCKADDR EndpointToSockAddr(boost::asio::ip::udp::endpoint end_point);
		//SOCKADDR EndpointToSockAddr(boost::asio::ip::tcp::endpoint end_point);

		// endpoinit �� ip,port ֮���ת��
		boost::asio::ip::udp::endpoint IpPortToUdpEndpoint(u_long ip, u_short port);
		boost::asio::ip::tcp::endpoint IpPortToTcpEndpoint(u_long ip, u_short port);
		boost::asio::ip::udp::endpoint IpPortToUdpEndpoint(const string& ip, u_short port);
		boost::asio::ip::tcp::endpoint IpPortToTcpEndpoint(const string& ip, u_short port);
		bool EndpointToIpPort(boost::asio::ip::udp::endpoint end_point, u_long& ip, u_short& port);
		bool EndpointToIpPort(boost::asio::ip::tcp::endpoint end_point, u_long& ip, u_short& port);

        boost::asio::ip::udp::endpoint ParseUdpEndpoint(const std::string& host);

		// ��ñ�����IP��ַ
		void LoadLocalIPs(vector<u_long>& ipArray);
	}
}