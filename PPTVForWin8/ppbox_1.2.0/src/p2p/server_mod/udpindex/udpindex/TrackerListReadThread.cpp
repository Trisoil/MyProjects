#include "stdafx.h"
#include "TrackerListReadThread.h"
#include "framework/MainThread.h"
#include "udpindex/UdpIndexModule.h"
#include "udpindex/TrackerListConfig.h"
#include "udpindex/StunServerListConfig.h"
#include "protocal/IndexPacket.h"
#include "list_config.h"
#include "IndexFunc.h"

namespace udpindex
{
	boost::shared_ptr<TrackerListReadThread> TrackerListReadThread::inst_ = 
		boost::shared_ptr<TrackerListReadThread>(new TrackerListReadThread());

	void TrackerListReadThread::Start()
	{
		if (NULL == work_)
		{
			work_ = new boost::asio::io_service::work(ios_);
			thread_ = ::CreateThread(NULL, 0, TrackerListReadThread::Run, NULL, NULL, 0);
		}
	}

	void TrackerListReadThread::Stop()
	{
		if (NULL != work_)
		{
			delete work_;
			work_ = NULL;
			::WaitForSingleObject(thread_, INFINITE);
		}
	}

	void TrackerListReadThread::GetTrackerList()
	{
		TrackerListConfig::p tlc = TrackerListConfig::Create();

		tstring tracker_list_file = Config::Inst().GetTString("index.trackerlistfile");
		if (tracker_list_file.empty())
		{
			CONSOLE_LOG("Failed to Get a Filename of TrackerList!");
			return;
		}

		tlc->LoadFile(tracker_list_file.c_str());

		QueryTrackerListResponsePacket::p packet = tlc->GetTrackerListPacket();
		//post到主线程读取成功 OnTrackerList;
		INDEX_EVENT("读TrackerList成功");
		MainThread::IOS().post(
			boost::bind(&UdpIndexModule::OnGetTrackerList,UdpIndexModule::Inst(), packet)
			);
		
	}

	void TrackerListReadThread::GetStunServerList()
	{
		StunServerListConfig::p tlc = StunServerListConfig::Create();

		tstring stun_list_file = Config::Inst().GetTString("index.stunlistfile");
		if (stun_list_file.empty())
		{
			CONSOLE_LOG("Failed to Get a Filename of StunServerList!");
			return;
		}

		tlc->LoadFile(stun_list_file.c_str());

		QueryStunServerListResponsePacket::p packet = tlc->GetStunServerListPacket();
		//post到主线程读取成功 OnStunList;
		INDEX_EVENT("读StunServerList成功");
		MainThread::IOS().post(
			boost::bind(&UdpIndexModule::OnGetStunServerList,UdpIndexModule::Inst(), packet)
			);

	}

	void TrackerListReadThread::GetIndexServerList()
	{
		tstring index_list_file = Config::Inst().GetTString("index.indexlistfile");
		if (index_list_file.empty())
		{
			CONSOLE_LOG("Failed to Get a Filename of IndexServerList!");
			return;
		}
		vector<INDEX_SERVER_INFO> index_list;
		vector<UINT8> mode_list;
		bool res = ListConfig::GetIndexServerList(index_list_file, index_list, mode_list);
		protocal::QueryIndexServerListResponsePacket::p packet = 
			protocal::QueryIndexServerListResponsePacket::CreatePacket(protocal::Packet::NewTransactionID(),
			mode_list, index_list);
		if (res) 
		{
			INDEX_EVENT("读IndexServerList成功");
			MainThread::IOS().post(
				boost::bind(&UdpIndexModule::OnGetIndexServerList, UdpIndexModule::Inst(), packet)
				);
			return;
		}
		CONSOLE_LOG("读取IndexServerList失败");
	}

	void TrackerListReadThread::GetDBList()
	{
		tstring db_list_file = Config::Inst().GetTString("index.dblistfile");
		if (db_list_file.empty())
		{
			CONSOLE_LOG("Failed to Get a Filename of DBList!");
			return;
		}
		vector<HostInfo> db_list;
		vector<DBInfo> db_info;
		vector<boost::asio::ip::udp::endpoint> stat_list;
		bool res = ListConfig::GetDBList(db_list_file, db_list, db_info, stat_list);
		if (res) 
		{
			INDEX_EVENT("读DBList成功");
			MainThread::IOS().post(
				boost::bind(&UdpIndexModule::OnGetDBList, UdpIndexModule::Inst(), db_info, db_list, stat_list)
				);
			return;
		}
		CONSOLE_LOG("读取DBList失败");
	}
}
