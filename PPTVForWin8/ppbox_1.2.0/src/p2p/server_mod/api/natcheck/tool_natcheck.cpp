#include "NatCheckClient.h"
#include <iostream>
#include <sstream>
using namespace std;
boost::asio::io_service g_ios;
ns_pplive::ns_natcheck::NatCheckClient g_client;

using namespace ns_pplive::ns_natcheck;

int g_argc = 0;
char** g_argv;
int g_argindex = 1;
void readcin(string& value);
void readcin(int& value);
void readcin(double& value);
CheckState check_state = CheckState::ISNATCHECKING;
boost::uint16_t detect_port_;
boost::uint32_t detect_ip_;
bool is_nat_;
std::vector<uint32_t> local_ips_;
uint32_t local_first_ip_;
boost::uint16_t local_port_;
boost::uint32_t local_ip_;
string ip;
int port;
string ip2;
int port2;

void readcin(int& value)
{
	if(g_argindex < g_argc)
	{
		value = atoi(g_argv[g_argindex]);
		++g_argindex;
	}
	else
	{
		cin>>value;
	}
}

void readcin(double& value)
{
	if(g_argindex < g_argc)
	{
		value = atof(g_argv[g_argindex]);
		++g_argindex;
	}
	else
	{
		cin>>value;
	}
}

void readcin(string& value)
{    
	if(g_argindex < g_argc)
	{
		value = g_argv[g_argindex];
		++g_argindex;
	}
	else
	{
		cin>>value;
	}
}

int ProcessSameRoute()
{
	//string ip;
	//readcin(ip);
	//int port;
	//readcin(port);

	boost::asio::ip::address_v4 av4;
	av4 = boost::asio::ip::address_v4::from_string(ip);
	boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);

	boost::asio::ip::udp::endpoint detect_ep;
	//boost::uint8_t error_code;

	int i = 0;
	while (i < 3)
	{
		if (0 == g_client.SameRouteDetect(g_ios,endpoint_,rand(),rand(),local_ip_,local_port_,detect_ep,1.0))
		{
			//cout<<"process failed,msg:"<<g_client.GetErrMsg()<<endl;
			break;
		}
		i++;
	}

	if (i == 3)
	{
		std::cout<<"cann't udp communication!"<<std::endl;
		check_state = CheckState::END;
		return 0;
	}

	cout<<"detect ep:"<<detect_ep<<endl;  

	if (check_state == CheckState::ISNATCHECKING)
	{
		detect_port_ = detect_ep.port();
		detect_ip_ = detect_ep.address().to_v4().to_ulong();
		//detect_ip_ = atoi(ip_string);

		//如果服务器检测地址及端口与本地地址一致，则表明并没有进行NAT转换
		if (std::find(local_ips_.begin(), local_ips_.end(), detect_ip_) != local_ips_.end() && detect_port_ == local_port_)
		{
			is_nat_ = false;
			check_state = CheckState::END;
			std::cout<<"NAT TYPE: PUBLIC TYPE"<<std::endl;
		}
		else
		{
			is_nat_ = true;
			check_state = CheckState::FULLCORNCHECKING;
		}

	}
	else if (check_state == CheckState::SYMMETRICCHECKING)
	{
		boost::uint16_t detect_port_2 = detect_ep.port();
		boost::uint32_t detect_ip_2 = detect_ep.address().to_v4().to_ulong();
		//detect_ip_ = atoi(ip_string);

		if (detect_port_ != detect_port_2 || detect_ip_ != detect_ip_2)
		{
			// peer endpoint1 和 peer endpoint2 是不同的，返回nat类型为 TYPE_SYMNAT，检测结束
			std::cout<<"NAT TYPE: TYPE_SYMNAT"<<std::endl;
			check_state = CheckState::END;

		}
		else
		{
			//Restricted类型检测
			check_state = CheckState::RESTRICTEDCHECKING;

		}
	}
	else
	{
		assert(false);
	}
}

int ProcessDiffPort()
{
	//string ip;
	//readcin(ip);
	//int port;
	//readcin(port);

	boost::asio::ip::address_v4 av4;
	av4 = boost::asio::ip::address_v4::from_string(ip);
	boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);

	boost::asio::ip::udp::endpoint detect_ep;
	//boost::uint8_t error_code;

	boost::uint16_t send_nc_port = 0;

	int i = 0;
	while (i < 3)
	{
		if (0 == g_client.DiffPortDetect(g_ios,endpoint_,rand(),rand(),local_ip_,local_port_,detect_ep,send_nc_port,1.0))
		{
			//cout<<"process failed,msg:"<<g_client.GetErrMsg()<<endl;
			std::cout<<"NAT TYPE: IP_RESTRICTEDNAT"<<std::endl;
			check_state = CheckState::END;
			return 0;
		}
		i++;
	}

	std::cout<<"NAT TYPE: IP_PORT_RESTRICTEDNAT"<<std::endl;
	check_state = CheckState::END;
	return 0;
	//cout<<"detect ep:"<<detect_ep<<" send nc port:"<<send_nc_port<<endl;   
}

int ProcessDiffIp()
{
	//string ip;
	//readcin(ip);
	//int port;
	//readcin(port);

	boost::asio::ip::address_v4 av4;
	av4 = boost::asio::ip::address_v4::from_string(ip);
	boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);

	boost::asio::ip::udp::endpoint detect_ep;
	//boost::uint8_t error_code;

	boost::uint16_t send_nc_port = 0;
	boost::uint32_t send_nc_ip = 0;

	int i = 0;
	while (i < 3)
	{
		if (0 == g_client.DiffIpDetect(g_ios,endpoint_,rand(),rand(),local_ip_,local_port_,detect_ep,send_nc_ip,send_nc_port,1.0))
		{
			//cout<<"process failed,msg:"<<g_client.GetErrMsg()<<endl;
			std::cout<<"NAT TYPE: FULLCONENAT"<<std::endl;
			check_state = CheckState::END;
			return 0;
		}
		i++;
	}

	check_state = CheckState::SYMMETRICCHECKING;
	return 0;

}

/*
#if 0
int main(int argc,char** argv)
{
	//cout<<"hello,usage:"<<endl;
	//cout<<"0(list) rid ip port pid"<<endl;

	g_argc = argc;
	g_argv = argv;

	//cout << "please input command: ";
	int cmd=-1;
	//cin>>cmd;
	readcin(cmd);
REQURY:
	switch(cmd)
	{
	case 0:
		ProcessSameRoute();
		break;  
	case 1:
		ProcessDiffPort();
		break;
	case 2:
		ProcessDiffIp();
		break;

	default:
		cout<<"error input cmd"<<endl;
		return 0;
	}

	//再一个输入，防止太快退出
	string tmp;
	//cin>>tmp;
	readcin(tmp);
	if(atoi(tmp.c_str()) != 0 || tmp == "0")
	{
		cmd = atoi(tmp.c_str());
		goto REQURY;
	}

	return 0;
}
#endif
*/


int main(int argc,char** argv)
{

	g_argc = argc;
	g_argv = argv;
    readcin(ip);
	readcin(port);
	readcin(ip2);
	readcin(port2);

	local_port_ = 1234;
	g_client.SetLocalPort(1234);
	g_client.LoadLocalIPs(local_ips_);
	local_ip_ = local_ips_[0];

	while (check_state != CheckState::END)
	{
		switch(check_state)
		{
		case CheckState::IDLE:
			break;
		case CheckState::FULLCORNCHECKING:
			ProcessDiffIp();
			break;
		case CheckState::SYMMETRICCHECKING:                    //检验是否是symmetric的时候，发给不同的ip和port
			ip = ip2;
			port = port2;
		case CheckState::ISNATCHECKING:
			ProcessSameRoute();
			break;
		case CheckState::RESTRICTEDCHECKING:
			ProcessDiffPort();
			break;
		}
	}


	return 0;
}