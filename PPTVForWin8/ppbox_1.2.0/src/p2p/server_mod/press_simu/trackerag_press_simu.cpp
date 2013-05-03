/******************************************************************************
 *
 * Copyright (c) 2011 PPLive Inc.  All rights reserved
 * 
 * trackerag_press_simu.cpp
 * 
 * Description: 用于对trackerag进行压力测试，模拟客户端发包
 *             
 * 
 * --------------------
 * 2011-12-01, kelvinchen create
 * --------------------
 ******************************************************************************/

#include "Common.h"
#include <map>
#include <fstream>
#include "PPliveThreadPool.h"
#include "TrackerClient.h"
#include "PPlivePubApi.h"
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

using namespace std;
using namespace boost;
using namespace framework::string;
//using namespace boost::interprocess;

//前一项是之前的rid出现的总次数，后一项是rid
//例如map的前两项是  2 -> rid1，20 -> rid2,那么rid1出现的次数就是2次，rid2出现的次数就是20-2=18次。类似的第三项是30 ->rid3，表示rid3出现的次数10次。
map<unsigned,string> g_times_rid;
set<string> g_peerid;

//这个map和g_times_rid map的含义差不多
map<double,uint8_t> g_percent_cmd;


int ReadRidTimes(const string& ridfilename)
{
	/*要求文件格式为
	4 000D8C3B54EAADA69D60988FC6A3C2E5
	2 000D9C6E8F9F31A60D1DF494FDFF526A
	1 000DBA93F3AE5E12DD5965355068D7B4
	3 000DF9DC619616102D22AA83D35EAD31
	1 000F0596DF758583E91A90DE9B1FD014
	1 000F0EDD754DE3A85946E4BF3BF60C8F
	1 000F60E291B3B622B1AE9E68E2C93B62
	...
	前面一个个数表示出现次数，后面一个是rid
	*/
	ifstream infile(ridfilename.c_str());
	string line;
	unsigned totaltime=0;
	while(getline(infile,line))
	{
		unsigned times;
		string rid;
		istringstream stream(line);
		stream>>times>>rid;
		totaltime += times;
		g_times_rid[totaltime] = rid;
	}
	return 0;
}

int ReadPeerids(const string& peeridfilename)
{
	ifstream infile(peeridfilename.c_str());
	string line;
	unsigned totaltime=0;
	while(getline(infile,line))
	{
		string peerid;
		istringstream stream(line);
		stream>>peerid;
		g_peerid.insert(peerid);
	}
	return 0;
}

//获得一个随机的guid，这个算法的随机性在得到验证前不应该大范围使用，建议仅用于测试程序
Uuid GetRandomUuid()
{

    static UUID randomuuid;
    randomuuid.Data1 += rand();
    randomuuid.Data2 += rand();
    randomuuid.Data3 += rand();
    for (int i=0;i<8;++i)
        randomuuid.Data4[i] += rand();
    Uuid ret;
    ret.assign(randomuuid);
    return ret;
}


class PressSimuHandler:public PPliveThreadPool
{
public:
	PressSimuHandler(int pool_count,const map<double,uint8_t>& percent_cmd,const map<unsigned,string>& times_rid,int cmd_speed,int speed_interval,const boost::asio::ip::udp::endpoint& end_point)
		:PPliveThreadPool(pool_count),percent_cmd_(percent_cmd),times_rid_(times_rid),cmd_speed_(cmd_speed),speed_interval_(speed_interval),endpoint_(end_point)
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

    bool ProcessList();
    bool ProcessListTcpIp();
    bool ProcessListTcp();
    bool ProcessListIp();

	//根据rid的出现次数随机返回一个rid
	RID GetRandomRidByTimes();

	//Uuid GetRandomUuid();

	const map<double,uint8_t> percent_cmd_;
	map<uint8_t,unsigned> cmd_num_;
	const map<unsigned,string> times_rid_;

	//下面两个参数用于限速，表示在speed_interval_秒的时间内，最多发送cmd_speed_个命令
	int cmd_speed_;
	double speed_interval_;
	boost::asio::io_service ios_; 
	boost::asio::ip::udp::endpoint endpoint_;
	boost::interprocess::interprocess_mutex peerinfo_mutex_;

    map<int,int> list_counts_;

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
            for(map<int,int>::iterator it = list_counts_.begin();it != list_counts_.end();++it)
            {
                printf("list ret:%d,times:%d\n",it->first,it->second);
            }
            list_counts_.clear();
			
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

RID PressSimuHandler::GetRandomRidByTimes()
{
	//随机得到要查询的rid
	unsigned max_random_num = times_rid_.rbegin()->first;
	unsigned random_num = ns_pplive::PubApi::GenRandom();
	random_num %= max_random_num;
	map<unsigned,string>::const_iterator it = times_rid_.lower_bound(random_num);	
	assert(it != times_rid_.end());
	return RID(it->second);
}

bool PressSimuHandler::ProcessList()
{
	//查询
	vector<CandidatePeerInfo> candidate_peer_info;
	string tmppeerid;	
	ns_pplive::ns_tracker::TrackerClient tracker_client;

    //搞个rid冒充peerid，免得peerid重复了，导致频率超限。
	if(0 != tracker_client.ListPeer(candidate_peer_info,ns_pplive::PubApi::GenRandom(),GetRandomRidByTimes(),GetRandomUuid(),50,endpoint_,ios_,0.1))
	{
		//printf("%s,%d,%s,%s\n",__FILE__,__LINE__,__FUNCTION__,tracker_client.GetErrMsg());
         ++list_counts_[-1];
        return false;
	}	
    return true;
}

bool PressSimuHandler::ProcessListTcp()
{
    vector<CandidatePeerInfo> candidate_peer_info;
    string tmppeerid;	
    ns_pplive::ns_tracker::TrackerClient tracker_client;
    if(0 != tracker_client.ListPeerTcp(candidate_peer_info,ns_pplive::PubApi::GenRandom(),GetRandomRidByTimes(),GetRandomUuid(),50,endpoint_,ios_,0.1))
    { 
        ++list_counts_[-1];
        return false;
    }
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
    list_counts_[candidate_peer_info.size()]++;
    lock.unlock();
    return true;
}

bool PressSimuHandler::ProcessListIp()
{
    vector<CandidatePeerInfo> candidate_peer_info;
    string tmppeerid;	
    ns_pplive::ns_tracker::TrackerClient tracker_client;
    if(0 != tracker_client.ListPeer(candidate_peer_info,ns_pplive::PubApi::GenRandom(),GetRandomRidByTimes(),GetRandomUuid(),50,endpoint_,ios_,0.1))
    {  
         ++list_counts_[-1];
        return false;
    }
    list_counts_[candidate_peer_info.size()]++;
    return true;
}

bool PressSimuHandler::ProcessListTcpIp()
{
    vector<CandidatePeerInfo> candidate_peer_info;
    string tmppeerid;	
    ns_pplive::ns_tracker::TrackerClient tracker_client;
    if(0 != tracker_client.ListPeerTcpWithIp(candidate_peer_info,ns_pplive::PubApi::GenRandom(),GetRandomRidByTimes(),GetRandomUuid(),50,rand(),endpoint_,ios_,0.1))
    {  
         ++list_counts_[-1];
        return false;
    }
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
    list_counts_[candidate_peer_info.size()]++;
    lock.unlock();
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
        case protocol::ListPacket::Action:
            ProcessList();
            break;
        case protocol::ListTcpPacket::Action:
            ProcessListTcp();
            break;
        case protocol::ListTcpWithIpPacket::Action:
            ProcessListTcpIp();
            break;
        case protocol::ListWithIpPacket::Action:
            ProcessListIp();
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
    u_int list_cmd_percent = Config::Inst().GetInteger("trackerag.list_cmd_percent",90);
    totalpercent +=  list_cmd_percent * 0.01;
    g_percent_cmd[totalpercent] = protocol::ListPacket::Action;

    u_int listtcp_cmd_percent = Config::Inst().GetInteger("trackerag.listtcp_cmd_percent",1);
    totalpercent +=  listtcp_cmd_percent * 0.01;
    g_percent_cmd[totalpercent] = protocol::ListTcpPacket::Action;

    u_int listtcpip_cmd_percent = Config::Inst().GetInteger("trackerag.listtcpip_cmd_percent",8);
    totalpercent +=  listtcpip_cmd_percent * 0.01;
    g_percent_cmd[totalpercent] = protocol::ListTcpWithIpPacket::Action;

    u_int listtip_cmd_percent = Config::Inst().GetInteger("trackerag.listip_cmd_percent",1);
    totalpercent +=  listtip_cmd_percent * 0.01;
    g_percent_cmd[totalpercent] = protocol::ListWithIpPacket::Action;

    assert(list_cmd_percent +  listtcp_cmd_percent + listtcpip_cmd_percent + listtip_cmd_percent == 100);


	//将所有的rid和出现次数读入
	std::string ridtimes_file_name = "D:\\work\\p2p\\build\\msvc-90\\server_mod\\tracker\\tracker_press_simu\\Debug\\ridtimesfile.txt";
	if (argc >= 3)
	{
		config_file_name = argv[2];
		printf("Load ridtimesfile %s\n.", ridtimes_file_name.c_str());
	}
	ReadRidTimes(ridtimes_file_name);

	//string trackerag_ip = Config::Inst().GetTString("trackerag.ip","172.16.200.11");
    //string trackerag_ip = Config::Inst().GetTString("trackerag.ip","113.108.219.37");
    string trackerag_ip = Config::Inst().GetTString("trackerag.ip","118.123.212.12");

	uint16_t trackerag_port = Config::Inst().GetInteger("trackerag.port",17900);

	boost::asio::ip::address_v4 av4;
	av4 = boost::asio::ip::address_v4::from_string(trackerag_ip);
	boost::asio::ip::udp::endpoint end_point(boost::asio::ip::address(av4),trackerag_port);

	int thread_count = Config::Inst().GetInteger("trackerag.thread_count",200);
	int cmd_speed = Config::Inst().GetInteger("trackerag.cmd_speed",1000);

	
	PressSimuHandler handler(thread_count,g_percent_cmd,g_times_rid,cmd_speed,0.1,end_point);
	handler.StartThreads();
	handler.WaitThreadsTermination();

	return 0;
}