/******************************************************************************
 *
 * Copyright (c) 2011 PPLive Inc.  All rights reserved
 * 
 * natcheck_press_simu.cpp
 * 
 * Description: 用于对natcheck进行压力测试，模拟客户端发包
 *             
 * 
 * --------------------
 * 2011-12-01, kelvinchen create
 * --------------------
 ******************************************************************************/

#include "Common.h"
//#include "winsock2.h"
#include <map>
#include <fstream>
#include "PPliveThreadPool.h"
#include "NatCheckClient.h"
#include "PPlivePubApi.h"
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

using namespace std;
using namespace boost;
using namespace framework::string;
//using namespace boost::interprocess;

//前一项是之前的rid出现的总次数，后一项是rid
//例如map的前两项是  2 -> rid1，20 -> rid2,那么rid1出现的次数就是2次，rid2出现的次数就是20-2=18次。类似的第三项是30 ->rid3，表示rid3出现的次数10次。
//map<unsigned,string> g_times_rid;
//set<string> g_peerid;

//这个map和g_times_rid map的含义差不多
map<double,uint8_t> g_percent_cmd;

//
//int ReadRidTimes(const string& ridfilename)
//{
//	/*要求文件格式为
//	4 000D8C3B54EAADA69D60988FC6A3C2E5
//	2 000D9C6E8F9F31A60D1DF494FDFF526A
//	1 000DBA93F3AE5E12DD5965355068D7B4
//	3 000DF9DC619616102D22AA83D35EAD31
//	1 000F0596DF758583E91A90DE9B1FD014
//	1 000F0EDD754DE3A85946E4BF3BF60C8F
//	1 000F60E291B3B622B1AE9E68E2C93B62
//	...
//	前面一个个数表示出现次数，后面一个是rid
//	*/
//	ifstream infile(ridfilename.c_str());
//	string line;
//	unsigned totaltime=0;
//	while(getline(infile,line))
//	{
//		unsigned times;
//		string rid;
//		istringstream stream(line);
//		stream>>times>>rid;
//		totaltime += times;
//		g_times_rid[totaltime] = rid;
//	}
//	return 0;
//}

//int ReadPeerids(const string& peeridfilename)
//{
//	ifstream infile(peeridfilename.c_str());
//	string line;
//	unsigned totaltime=0;
//	while(getline(infile,line))
//	{
//		string peerid;
//		istringstream stream(line);
//		stream>>peerid;
//		g_peerid.insert(peerid);
//	}
//	return 0;
//}

////获得一个随机的guid，这个算法的随机性在得到验证前不应该大范围使用，建议仅用于测试程序
//Uuid GetRandomUuid()
//{
//
//    static UUID randomuuid;
//    randomuuid.Data1 += rand();
//    randomuuid.Data2 += rand();
//    randomuuid.Data3 += rand();
//    for (int i=0;i<8;++i)
//        randomuuid.Data4[i] += rand();
//    Uuid ret;
//    ret.assign(randomuuid);
//    return ret;
//}


class PressSimuHandler:public PPliveThreadPool
{
public:
	PressSimuHandler(int pool_count,const map<double,uint8_t>& percent_cmd,int cmd_speed,int speed_interval,const boost::asio::ip::udp::endpoint& end_point)
		:PPliveThreadPool(pool_count),percent_cmd_(percent_cmd),cmd_speed_(cmd_speed),speed_interval_(speed_interval),endpoint_(end_point)
	{
		map<double,uint8_t>::const_iterator it = percent_cmd_.begin();
		while(it != percent_cmd_.end())
		{
			printf("cmd:%d,percent:%f\n",it->second,it->first);
			++it;
		}
	}
	~PressSimuHandler(){};
private:
	void DoIt();

	void SpeedLimit();
	//bool Init();
	void ProcessCmd(uint8_t cmd);

    //bool ProcessList();
    //bool ProcessListTcpIp();
    //bool ProcessListTcp();
    //bool ProcessListIp();

    bool ProcessSameRoute();
    bool ProcessDiffPort();
    bool ProcessDiffIp();

	////根据rid的出现次数随机返回一个rid
	//RID GetRandomRidByTimes();

	//Uuid GetRandomUuid();

	const map<double,uint8_t> percent_cmd_;
	map<uint8_t,unsigned> cmd_num_;
	//const map<unsigned,string> times_rid_;

	//下面两个参数用于限速，表示在speed_interval_秒的时间内，最多发送cmd_speed_个命令
	int cmd_speed_;
	double speed_interval_;
	boost::asio::io_service ios_; 
	boost::asio::ip::udp::endpoint endpoint_;
	boost::interprocess::interprocess_mutex peerinfo_mutex_;

    //map<int,int> list_counts_;

};

void PressSimuHandler::DoIt()
{
	while(true)
	{
		//随机一个命令字。处理。如果处理完后，限速未到，usleep一段。否则继续while
		
		static unsigned random_num = ns_pplive::PubApi::GenRandom();		
		random_num += ns_pplive::PubApi::GenRandom();
		double d_random_num = random_num % 10000 * 0.0001;
		map<double,uint8_t>::const_iterator it = percent_cmd_.lower_bound(d_random_num);
		//因为认为random_num是小于1.0的，而percent_cmd_的最后一个key是1.0
		assert(it != percent_cmd_.end());
		ProcessCmd(it->second);
		SpeedLimit();
	}
}

void PressSimuHandler::SpeedLimit()
{
	static double lastsleeptime = ns_pplive::PubApi::GetExactTimeNow();
	static int intervalprocessnum = 0;
	if(intervalprocessnum++ > cmd_speed_)
	{
		//数量达到了，再看时间是否超过了。
		double timenow = ns_pplive::PubApi::GetExactTimeNow();
		//if(0 == intervalprocessnum%101)
		{	
			boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
			printf("process time:%f,cmd num:%d\n",timenow - lastsleeptime,intervalprocessnum);
		
			//lock.lock();
			for(map<uint8_t,unsigned>::iterator it = cmd_num_.begin();it!= cmd_num_.end();++it)
			{
				printf("cmd type:%x,cmd num:%d\n",it->first,it->second);
			}	
            cmd_num_.clear();
            //for(map<int,int>::iterator it = list_counts_.begin();it != list_counts_.end();++it)
            //{
            //    printf("list ret:%d,times:%d\n",it->first,it->second);
            //}
            //list_counts_.clear();
			
			lock.unlock();
		}

		if( timenow - lastsleeptime > speed_interval_)
		{
#ifndef BOOST_WINDOWS_API
			usleep((speed_interval_-(timenow - lastsleeptime))*1000000);
#else
			Sleep(1);
#endif
			lastsleeptime = lastsleeptime+speed_interval_;
			intervalprocessnum = 0;
		}
	}

}

//RID PressSimuHandler::GetRandomRidByTimes()
//{
//	//随机得到要查询的rid
//	unsigned max_random_num = times_rid_.rbegin()->first;
//	unsigned random_num = ns_pplive::PubApi::GenRandom();
//	random_num %= max_random_num;
//	map<unsigned,string>::const_iterator it = times_rid_.lower_bound(random_num);	
//	assert(it != times_rid_.end());
//	return RID(it->second);
//}

bool PressSimuHandler::ProcessSameRoute()
{
    ns_pplive::ns_natcheck::NatCheckClient natcheck_client; 

    boost::asio::ip::udp::endpoint detect_ep;
    //boost::uint8_t error_code;

    if (0 != natcheck_client.SameRouteDetect(ios_,endpoint_,rand(),rand()%4,123,345,detect_ep,1.0))
    {
        //cout<<"process failed,msg:"<<g_client.GetErrMsg()<<endl;
        return false;
    }
    return true;
}

bool PressSimuHandler::ProcessDiffPort()
{
    ns_pplive::ns_natcheck::NatCheckClient natcheck_client; 
    boost::asio::ip::udp::endpoint detect_ep;
    //boost::uint8_t error_code;
    boost::uint16_t recv_nc_port;
    if (0 != natcheck_client.DiffPortDetect(ios_,endpoint_,rand(),rand()%4,123,345,detect_ep,recv_nc_port,1.0))
    {
        //cout<<"process failed,msg:"<<g_client.GetErrMsg()<<endl;
        return false;
    }
    return true;
}

bool PressSimuHandler::ProcessDiffIp()
{
    ns_pplive::ns_natcheck::NatCheckClient natcheck_client; 

    boost::asio::ip::udp::endpoint detect_ep;
    //boost::uint8_t error_code;
    boost::uint16_t recv_nc_port;
    boost::uint32_t recv_nc_ip;
    if (0 != natcheck_client.DiffIpDetect(ios_,endpoint_,rand(),rand()%4,123,345,detect_ep,recv_nc_ip,recv_nc_port,1.0))
    {
        //cout<<"process failed,msg:"<<g_client.GetErrMsg()<<endl;
        return false;
    }
    return true;
}



void PressSimuHandler::ProcessCmd(uint8_t cmd)
{
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
	//lock.lock();
	cmd_num_[cmd]++;
	lock.unlock();
	switch(cmd)
	{
        case protocol::NatCheckSameRoutePacket::Action:
            ProcessSameRoute();
            break;
        case protocol::NatCheckDiffPortPacket::Action:
            ProcessDiffPort();
            break;
        case protocol::NatCheckDiffIpPacket::Action:
            ProcessDiffIp();
            break;	
		default:
			break;
	}
}


int main(int argc,char* argv[])
{
	//读入配置文件
	std::string config_file_name = "D:\\work\\p2p\\build\\msvc-90\\server_mod\\tracker\\tracker_press_simu\\Debug\\trackerag_press_simu.config.txt";
	//std::string config_file_name = "tracker_press_simu.config.txt";
	if (argc >= 2)
	{
		config_file_name = argv[1];
		printf("Load config %s\n.", config_file_name.c_str());
	}
	if ( Config::Inst().LoadConfig(config_file_name) == false )
	{
		printf("Failed to Load Config file %s\n.", config_file_name.c_str());
		return -1;
	}

    double totalpercent = 0.0;   
    u_int sm_cmd_percent = Config::Inst().GetInteger("natcheck","natcheck.sameroute_cmd_percent",70);
    totalpercent +=  sm_cmd_percent * 0.01;
    g_percent_cmd[totalpercent] = protocol::NatCheckSameRoutePacket::Action;

    u_int dp_cmd_percent = Config::Inst().GetInteger("natcheck","natcheck.diffport_cmd_percent",20);
    totalpercent +=  dp_cmd_percent * 0.01;
    g_percent_cmd[totalpercent] = protocol::NatCheckDiffPortPacket::Action;

    u_int di_cmd_percent = Config::Inst().GetInteger("natcheck","natcheck.diffip_cmd_percent",10);
    totalpercent +=  di_cmd_percent * 0.01;
    g_percent_cmd[totalpercent] = protocol::NatCheckDiffIpPacket::Action;
   
    assert(sm_cmd_percent +  dp_cmd_percent + di_cmd_percent == 100);

    string natcheck_ip = Config::Inst().GetTString("natcheck","natcheck.ip","172.16.200.11");

	uint16_t natcheck_port = Config::Inst().GetInteger("natcheck","natcheck.port",13478);

	boost::asio::ip::address_v4 av4;
	av4 = boost::asio::ip::address_v4::from_string(natcheck_ip);
	boost::asio::ip::udp::endpoint end_point(boost::asio::ip::address(av4),natcheck_port);

	int thread_count = Config::Inst().GetInteger("natcheck","natcheck.thread_count",1424);
	int cmd_speed = Config::Inst().GetInteger("natcheck","natcheck.cmd_speed",10000);

	
	PressSimuHandler handler(thread_count,g_percent_cmd,cmd_speed,0.1,end_point);
	handler.StartThreads();
	handler.WaitThreadsTermination();

	return 0;
}