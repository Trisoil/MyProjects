#include "Common.h"
#include "BootstrapClient.h"
#include "TrackerClient.h"
#include <sstream>
#include <iostream>
#include <boost/thread/thread.hpp> 
#include <boost/bind.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
using namespace std;
using boost::asio::ip::udp;
using namespace framework::configure;
boost::asio::io_service g_ios;
boost::interprocess::interprocess_mutex peerinfo_mutex_;
boost::interprocess::interprocess_mutex trackerinfo_mutex_;
boost::interprocess::interprocess_mutex number_mutex_1;
boost::interprocess::interprocess_mutex number_mutex_2;
#include <log4cplus/configurator.h>
int g_argc = 0;
char** g_argv;
int g_argindex = 1;
int available_number = 0;
int unavailable_number = 0;
int global_index = 0;
void readcin(string& value);
void readcin(int& value);
void readcin(double& value);

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

void Process_query_tracker(set<TRACKER_INFO> *total_tracker_info, udp::endpoint endpoint_)
{
	Guid pid;
	pid.generate();
	vector<TRACKER_INFO> tracker_info;
	ns_pplive::ns_bootstrap::BootstrapClient client;
	if (0 != client.QueryTrackerList(tracker_info,boost::uint32_t(rand()),Guid(pid),endpoint_,g_ios,2.0))
	{
		cout<<"QueryTrackerList fail!"<<endl;
		return;
	}

	for(unsigned i=0;i<tracker_info.size();++i)
	{
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(peerinfo_mutex_);
		(*total_tracker_info).insert(tracker_info[i]);
		lock.unlock();
	}

}

bool Query_all_tracker(set<TRACKER_INFO> *total_tracker_info)
{
	udp::endpoint endpoint_;
	int cmd = -1;
	readcin(cmd);

	if (0 == cmd)
	{
		Guid pid;

		udp::resolver resolver(g_ios);
		udp::resolver::query query(udp::v4(),"ppvabs.pplive.com", "6400");

		boost::system::error_code ec;
		udp::resolver::iterator endpoint_iterator = resolver.resolve(query, ec);
		if(ec) 
		{ 
			std::cout <<boost::system::system_error(ec).what() << std::endl; 
			return false; 
		} 

		udp::resolver::iterator end;
		if (endpoint_iterator == end)
		{
			std::cout<<"query failure!"<<std::endl;
		}
		endpoint_ = (*endpoint_iterator);
		cout<<"bs endpoint: "<<endpoint_<<endl;
	}
	else
	{
		string bs_ip_str;
		readcin(bs_ip_str);
		boost::asio::ip::address_v4 av4;
		av4 = boost::asio::ip::address_v4::from_string(bs_ip_str);
		endpoint_ = boost::asio::ip::udp::endpoint(boost::asio::ip::address(av4), 6400);
		cout<<"input bs endpoint: "<<endpoint_<<endl;
	}

	std::vector<boost::thread*> thread_ids;
	int i = 0;
	while (i<100)
	{
		boost::thread* pThread = new boost::thread(boost::bind(&Process_query_tracker, total_tracker_info, endpoint_));
		++i;
		if(NULL == pThread)
		{
			return false;
		}
		else
		{
			thread_ids.push_back(pThread);
		}
	}

	for(uint32_t i=0;i<thread_ids.size();++i)
	{
		thread_ids[i]->join();
	}

	cout<<"query times: "<<i<<endl;
	cout<<"tracker number: "<<total_tracker_info->size()<<endl;

}

bool ProcessQuerySpecialTrackerList()
{
	string trackerag_ip;
	vector<TRACKER_INFO> tracker_info;
	set<TRACKER_INFO> total_tracker_info;
	set<TRACKER_INFO> special_tracker_info;
	int group_num = 0;

	map<int, set<TRACKER_INFO> > tracker_group;
	Query_all_tracker(&total_tracker_info);

	string path;
	readcin(path);

	while (g_argindex < g_argc) //compare with trackerag ip
	{
		readcin(trackerag_ip);
		string prefix = trackerag_ip.substr(0, trackerag_ip.find_last_of('.'));
		for (set<TRACKER_INFO>::iterator it = total_tracker_info.begin(); it != total_tracker_info.end(); ++it)
		{
			string trackerip_string = boost::asio::ip::address_v4((it->IP)).to_string();

			if (trackerip_string.substr(0, trackerip_string.find_last_of('.')) == prefix)
			{
				special_tracker_info.insert(*it);
			}
		}
	}

	cout<<"special_tracker_info size: "<<special_tracker_info.size()<<endl;
	for(set<TRACKER_INFO>::iterator it = special_tracker_info.begin(); it != special_tracker_info.end(); ++it)
	{
		if (it->ModNo > group_num) group_num = it->ModNo;
		tracker_group[it->ModNo].insert(*it);
	}
	group_num++;
	if (group_num != tracker_group.size())
	{
		std::cout<<"group_num should equal to tracker_group.size!"<<std::endl;
	}

	Profile myprofile;
	if(0 != myprofile.load(path))
	{
		cout<<"load error"<<path<<endl;
		return false;
	}
	string section = "trackerag";
	string key = "tracker.groupnum";
	stringstream stream;
	string value;
	stream<<group_num;
	stream>>value;
	stream.clear();

	myprofile.set(section, key, value, false);

	//cout<<"tracker_group size: "<<tracker_group.size()<<endl;
	for (map<int, set<TRACKER_INFO> >::iterator iter1 = tracker_group.begin(); iter1 != tracker_group.end(); ++iter1)
	{
		stream<<"tracker.groupnum_"<<iter1->first;
		stream>>key;
		stream.clear();
		stream<<iter1->second.size();
		stream>>value;
		stream.clear();
		myprofile.set(section, key, value, false);

		int i = 0;
		for (set<TRACKER_INFO>::iterator iter2 = iter1->second.begin(); iter2 != iter1->second.end(); ++iter2)
		{
			stream<<"tracker.ipport_"<<iter1->first<<"_"<<i;
			stream>>key;
			stream.clear();
			stream<<boost::asio::ip::address_v4((iter2->IP)).to_string()<<":"<<iter2->Port;
			stream>>value;
			stream.clear();
			myprofile.set(section, key, value, false);
			i++;
		}
	}
	myprofile.save();


	return true;

}

bool TestATracker(udp::endpoint& end_point_)
{
	//验证list，report是否正常
	//先上报一个奇怪的资源，然后去list，期待只得到一个结果。
	boost::uint16_t keepalive_interval;
	boost::uint32_t detected_ip;
	boost::uint16_t detected_udp_port;
	boost::uint16_t resource_count;
	Guid peer_id;
	peer_id.generate();
	std::vector<boost::uint32_t> real_ips;
	real_ips.push_back(1232423);
	vector<REPORT_RESOURCE_STRUCT> resource_ids;
	REPORT_RESOURCE_STRUCT res_id;
	res_id.ResourceID.generate();
	res_id.Type = 1;
	resource_ids.push_back(res_id);

	ns_pplive::ns_tracker::TrackerClient client;
	if(0 != client.ReportPeer(keepalive_interval,detected_ip,detected_udp_port,resource_count,rand(),peer_id,1,1,5041,123124,1234,123456,342,real_ips,resource_ids,255,10,10,64,64,10,end_point_,g_ios,1.0))
	{
		return false;
	}

	//(std::vector<CandidatePeerInfo>& candidate_peer_info,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
	//boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
	std::vector<CandidatePeerInfo> candidate_peer_info;
	if(0 != client.ListPeer(candidate_peer_info,rand(),res_id.ResourceID,peer_id,50,end_point_,g_ios,1.0))
	{
		return false;
	}
	if(candidate_peer_info.size()>=1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Process_check_tracker(vector<udp::endpoint>* check_tracker_info)
{
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(trackerinfo_mutex_);
	int i = global_index;
	global_index++;
	lock.unlock();

	int k = 0;
	while (k < 10)
	{

		if (TestATracker((*check_tracker_info)[i]))
		{
			//std::cout<<(*check_tracker_info)[i]<<" available"<<std::endl;
			boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(number_mutex_1);
			available_number++;
			lock.unlock();
			break;
		}
	}

	if (k == 10)
	{
		std::cout<<(*check_tracker_info)[i]<<" unavailable"<<std::endl;
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(number_mutex_2);
		unavailable_number++;
		lock.unlock();
	}


}

bool CheckTrackerAvailibity()
{
	set<TRACKER_INFO> total_tracker_info;
	vector<udp::endpoint> check_tracker_info;
	Query_all_tracker(&total_tracker_info);

	for (set<TRACKER_INFO>::iterator it = total_tracker_info.begin(); it != total_tracker_info.end(); ++it)
	{
		udp::endpoint endpoint_(boost::asio::ip::address_v4((it->IP)), it->Port);
		check_tracker_info.push_back(endpoint_);
	}

	std::vector<boost::thread*> thread_ids;
	int i = 0;
	//int process_number = (check_tracker_info.size()/200)+1;
	int process_number = check_tracker_info.size();
	//std::cout<<"each thread process tracker number: "<<process_number<<std::endl;
	while (i < process_number)
	{
		boost::thread* pThread = new boost::thread(boost::bind(&Process_check_tracker, &check_tracker_info));
		++i;
		if(NULL == pThread)
		{
			return false;
		}
		else
		{
			thread_ids.push_back(pThread);
		}
	}

	for(uint32_t i=0;i<thread_ids.size();++i)
	{
		thread_ids[i]->join();
	}

	std::cout<<"available tracker number: "<<available_number<<std::endl;
	std::cout<<"unavailable tracker number: "<<unavailable_number<<std::endl;


	return true;
}



int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout<<"para count error!"<<std::endl;
		return 1;
	}
	g_argc = argc;
	g_argv = argv;


	int cmd=-1;
	readcin(cmd);

	switch (cmd)
	{
	case 0:
		ProcessQuerySpecialTrackerList();
		break;
	case 1:
		CheckTrackerAvailibity();
		break;
	default:
		cout<<"error input cmd"<<endl;
		return 0;

	}

	return 0;
}