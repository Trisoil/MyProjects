/******************************************************************************
 *
 * Copyright (c) 2011 PPLive Inc.  All rights reserved
 * 
 * tracker_press_simu.cpp
 * 
 * Description: ���ڶ�tracker����ѹ�����ԣ�ģ��tracker�Ŀͻ��˷���
 *             
 * 
 * --------------------
 * 2011-09-22, kelvinchen create
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

//ǰһ����֮ǰ��rid���ֵ��ܴ�������һ����rid
//����map��ǰ������  2 -> rid1��20 -> rid2,��ôrid1���ֵĴ�������2�Σ�rid2���ֵĴ�������20-2=18�Ρ����Ƶĵ�������30 ->rid3����ʾrid3���ֵĴ���20�Ρ�
//�������ӳ���Ŀ����
map<unsigned,string> g_times_rid;
set<string> g_peerid;

//���ﶨ���cmd��ʵ�ʷ��͵�cmdû�й�ϵ��
enum CMDTPYE
{
	PING_REPORT_CMD,
	ALL_REPORT_CMD,
	DEL_REPORT_CMD,
	ADD_REPORT_CMD,

	EMPTY_LIST_CMD,
	COMM_LIST_CMD,

	EMPTY_LEAVE_CMD,
	COMM_LEAVE_CMD,

};
//���map��g_times_rid map�ĺ�����
map<double,CMDTPYE> g_percent_cmd;


int ReadRidTimes(const string& ridfilename)
{
	/*Ҫ���ļ���ʽΪ
	4 000D8C3B54EAADA69D60988FC6A3C2E5
	2 000D9C6E8F9F31A60D1DF494FDFF526A
	1 000DBA93F3AE5E12DD5965355068D7B4
	3 000DF9DC619616102D22AA83D35EAD31
	1 000F0596DF758583E91A90DE9B1FD014
	1 000F0EDD754DE3A85946E4BF3BF60C8F
	1 000F60E291B3B622B1AE9E68E2C93B62
	...
	ǰ��һ��������ʾ���ִ���������һ����rid
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


class PressSimuHandler:public PPliveThreadPool
{
public:
	PressSimuHandler(int pool_count,const map<double,CMDTPYE>& percent_cmd,const map<unsigned,string>& times_rid,int cmd_speed,int speed_interval,const boost::asio::ip::udp::endpoint& end_point,const set<string> peerid)
		:PPliveThreadPool(pool_count),percent_cmd_(percent_cmd),times_rid_(times_rid),cmd_speed_(cmd_speed),speed_interval_(speed_interval),endpoint_(end_point),peerids_(peerid)
	{
		map<double,CMDTPYE>::const_iterator it = percent_cmd_.begin();
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
	void ProcessCmd(CMDTPYE cmd);
	/*
	PING_REPORT_CMD,
	ALL_REPORT_CMD,
	DEL_REPORT_CMD,
	ADD_REPORT_CMD,

	EMPTY_LIST_CMD,
	COMM_LIST_CMD,

	EMPTY_LEAVE_CMD,
	COMM_LEAVE_CMD,
	*/
	bool ProcessPingReport();
	bool ProcessAllReport();
	bool ProcessDelReport();
	bool ProcessAddReport();
	bool ProcessEmptyList();
	bool ProcessCommList();
	bool ProcessEmptyLeave();
	bool ProcessCommLeave();
	//����rid�ĳ��ִ����������һ��rid
	RID GetRandomRidByTimes();

	Uuid GetRandomUuid();

	const map<double,CMDTPYE> percent_cmd_;
	map<CMDTPYE,unsigned> cmd_num_;
	const map<unsigned,string> times_rid_;

	//�������������������٣���ʾ��speed_interval_���ʱ���ڣ���෢��cmd_speed_������
	int cmd_speed_;
	double speed_interval_;
	//ns_pplive::ns_tracker::TrackerClient tracker_client_;
	boost::asio::io_service ios_; 
	boost::asio::ip::udp::endpoint endpoint_;
	vector<CandidatePeerInfo> candidate_peer_info_;
	boost::interprocess::interprocess_mutex peerinfo_mutex_;
	//��peerid��������
	set<string> peerids_;
	//��¼set���ϴδ����
	//string		last_process_peerid;
};

void PressSimuHandler::DoIt()
{
	while(true)
	{
		//���һ�������֡�������������������δ����usleepһ�Ρ��������while
		
		static unsigned random_num = ns_pplive::PubApi::GenRandom();		
		random_num += ns_pplive::PubApi::GenRandom();
		double d_random_num = random_num % 10000 * 0.0001;
		map<double,CMDTPYE>::const_iterator it = percent_cmd_.lower_bound(d_random_num);
		//��Ϊ��Ϊrandom_num��С��1.0�ģ���percent_cmd_�����һ��key��1.0
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
		//�����ﵽ�ˣ��ٿ�ʱ���Ƿ񳬹��ˡ�
		double timenow = ns_pplive::PubApi::GetExactTimeNow();
		//if(0 == intervalprocessnum%101)
		{	
			boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
			printf("process time:%f,cmd num:%d,pid count:%d\n",timenow - lastsleeptime,intervalprocessnum,peerids_.size());
		
			//lock.lock();
			for(map<CMDTPYE,unsigned>::iterator it = cmd_num_.begin();it!= cmd_num_.end();++it)
			{
				printf("cmd type:%d,cmd num:%d\n",it->first,it->second);
			}			
			cmd_num_.clear();
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
	//����õ�Ҫ��ѯ��rid
	unsigned max_random_num = times_rid_.rbegin()->first;
	unsigned random_num = ns_pplive::PubApi::GenRandom();
	random_num %= max_random_num;
	map<unsigned,string>::const_iterator it = times_rid_.lower_bound(random_num);	
	assert(it != times_rid_.end());
	return RID(it->second);
}

Uuid PressSimuHandler::GetRandomUuid()
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

bool PressSimuHandler::ProcessPingReport()
{
	unsigned random_num = ns_pplive::PubApi::GenRandom();
	//����ط���Ҫ���������Ǹĳɶ�д����

	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
	//lock.lock();
	if (candidate_peer_info_.size() == 0)
	{
		//û��list�����ݹ����������������ˡ�
		return false;
	}
	CandidatePeerInfo peer_info = candidate_peer_info_[random_num % candidate_peer_info_.size()];	
	string peerid;
	set<string>::iterator its = peerids_.upper_bound(GetRandomUuid().to_string());
	if(its != peerids_.end())
	{
		peerid = *its;
	}
	else
	{
		peerid = *(peerids_.begin());
	}


	//last_process_peerid = peerid;
	lock.unlock();

	//��Ҫ����Ĳ���
	std::vector<boost::uint32_t> real_ips;
	real_ips.push_back(peer_info.DetectIP);
	std::vector<REPORT_RESOURCE_STRUCT> resource_ids;
	//���������صĽ��
	boost::uint16_t keepalive_interval;
	boost::uint32_t detected_ip;
	boost::uint16_t detected_udp_port;
	boost::uint16_t resource_count;
	//���ﻹ�кܶ�Ĳ�����Ҫ���ŵġ�
	ns_pplive::ns_tracker::TrackerClient tracker_client;
	if (0 != tracker_client.ReportPeer(keepalive_interval,detected_ip,detected_udp_port,resource_count,ns_pplive::PubApi::GenRandom(),Guid(peerid),0,0,
		peer_info.UdpPort,peer_info.StunIP,peer_info.StunUdpPort,peer_info.DetectUdpPort,real_ips,resource_ids,
		peer_info.PeerNatType,peer_info.UploadPriority,ns_pplive::PubApi::GenRandom()%60,ns_pplive::PubApi::GenRandom()%500+10,ns_pplive::PubApi::GenRandom()%1000,
		ns_pplive::PubApi::GenRandom()%100,endpoint_,ios_,0.2))
	{
		//printf("%s,%d,%s,%s\n",__FILE__,__LINE__,__FUNCTION__,tracker_client.GetErrMsg());
	}


	return true;
}
bool PressSimuHandler::ProcessAddReport()
{
	unsigned random_num = ns_pplive::PubApi::GenRandom();
	//����ط���Ҫ���������Ǹĳɶ�д����

	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
	//lock.lock();
	if (candidate_peer_info_.size() == 0)
	{
		//û��list�����ݹ����������������ˡ�
		return false;
	}
	CandidatePeerInfo peer_info = candidate_peer_info_[random_num % candidate_peer_info_.size()];	
	string peerid;
	set<string>::iterator its = peerids_.upper_bound(GetRandomUuid().to_string());
	if(its != peerids_.end())
	{
		peerid = *its;
	}
	else
	{
		peerid = *(peerids_.begin());
	}


	//last_process_peerid = peerid;
	lock.unlock();

	//��Ҫ����Ĳ���
	std::vector<boost::uint32_t> real_ips;
	real_ips.push_back(peer_info.DetectIP);
	std::vector<REPORT_RESOURCE_STRUCT> resource_ids;
	REPORT_RESOURCE_STRUCT rps;
	//typeΪ1��ʾ�����Դ
	rps.Type = 1;
	rps.ResourceID = GetRandomRidByTimes();
	resource_ids.push_back(rps);
	//���������صĽ��
	boost::uint16_t keepalive_interval;
	boost::uint32_t detected_ip;
	boost::uint16_t detected_udp_port;
	//resource_count����ֶ�tracker��ʱû��ʹ��
	boost::uint16_t resource_count;
	//���ﻹ�кܶ�Ĳ�����Ҫ���ŵġ�
	ns_pplive::ns_tracker::TrackerClient tracker_client;
	if (0 != tracker_client.ReportPeer(keepalive_interval,detected_ip,detected_udp_port,resource_count,ns_pplive::PubApi::GenRandom(),Guid(peerid),0,0,
		peer_info.UdpPort,peer_info.StunIP,peer_info.StunUdpPort,peer_info.DetectUdpPort,real_ips,resource_ids,
		peer_info.PeerNatType,peer_info.UploadPriority,ns_pplive::PubApi::GenRandom()%60,ns_pplive::PubApi::GenRandom()%500+10,ns_pplive::PubApi::GenRandom()%1000,
		ns_pplive::PubApi::GenRandom()%100,endpoint_,ios_,0.2))
	{
		//printf("%s,%d,%s,%s\n",__FILE__,__LINE__,__FUNCTION__,tracker_client.GetErrMsg());
	}


	return true;
}

//del���Ǻܺ�ģ�⣬����del������Ӧ�ò��ࡣ��
bool PressSimuHandler::ProcessDelReport()
{
	unsigned random_num = ns_pplive::PubApi::GenRandom();
	//����ط���Ҫ���������Ǹĳɶ�д����

	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
	//lock.lock();
	if (candidate_peer_info_.size() == 0)
	{
		//û��list�����ݹ����������������ˡ�
		return false;
	}
	CandidatePeerInfo peer_info = candidate_peer_info_[random_num % candidate_peer_info_.size()];	
	string peerid;
	set<string>::iterator its = peerids_.upper_bound(GetRandomUuid().to_string());
	if(its != peerids_.end())
	{
		peerid = *its;
	}
	else
	{
		peerid = *(peerids_.begin());
	}


	//last_process_peerid = peerid;
	lock.unlock();

	//��Ҫ����Ĳ���
	std::vector<boost::uint32_t> real_ips;
	real_ips.push_back(peer_info.DetectIP);
	std::vector<REPORT_RESOURCE_STRUCT> resource_ids;
	REPORT_RESOURCE_STRUCT rps;
	//typeΪ1��ʾ�����Դ
	rps.Type = 0;
	rps.ResourceID = GetRandomRidByTimes();
	resource_ids.push_back(rps);
	//���������صĽ��
	boost::uint16_t keepalive_interval;
	boost::uint32_t detected_ip;
	boost::uint16_t detected_udp_port;
	//resource_count����ֶ�tracker��ʱû��ʹ��
	boost::uint16_t resource_count;
	//���ﻹ�кܶ�Ĳ�����Ҫ���ŵġ�
	ns_pplive::ns_tracker::TrackerClient tracker_client;
	if (0 != tracker_client.ReportPeer(keepalive_interval,detected_ip,detected_udp_port,resource_count,ns_pplive::PubApi::GenRandom(),Guid(peerid),0,0,
		peer_info.UdpPort,peer_info.StunIP,peer_info.StunUdpPort,peer_info.DetectUdpPort,real_ips,resource_ids,
		peer_info.PeerNatType,peer_info.UploadPriority,ns_pplive::PubApi::GenRandom()%60,ns_pplive::PubApi::GenRandom()%500+10,ns_pplive::PubApi::GenRandom()%1000,
		ns_pplive::PubApi::GenRandom()%100,endpoint_,ios_,0.2))
	{
		//printf("%s,%d,%s,%s\n",__FILE__,__LINE__,__FUNCTION__,tracker_client.GetErrMsg());
	}

	return true;
}
bool PressSimuHandler::ProcessAllReport()
{
	//��Ҫһ��ȫ�µ�peerid��rid���Ժ����е��ظ�
	unsigned random_num = ns_pplive::PubApi::GenRandom();
	//����ط���Ҫ���������Ǹĳɶ�д����

	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
	//lock.lock();
	if (candidate_peer_info_.size() == 0)
	{
		//û��list�����ݹ����������������ˡ�
		return false;
	}
	CandidatePeerInfo peer_info = candidate_peer_info_[random_num % candidate_peer_info_.size()];	
	

	Guid peerid = GetRandomUuid();
	peerids_.insert(peerid.to_string());

	lock.unlock();
	//��Ҫ����Ĳ���
	std::vector<boost::uint32_t> real_ips;
	real_ips.push_back(peer_info.DetectIP);
	std::vector<REPORT_RESOURCE_STRUCT> resource_ids;
	REPORT_RESOURCE_STRUCT rps;
	//typeΪ1��ʾ�����Դ
	int resnum = rand() %100;
	for(unsigned i=0;i<resnum;++i)
	{
		rps.Type = 1;
		rps.ResourceID = GetRandomRidByTimes();	
		resource_ids.push_back(rps);
	}
	//���������صĽ��
	boost::uint16_t keepalive_interval;
	boost::uint32_t detected_ip;
	boost::uint16_t detected_udp_port;
	//resource_count����ֶ�tracker��ʱû��ʹ��
	boost::uint16_t resource_count;
	//���ﻹ�кܶ�Ĳ�����Ҫ���ŵġ�
	ns_pplive::ns_tracker::TrackerClient tracker_client;

	//�ϱ���peer����peerGuid���Լ��ģ������Ķ��Ǵ�list����������ˡ�
	if (0 != tracker_client.ReportPeer(keepalive_interval,detected_ip,detected_udp_port,resource_count,ns_pplive::PubApi::GenRandom(),peerid,0,0,
		peer_info.UdpPort,peer_info.StunIP,peer_info.StunUdpPort,peer_info.DetectUdpPort,real_ips,resource_ids,
		peer_info.PeerNatType,peer_info.UploadPriority,ns_pplive::PubApi::GenRandom()%60,ns_pplive::PubApi::GenRandom()%500+10,ns_pplive::PubApi::GenRandom()%1000,
		ns_pplive::PubApi::GenRandom()%100,endpoint_,ios_,0.2))
	{
		//printf("%s,%d,%s,%s\n",__FILE__,__LINE__,__FUNCTION__,tracker_client.GetErrMsg());
	}


	return true;
}
bool PressSimuHandler::ProcessEmptyList()
{
	//��һ�������rid
	char tmp[32];
	char tmp2[32];
	string tmprid,tmppeerid;
	tmprid.assign(tmp,sizeof(tmp));
	tmppeerid.assign(tmp2,sizeof(tmp2));

	vector<CandidatePeerInfo> candidate_peer_info;
	ns_pplive::ns_tracker::TrackerClient tracker_client;
	if(0 != tracker_client.ListPeer(candidate_peer_info,rand(),GetRandomUuid(),GetRandomUuid(),50,endpoint_,ios_,0.1))
	{
		//printf("%s,%d,%s,%s\n",__FILE__,__LINE__,__FUNCTION__,tracker_client.GetErrMsg());
	}


	return true;
}
bool PressSimuHandler::ProcessCommList()
{
	//unsigned max_random_num = times_rid_.rbegin()->first;
	////����õ�Ҫ��ѯ��rid
	//unsigned random_num = ns_pplive::PubApi::GenRandom();
	//random_num %= max_random_num;
	//map<unsigned,string>::iterator it = times_rid_.lower_bound(random_num);	
	//assert(it != times_rid_.end());
	//��ѯ
	vector<CandidatePeerInfo> candidate_peer_info;
	string tmppeerid;
	char tmp[33]="A5415160B93147369F345713BE501A5C";
	tmppeerid.assign(tmp,sizeof(tmp)-1);
	ns_pplive::ns_tracker::TrackerClient tracker_client;
	if(0 != tracker_client.ListPeer(candidate_peer_info,ns_pplive::PubApi::GenRandom(),GetRandomRidByTimes(),Guid(tmppeerid),50,endpoint_,ios_,0.1))
	{
		//printf("%s,%d,%s,%s\n",__FILE__,__LINE__,__FUNCTION__,tracker_client.GetErrMsg());
	}
	//else
	//{
	//	printf("threadid:%d,randomnum:%u,%s,%d,%s,rid:%s,comlist get peer num:%d\n",boost::this_thread::get_id(),random_num,
	//		__FILE__,__LINE__,__FUNCTION__,it->second.c_str(),candidate_peer_info.size());
	//}

	//���²鵽��peerid������report��leave����ʹ��
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
	//lock.lock();
	if(candidate_peer_info.size()>0)
		candidate_peer_info_.swap(candidate_peer_info);
	lock.unlock();

	return true;
}
bool PressSimuHandler::ProcessEmptyLeave()
{
	ns_pplive::ns_tracker::TrackerClient tracker_client;
	tracker_client.Leave(ns_pplive::PubApi::GenRandom(),GetRandomUuid(),endpoint_,ios_,0.1);
	return true;
}
bool PressSimuHandler::ProcessCommLeave()
{
	unsigned random_num = ns_pplive::PubApi::GenRandom();
	//����ط���Ҫ���������Ǹĳɶ�д����

	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
	//lock.lock();
	if (candidate_peer_info_.size() == 0)
	{
		//û��list�����ݹ����������������ˡ�
		return false;
	}
	CandidatePeerInfo peer_info = candidate_peer_info_[random_num % candidate_peer_info_.size()];	
	string peerid;
	set<string>::iterator its = peerids_.upper_bound(GetRandomUuid().to_string());
	if(its != peerids_.end())
	{
		peerid = *its;
	}
	else
	{
		peerid = *(peerids_.begin());
	}
	peerids_.erase(peerid);

	//last_process_peerid = peerid;
	lock.unlock();
	ns_pplive::ns_tracker::TrackerClient tracker_client;
	
	tracker_client.Leave(ns_pplive::PubApi::GenRandom(),Guid(peerid),endpoint_,ios_,0.1);

	return true;
}

void PressSimuHandler::ProcessCmd(CMDTPYE cmd)
{
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
	//lock.lock();
	cmd_num_[cmd]++;
	lock.unlock();
	switch(cmd)
	{
		case PING_REPORT_CMD:
			ProcessPingReport();
			break;
		case ALL_REPORT_CMD:
			ProcessAllReport();
			break;
		case DEL_REPORT_CMD:
			ProcessDelReport();
			break;
		case ADD_REPORT_CMD:
			ProcessAddReport();
			break;
		case EMPTY_LIST_CMD:
			ProcessEmptyList();
			break;
		case COMM_LIST_CMD:
			ProcessCommList();
			break;
		case EMPTY_LEAVE_CMD:
			ProcessEmptyLeave();
			break;
		case COMM_LEAVE_CMD:
			ProcessCommLeave();
			break;
		default:
			break;
	}
}


int main(int argc,char* argv[])
{
	//���������ļ�
	//std::string config_file_name = "D:\\work\\p2p\\build\\msvc-90\\server_mod\\tracker\\tracker_press_simu\\Debug\\tracker_press_simu.config.txt";
    std::string config_file_name = "tracker_press_simu.config.txt";
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

	//report�����������	
	u_int report_cmd_percent = Config::Inst().GetInteger("report.report_cmd_percent",66);
	//ping����ռreport����ı�����ping������Ҫ��¼λ�ã�����һ���ĸ���������ping����ʾ���߻���ͻȻ����
	u_int ping_report_cmd_percent = Config::Inst().GetInteger("report.ping_report_cmd_percent",77);
	double totalpercent = 0.0;
	totalpercent += report_cmd_percent * 0.01 * ping_report_cmd_percent * 0.01;
	g_percent_cmd[totalpercent] = PING_REPORT_CMD;
	//reportȫ����ռreport����ı���������Ӧ�ú�leave����һ����
	u_int all_report_cmd_percent = Config::Inst().GetInteger("report.all_report_cmd_percent",17);
	totalpercent += report_cmd_percent * 0.01 * all_report_cmd_percent * 0.01;
	g_percent_cmd[totalpercent] = ALL_REPORT_CMD;
	//��ӵ�reportռ�ܵ�report�ı���
	u_int add_report_cmd_percent = Config::Inst().GetInteger("report.add_report_cmd_percent",3);
	totalpercent += report_cmd_percent * 0.01 * add_report_cmd_percent * 0.01;
	g_percent_cmd[totalpercent] = ADD_REPORT_CMD;
	//ɾ����reportռ�ܵ�report�ı���
	u_int del_report_cmd_percent = Config::Inst().GetInteger("report.del_report_cmd_percent",3);
	totalpercent += report_cmd_percent * 0.01 * del_report_cmd_percent * 0.01;
	g_percent_cmd[totalpercent] = DEL_REPORT_CMD;

	//all report ���ϱ����ݵ����ֵ
	u_int max_report_res_num = Config::Inst().GetInteger("report.max_report_res_num",200);
	//all report ���ϱ����ݵ�ƽ��ֵ
	u_int ave_report_res_num = Config::Inst().GetInteger("report.ave_report_res_num",50);

	//�������͵�����İٷֱ�֮����100
	assert(ping_report_cmd_percent+all_report_cmd_percent+add_report_cmd_percent+del_report_cmd_percent == 100);
	//��Ӻ�ɾ��������ܲ��̫��
	assert((add_report_cmd_percent < del_report_cmd_percent + 5) && (add_report_cmd_percent +5 > del_report_cmd_percent));
	assert(ave_report_res_num<max_report_res_num);


	//list�����������
	u_int list_cmd_percent = Config::Inst().GetInteger("list.list_cmd_percent",22);
	//��list�Ĳ�ѯ����
	u_int empty_list_cmd_percent = Config::Inst().GetInteger("list.empty_list_cmd_percent",19);
	//�����list�������load�ļ�����ֵĴ��������а��������

	totalpercent += list_cmd_percent * 0.01 * empty_list_cmd_percent * 0.01;
	g_percent_cmd[totalpercent] = EMPTY_LIST_CMD;

	totalpercent += list_cmd_percent * 0.01 * (100-empty_list_cmd_percent) * 0.01;
	g_percent_cmd[totalpercent] = COMM_LIST_CMD;

	//leave����
	u_int leave_cmd_percent = Config::Inst().GetInteger("leave.leave_cmd_percent",12);
	//����һ����tracker��û�е�peer��leave����������п�������Ϊǰ����ping��������ˡ��������Ӧ�úܵ͡�
	u_int empty_leave_cmd_percent = Config::Inst().GetInteger("leave.empty_leave_cmd_percent",1);

	totalpercent += leave_cmd_percent * 0.01 * empty_leave_cmd_percent * 0.01;
	g_percent_cmd[totalpercent] = EMPTY_LEAVE_CMD;

	totalpercent += leave_cmd_percent * 0.01 * (100-empty_leave_cmd_percent) * 0.01;
	g_percent_cmd[totalpercent] = COMM_LEAVE_CMD;

	assert(empty_leave_cmd_percent<5);
	assert(leave_cmd_percent+report_cmd_percent+list_cmd_percent == 100);

	//�����е�rid�ͳ��ִ�������
	//std::string ridtimes_file_name = "D:\\work\\p2p\\build\\msvc-90\\server_mod\\tracker\\tracker_press_simu\\Debug\\ridtimesfile.txt";
    std::string ridtimes_file_name = "ridtimesfile.txt";
	if (argc >= 3)
	{
		ridtimes_file_name = argv[2];
		printf("Load ridtimesfile %s\n.", ridtimes_file_name.c_str());
	}
	ReadRidTimes(ridtimes_file_name);

	string tracker_ip = Config::Inst().GetTString("tracker.ip","172.16.200.11");
	uint16_t tracker_port = Config::Inst().GetInteger("tracker.port",18000);

	boost::asio::ip::address_v4 av4;
	av4 = boost::asio::ip::address_v4::from_string(tracker_ip);
	boost::asio::ip::udp::endpoint end_point(boost::asio::ip::address(av4),tracker_port);

	//set<string> peerids;
	//peerids.insert("A5415160B93147369F345713BE501A5C");

	int thread_count = Config::Inst().GetInteger("tracker.thread_count",1);
	int cmd_speed = Config::Inst().GetInteger("tracker.cmd_speed",100);

	//std::string peerid_file_name = "D:\\work\\p2p\\build\\msvc-90\\server_mod\\tracker\\tracker_press_simu\\Debug\\peeridfile.txt";
    std::string peerid_file_name = "peeridfile.txt";
	if (argc >= 4)
	{
		peerid_file_name = argv[3];
		printf("Load peeridfile %s\n.", peerid_file_name.c_str());
	}

	ReadPeerids(peerid_file_name);

	//(int pool_count,const map<double,CMDTPYE>& percent_cmd,const map<unsigned,string>& times_rid,int cmd_speed,int speed_interval,const boost::asio::ip::udp::endpoint& end_point,const set<string> peerid)

	PressSimuHandler handler(thread_count,g_percent_cmd,g_times_rid,cmd_speed,0.1,end_point,g_peerid);
	handler.StartThreads();
	handler.WaitThreadsTermination();

	return 0;
}