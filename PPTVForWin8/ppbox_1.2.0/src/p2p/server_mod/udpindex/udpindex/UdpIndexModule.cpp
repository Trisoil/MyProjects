#include "stdafx.h"
#include "udpindex/UdpIndexModule.h"
#include "protocal/IndexPacket.h"
#include "protocal/StatisticPacket.h"

#include "udpindex/DataBaseThreadPoolAdd.h"
#include "udpindex/DataBaseThreadPoolQuery.h"
#include "udpindex/TrackerListReadThread.h"
#include "udpindex/DataBase.h"
#include "udpindex/DataManager.h"
#include "udpindex/Config.h"
#include <boost/format.hpp>
#include <numeric>
#include "ClearDBCacheThread.h"
#include "UrlVote.h"
#include "udpindex/BlackList.h"

using namespace protocal;

namespace udpindex
{
	UdpIndexModule::p UdpIndexModule::inst_(new UdpIndexModule());

	void UdpIndexModule::Start(u_short local_udp_port)
	{
		if(is_running_ == true) return;

		url_cache_attach_times_ = 0;
		url_cache_query_times_ = 0;
// 		rid_cache_attach_times_ = 0;
// 		rid_cache_query_times_ = 0;
		content_cache_attach_times_ = 0;
		content_cache_query_times_ = 0;
		url_sql_faild_query_times_ = 0;
		content_sql_faild_query_times_ = 0;
//		rid_sql_faild_query_times_ = 0;
		url_sql_succed_query_times_ = 0;
		content_sql_succed_query_times_ = 0;
//		rid_sql_succed_query_times_ = 0;
		add_ridurl_times_ = 0;
		url_sql_file_too_small_times_ = 0;
		content_sql_file_too_small_times_ = 0;

		waiting_add_url_rid_no_ = 0;
		waiting_query_rid_by_content_no_ = 0;
		waiting_query_rid_by_url_no_ = 0;

		add_busy_times_=0;
		url_busy_times_=0;
		content_busy_times_=0;

		database_threadpool_test_ = 0;
        
        min_protocol_version_ = 0;
		
		unsigned int tracker_list_time = Config::Inst().GetInteger("index.trackerlisttime");

		if (tracker_list_time == 0)
		{
			RELEASE_LOG("The Tracker List Time is a Zero and It is set as 600s");
			tracker_list_time = 600;
		}

		tracker_list_time *= 1000;

		get_tracker_list_timer_ = framework::timer::PeriodicTimer::create(tracker_list_time, shared_from_this());

		print_statistic_timer_ = framework::timer::PeriodicTimer::create(24*60*60*1000, shared_from_this());

		print_statistic_timer_->Start();

		get_tracker_list_timer_->Start();

		udp_server_ = framework::network::UdpServer::create(shared_from_this());

		is_send_packet_to_log_server_ = Config::Inst().GetInteger("index.ishavelogserver");

		if (!is_send_packet_to_log_server_)
		{
			CONSOLE_OUTPUT("Not Send Packet to the LogServer");
		}

		max_content_cache_size_ = Config::Inst().GetInteger("index.max.content.cache.size");
		max_url_cache_size_ = Config::Inst().GetInteger("index.max.url.cache.size");

		max_add_waiting_length_ = Config::Inst().GetInteger("index.max_add_waitting_queue");
		max_query_by_url_waiting_length_ = Config::Inst().GetInteger("index.max_query_by_url_waiting_queue");
		max_query_by_content_waiting_length_ = Config::Inst().GetInteger("index.max_query_by_content_waiting_queue");

        // 读取配置文件的最低支持版本
        min_protocol_version_ = Config::Inst().GetInteger("index.min_protocol_version");

		min_file_length_ = Config::Inst().GetInteger("index.min_udp_packet_file_length");

		if (max_content_cache_size_ == 0)
		{
			RELEASE_LOG("The Max Content Cache Size is a Zero!!!!!");
		}
		if (max_url_cache_size_ == 0)
		{
			RELEASE_LOG("The Max URL Cache Size is a Zero!!!!!");
		}
		if (max_add_waiting_length_ == 0)
		{
			CONSOLE_OUTPUT("The Max Add Waiting Queue Length is a Zero!!!!!");
		}
		if (max_query_by_url_waiting_length_ == 0)
		{
			CONSOLE_OUTPUT("The Max QueryByUrl Waiting Queue Length is a Zero!!!!!");
		}
		if (max_query_by_content_waiting_length_ == 0)
		{
			CONSOLE_OUTPUT("The Max QueryByContent Waiting Queue Length is a Zero!!!!!");
		}
		if (min_file_length_ == 0)
		{
			RELEASE_LOG("The min_packet_size is a Zero and it is set as 2000");
			min_file_length_ = 2000;
		}
		
		tracker_list_packet_ = QueryTrackerListResponsePacket::p();
		stun_list_packet_ = QueryStunServerListResponsePacket::p();
		index_list_packet_ = QueryIndexServerListResponsePacket::p();
		INDEX_INFO("UdpIndexModule::Start() 读TrackerList和StunServdrList文件");

        string black_list_name = Config::Inst().GetValue("index.blacklist", "");
		
		TrackerListReadThread::IOS().post(
			boost::bind(&TrackerListReadThread::GetDBList, &TrackerListReadThread::Inst())
			);
        TrackerListReadThread::IOS().post(
            boost::bind(&BlackList::Start, &BlackList::Inst(), black_list_name)
            );


		if( udp_server_->Listen(local_udp_port) == false )
		{
			CONSOLE_OUTPUT("Failed to Listen to the UDP Port " << local_udp_port);
			CONSOLE_OUTPUT("I Have Stopped to Work!!!!!");
			CONSOLE_OUTPUT("Please Type \"exit\" to Quit");
			MainThread::IOS().post(
				boost::bind(&UdpIndexModule::Stop, UdpIndexModule::Inst())
				);
		}
		else
		{
			CONSOLE_OUTPUT("UdpIndexModule::Start() UDP Listen To port " << local_udp_port);
		}

		int udp_server_receive_count = Config::Inst().GetInteger("index.udp_server_receive_count");
		if (udp_server_receive_count == 0)
		{
			RELEASE_LOG("Failed to Read the udp_server_receive_count from the Config File!!!!");
			RELEASE_LOG("The udp_server_receive_count has been Set as 1000");
			udp_server_receive_count = 1000;
		}
		CONSOLE_OUTPUT("The udp_server_receive_count is "<<udp_server_receive_count);
		udp_server_->Recv(udp_server_receive_count);

		statistic_ = IndexStatistic::Create();
		statistic_->Start();

		url_cache_timer_.Sync();

		string statistic_ip;

#if _UNICODE
		statistic_ip = framework::w2b(Config::Inst().GetTString("index.statistic.ip")); 
#else
		statistic_ip = Config::Inst().GetTString("index.statistic.ip"); 
#endif
		u_short statistic_port = Config::Inst().GetInteger("index.statistic.port"); 

		if (statistic_ip.empty() || statistic_port == 0)
		{
			CONSOLE_LOG("Failed to Get the Information of Statistic Server!");
			statistic_ip = "127.0.0.1";
			statistic_port = 8900;
		}
		else
		{
			CONSOLE_OUTPUT("StatisticIP   = " << statistic_ip);
			CONSOLE_OUTPUT("StatisticPort = " << statistic_port);
		}

		statistic_endpoint_ = framework::network::IpPortToUdpEndpoint(statistic_ip, statistic_port);

		is_running_ = true;
	}

	void UdpIndexModule::Stop()
	{	
		if(is_running_ == false) return;
		is_running_ = false;

		get_tracker_list_timer_->Stop(); //停止timer
		print_statistic_timer_->Stop();

        TrackerListReadThread::IOS().post(
            boost::bind(&BlackList::Stop, &BlackList::Inst())
            );


		statistic_->Stop();
		if (udp_server_)
		{
			udp_server_->Close();
		}
	}

    void UdpIndexModule::DoSendPacket(const boost::asio::ip::udp::endpoint& end_point, protocal::Packet::p packet,
        int peer_version)
	{
		//发送Packet

		if( is_running_ == false ) return;

		Buffer buf = packet->GetBuffer();

		// 添加校验
		if( protocal::Cryptography::Encrypt(buf, peer_version) == false)
		{
			LOG(__ERROR, "packet", "");
			INDEX_EVENT("UdpIndexModule::DoSendPacket() 添加校验失败");
			return;
		}

		assert(buf.length_ <= 2048);
		INDEX_EVENT("UdpIndexModule::DoSendPacket() 发包");

		udp_server_->UdpSendTo(buf, end_point);
	}

	void UdpIndexModule::DoSendBuffer(boost::asio::ip::udp::endpoint& end_point, const Buffer& buffer)
	{
		if( is_running_ == false ) return;

		udp_server_->UdpSendTo(buffer, end_point);
	}

	void UdpIndexModule::OnTimerElapsed(framework::timer::Timer::p pointer, u_int times)
		//计时器触发事件
	{		
		if( is_running_ == false ) return;
		if (pointer == get_tracker_list_timer_)
		{
			INDEX_EVENT("UdpIndexModule::OnTimerElapsed() TimerElapsed");
		
			TrackerListReadThread::IOS().post(
				boost::bind(&TrackerListReadThread::GetDBList, &TrackerListReadThread::Inst())
				);
            TrackerListReadThread::IOS().post(
                boost::bind(&BlackList::Refresh, &BlackList::Inst())
                );
		}
		else if (pointer == print_statistic_timer_)
		{			
			PrintCache(7,"timer", 0);

			url_cache_attach_times_ = 0;
			url_cache_query_times_ = 0;
			// 		rid_cache_attach_times_ = 0;
			// 		rid_cache_query_times_ = 0;
			content_cache_attach_times_ = 0;
			content_cache_query_times_ = 0;
			url_sql_faild_query_times_ = 0;
			content_sql_faild_query_times_ = 0;
			//		rid_sql_faild_query_times_ = 0;
			url_sql_succed_query_times_ = 0;
			content_sql_succed_query_times_ = 0;
			//		rid_sql_succed_query_times_ = 0;
			add_ridurl_times_ = 0;
			url_sql_file_too_small_times_ = 0;
			content_sql_file_too_small_times_ = 0;

			add_busy_times_=0;
			url_busy_times_=0;
			content_busy_times_=0;

			statistic_->Clear();

			url_cache_timer_.Sync();

            UrlVote::Inst().ClearUp();
		}
	}

	void UdpIndexModule::OnUdpRecv(boost::asio::ip::udp::endpoint& end_point, Buffer& buf)
		//处理接收到的请求
	{
		if (is_running_ == false) return;

        // 首先判断最低版本
        const byte * const data = buf.data_.get();
        boost::uint16_t protocol_version = *(boost::uint16_t*)(data+10);

        if (protocol_version < min_protocol_version_)
        {
            return;
        }

 		if( protocal::Cryptography::Decrypt(buf) == false)
 		{
			RELEASE_LOG("Bad Packet " << end_point);
			return;
 		}

		// 检查包头部正确， 如果读取包头部失败，不处理
		protocal::Packet::p packet = protocal::Packet::ParseFromBinary(buf);
		
		if(!packet)
		{
			LOG(__ERROR, "packet", "AppModule::OnUdpRecv");
			assert(0);
			return;
		}

		// QueryRidByUrl
		else if( packet->GetAction() == 0x12)
		{
 			QueryRidByUrlRequestPacket::p packet = QueryRidByUrlRequestPacket::ParseFromBinary(buf);
 			if (!packet)
 			{
 				return;
 			}
 
 			INDEX_EVENT("UdpIndexModule::OnUdpRecv() 收到QueryRidByUrl");
 			OnQueryRidByUrl(end_point, packet);

			StatisticRequestPacket::p send_packet = StatisticIndexPeerPacket::CreatePacket(
				packet->GetTransactionID(), packet->GetPeerVersion(), time(NULL), packet->GetPeerGuid(),
				protocal::StatisticRequestPacket::PEER_QUERY_URL);
			if (!send_packet)
			{
				RELEASE_LOG("UdpIndexModule::OnUdpRecv() Failed to Create Packet");
			}
			else
			{
				//DoSendPacket(statistic_endpoint_, send_packet);
			}	
		} 

		// AddRidUrl
		else if( packet->GetAction() == 0x13)
		{
			AddRidUrlRequestPacket::p packet = AddRidUrlRequestPacket::ParseFromBinary(buf);
			if (!packet)
			{
				return;
			}
			INDEX_EVENT("UdpIndexModule::OnUdpRecv() 收到AddRidUrl");

			OnAddRidUrl(end_point, packet);

		} 

		else if ( packet->GetAction() == 0x16 )
		{
			QueryRidByContentRequestPacket::p packet = QueryRidByContentRequestPacket::ParseFromBinary(buf);
			if (!packet)
			{
				return;
			}

			INDEX_EVENT("UdpIndexModule::OnUdpRecv() 收到QueryRidByContent");
			OnQueryRidByContent(end_point, packet);

			StatisticRequestPacket::p send_packet = StatisticIndexPeerPacket::CreatePacket(
				packet->GetTransactionID(), packet->GetPeerVersion(), time(NULL), packet->GetPeerGuid(),
				protocal::StatisticRequestPacket::PEER_QUERY_CONTENT);
			if (!send_packet)
			{
				RELEASE_LOG("UdpIndexModule::OnUdpRecv() Failed to Create Packet");
			}
			else
			{
				//DoSendPacket(statistic_endpoint_, send_packet);
			}
		}
	}

	void UdpIndexModule::OnGetTrackerList(QueryTrackerListResponsePacket::p packet)
	{		
		if( is_running_ == false ) return;
		
		this->tracker_list_packet_ = packet;
		//更新内存中的TrackerList Buffer
	}

	void UdpIndexModule::OnGetStunServerList(QueryStunServerListResponsePacket::p packet)
	{		
		if( is_running_ == false ) return;

		this->stun_list_packet_ = packet;
		//更新内存中的StunServerList Buffer
	}

	void UdpIndexModule::OnGetIndexServerList(QueryIndexServerListResponsePacket::p packet)
	{		
		if( is_running_ == false ) return;

		this->index_list_packet_ = packet;
		//更新内存中的StunServerList Buffer
	}

	void UdpIndexModule::OnGetDBList(vector<DBInfo> db_info, vector<HostInfo> db_host, 
		vector<boost::asio::ip::udp::endpoint> stat_list)
	{
		DataBaseConnectionPool::Inst().OnGetDBList(db_info, db_host, stat_list);
	}

	void UdpIndexModule::OnQueryHttpServerByRid(boost::asio::ip::udp::endpoint& end_point, 
		QueryHttpServerByRidRequestPacket::p packet)
	{		
		ResponseServerPacket::p response_packet = ResponseServerPacket::CreatePacket(
			QueryHttpServerByRidResponsePacket::ACTION, packet->GetTransactionID(), 1);

		if (!response_packet)
		{
			assert(0);
		}

		DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
	}

	void UdpIndexModule::OnQueryRidByUrl(boost::asio::ip::udp::endpoint& end_point, 
		QueryRidByUrlRequestPacket::p packet)
	{		
		if( is_running_ == false ) return;

        statistic_->SubmitQueryRidByUrlRequest(packet->GetBuffer().length_);

		string str_t = packet->GetUrlString();
        string str_ref = packet->GetReferString();

        string mini_url = "";

        bool is_good_url = true;

		if (str_t.empty() || str_t.find("\r\n") != string::npos)
		{
			RELEASE_LOG("UdpIndexModule::OnQueryRidByUrl()  Bad Packet! Unsafe url_");
			RELEASE_LOG("URL = " << str_t << "; Length = " << packet->GetUrlString().size());
			is_good_url = false;
		}

        if (is_good_url)
        {
            if (str_t.length() > 1000 || packet->GetReferLength() > 1000)
            {
                RELEASE_LOG("UdpIndexModule::OnQueryRidByUrl()  There\'s too long an url!");
                RELEASE_LOG("UdpIndexModule::OnQueryRidByUrl()  url = " << str_t);
                is_good_url = false;
            }

            mini_url = Url2Mini(str_t,packet->GetReferString());
            if (mini_url == "/")
            {
                RELEASE_LOG("UdpIndexModule::OnQueryRidByUrl()  There\'s an bad URL! URL = "<<str_t);
                is_good_url = false;
            }
            if (mini_url.empty())
            {
                RELEASE_LOG("UdpIndexModule::OnQueryRidByUrl()  There\'s an null mini-url!");
                is_good_url = false;
            }

            if (boost::algorithm::to_lower_copy(packet->GetUrlString()).find("start=") != string::npos)
            {
                is_good_url = false;
            }
        }

        if (is_good_url == false || BlackList::Inst().IsAllow(str_ref) == false)
        {
            OnQueryRidByUrlDenial(end_point, packet);
            return;
        }


		url_cache_query_times_ ++;


		INDEX_EVENT("查找URL = " << packet->GetUrlString());

		hash_map<string, UrlHashmapStruct>::iterator it = url_hash_map_.find(mini_url);
		// 查询在Hashmap中是否有该条记录

		if(it != url_hash_map_.end())
		{
			// 如果有：   得出该Buffer 
			//            调用 OnSQLQueryRidByUrl
			url_cache_attach_times_++;
			INDEX_EVENT("UdpIndexModule::OnQueryRidByUrl() 在哈希表中找到");
			OnSQLQueryRidByUrl(end_point, packet, it->second.packet);

			if (is_send_packet_to_log_server_)
			{
				OnVisitKey(DataBaseConnectionPool::Inst().GetStatEndPoint(boost::hash_value(mini_url)%256),
					protocal::StatisticRequestPacket::PEER_QUERY_URL_SUCCESS, packet->GetPeerGuid(),
					packet->GetPeerVersion(), packet->GetTransactionID(), "U_"+Url2MD5(mini_url).ToString());
				OnVisitKey(DataBaseConnectionPool::Inst().GetStatEndPoint(boost::hash_value(it->second.packet->GetResourceID())%256),
					protocal::StatisticRequestPacket::PEER_QUERY_CONTENT_SUCCESS, packet->GetPeerGuid(),
					packet->GetPeerVersion(), packet->GetTransactionID(), "R_"+it->second.packet->GetResourceID().ToString());
			}
		}
		else
		{
			// 如果没有:  
			// 到数据库中查找
			if (waiting_query_rid_by_url_no_ < max_query_by_url_waiting_length_)
			{
				DataManager::p database_ = DataManager::Create();

				NeedToDoQueryRidByUrl();

				INDEX_EVENT("UdpIndexModule::OnQueryRidByUrl() 到数据库中查找");
				DataBaseThreadPoolQuery::IOS().post(
					boost::bind(&DataManager::QueryRIDByUrl, database_, end_point, packet)
					);
			}
			else
			{
				ResponseServerPacket::p response_packet = ResponseServerPacket::CreatePacket(
					QueryRidByUrlResponsePacket::ACTION, packet->GetTransactionID(), 3);

				if (!response_packet)
				{
					assert(0);
				}
				INDEX_EVENT("UdpIndexModule::OnQueryRidByUrl() 等待队列过长");
				DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
				url_busy_times_++;
			}
		}
	}

	void UdpIndexModule::OnQueryRidByContent(boost::asio::ip::udp::endpoint& end_point, 
		QueryRidByContentRequestPacket::p packet)
	{		
		if( is_running_ == false ) return;

        if (packet->GetContentSenseMD5().IsEmpty() || packet->GetFileLength() == 0 || BlackList::Inst().IsEffect())
		{
			RELEASE_LOG("UdpIndexModule::OnQueryRidByContent()  Bad Packet!");
            OnQueryRidByCttDenial(end_point, packet);
			return;
		}
		content_cache_query_times_ ++;


		INDEX_EVENT("查找Content = " << packet->GetContentSenseMD5().ToString());

		statistic_->SubmitQueryRidByContentRequest(packet->GetBuffer().length_);

		hash_map<pair_hash, ContentHashmapStruct>::iterator it = 
			content_hash_map_.find(make_pair(packet->GetContentSenseMD5(),packet->GetFileLength()));
		// 查询在Hashmap中是否有该条记录

		if(it != content_hash_map_.end())
		{
			// 如果有：   得出该Buffer 
			//            调用 OnSQLQueryRidByContent
			content_cache_attach_times_++;
			INDEX_EVENT("UdpIndexModule::OnQueryRidByContent() 在哈希表中找到");
			OnSQLQueryRidByContent(end_point, packet, it->second.packet);

			if (is_send_packet_to_log_server_)
			{
				OnVisitKey(DataBaseConnectionPool::Inst().GetStatEndPoint(boost::hash_value(packet->GetContentSenseMD5())%256),
					protocal::StatisticRequestPacket::PEER_QUERY_CONTENT_SUCCESS, packet->GetPeerGuid(),
					packet->GetPeerVersion(), packet->GetTransactionID(), 
                    "C_"+Ctt2MD5(packet->GetContentSenseMD5(), packet->GetContentBytes(), packet->GetFileLength()).ToString());
				OnVisitKey(DataBaseConnectionPool::Inst().GetStatEndPoint(boost::hash_value(it->second.packet->GetResourceID())%256),
					protocal::StatisticRequestPacket::PEER_QUERY_CONTENT_SUCCESS, packet->GetPeerGuid(),
					packet->GetPeerVersion(), packet->GetTransactionID(), "R_"+it->second.packet->GetResourceID().ToString());
			}
		}
		else
		{
			// 如果没有:  
			// 到数据库中查找
			if(waiting_query_rid_by_content_no_ < max_query_by_content_waiting_length_)
			{
				DataManager::p database_ = DataManager::Create();

				NeedToDoQueryRidByContent();

				INDEX_EVENT("UdpIndexModule::OnQueryRidByContent() 到数据库中查找");
				DataBaseThreadPoolQuery::IOS().post(
					boost::bind(&DataManager::QueryRIDByCtt, database_, end_point, packet)
					);
			}
			else
			{
				ResponseServerPacket::p response_packet = ResponseServerPacket::CreatePacket(
					QueryRidByContentResponsePacket::ACTION, packet->GetTransactionID(), 3);

				if (!response_packet)
				{
					assert(0);
				}
				INDEX_EVENT("UdpIndexModule::OnQueryRidByContent() 等待队列过长");
				DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
				content_busy_times_++;
			}
		}
	}

	void UdpIndexModule::OnAddRidUrl(boost::asio::ip::udp::endpoint& end_point, AddRidUrlRequestPacket::p packet)
	{		
		if( is_running_ == false ) return;


        statistic_->SubmitAddUrlRidRequest(packet->GetBuffer().length_);

        bool is_good_url = true;
		
		if (packet->GetResourceInfo().url_info_s_.empty())
		{
			RELEASE_LOG("UdpIndexModule::OnAddRidUrl()  Bad Packet! Empty url_info_s_");
			is_good_url = false;
		}

		INDEX_EVENT("UdpIndexModule::OnAddRidUrl() RID = " << packet->GetResourceInfo().rid_.ToString());
		INDEX_EVENT("UdpIndexModule::OnAddRidUrl() URL = " << packet->GetResourceInfo().url_info_s_[0].url_);

 		if (packet->GetResourceInfo().file_length_ == 0)
 		{
 			RELEASE_LOG("UdpIndexModule::OnAddRidUrl()  File Length is a Zero!");
 			RELEASE_LOG("UdpIndexModule::OnAddRidUrl()  URL = " << packet->GetResourceInfo().url_info_s_[0].url_);
 			is_good_url = false;
 		}

		if (packet->GetResourceInfo().rid_.IsEmpty())
		{
			RELEASE_LOG("UdpIndexModule::OnAddRidUrl()  Bad Packet! Empty rid_");
			is_good_url = false;
		}

        if (is_good_url)
        {
            string str_t = packet->GetResourceInfo().url_info_s_[0].url_;
            string str_r = packet->GetResourceInfo().url_info_s_[0].refer_url_;

            if (str_t.empty() || str_t.find("\r\n") != string::npos)
            {
                RELEASE_LOG("UdpIndexModule::OnAddRidUrl()  Bad Packet! Unsafe url_");
                RELEASE_LOG("URL = " << str_t << "; Length = " << packet->GetResourceInfo().url_info_s_[0].url_.size());
                is_good_url = false;
            }


            if (str_r.find("\r\n") != string::npos)
            {
                RELEASE_LOG("UdpIndexModule::OnAddRidUrl()  Bad Packet! unsafe refer_url_");
                RELEASE_LOG("ReferURL = " << str_r);
                is_good_url = false;
            }


            if (str_t.length() > 1000 || packet->GetResourceInfo().url_info_s_[0].refer_url_.length() > 1000)
            {
                RELEASE_LOG("UdpIndexModule::OnAddRidUrl()  There\'s too long an url or refer url!");
                RELEASE_LOG("UdpIndexModule::OnAddRidUrl()  url = " << str_t);
                is_good_url = false;
            }

            if (boost::algorithm::to_lower_copy(packet->GetResourceInfo().url_info_s_[0].url_).find("start=") != string::npos)
            {
                is_good_url = false;
            }
        }


        if (is_good_url == false  || !BlackList::Inst().IsAllow(packet->GetResourceInfo().url_info_s_[0].refer_url_))
        {
            OnAddRidUrlDenial(end_point, packet);
            return;
        }

		// 直接Post
		AddRidUrlResponsePacket::p response_packet;
		if (waiting_add_url_rid_no_ < max_add_waiting_length_)
		{
			INDEX_EVENT("UdpIndexModule::OnAddRidUrl() 添加到数据库");

			add_ridurl_times_++;

			DataManager::p database_1 = DataManager::Create();

			NeedToDoAddUrlRid();

			DataBaseThreadPoolAdd::IOS().post(
					boost::bind(&DataManager::AddUrl, database_1, packet)
					);

			DataManager::p database_2 = DataManager::Create();

			DataBaseThreadPoolAdd::IOS().post(
					boost::bind(&DataManager::AddCtt, database_2, packet)
					);

			DataManager::p database_3 = DataManager::Create();

			DataBaseThreadPoolAdd::IOS().post(
					boost::bind(&DataManager::AddRid, database_3, packet)
					);

			response_packet = AddRidUrlResponsePacket::CreatePacket(packet->GetTransactionID(), 0, 1);
		}
		else
		{
			RELEASE_LOG("UdpIndexModule::OnAddRidUrl() Lost a Packet");
			response_packet = AddRidUrlResponsePacket::CreatePacket(packet->GetTransactionID(), 3, 1);
			add_busy_times_++;
		}

		statistic_->SubmitAddUrlRidResponse(response_packet->GetBuffer().length_);

		DoSendPacket(end_point, response_packet, packet->GetPeerVersion());

        stringstream ss;
        ss << "1\r\n" << packet->GetResourceInfo().url_info_s_[0].url_ << "\r\n" 
            << packet->GetResourceInfo().url_info_s_[0].refer_url_ << "\r\n" 
            << packet->GetResourceInfo().rid_.ToString();
        StatisticStringPacket::p stat_packet = StatisticStringPacket::CreatePacket(
            packet->GetTransactionID(), packet->GetPeerVersion(), time(NULL),
            packet->GetPeerGUID(), protocal::StatisticRequestPacket::PEER_ADD_RESOURCE, ss.str());
        //DoSendPacket(statistic_endpoint_, stat_packet);
	}

	void  UdpIndexModule::OnVoteRemoveCacheURL(string url)
	{
		if( is_running_ == false ) return;
		INDEX_EVENT("Remove URL from hashmap URL = " << url);

		hash_map<string, UrlHashmapStruct>::iterator u_it = url_hash_map_.find(url);
		if(u_it != url_hash_map_.end())
		{
			multimap<UINT64, string>::iterator um_iter = url_multimap_.find((UINT64)u_it->second.index_.QuadPart);
			while (um_iter != url_multimap_.end() && um_iter->first == (UINT64)u_it->second.index_.QuadPart)
			{
				if (um_iter->second == url)
				{
					url_multimap_.erase(um_iter);
					break;
				}
				um_iter++;
			}
			url_hash_map_.erase(u_it);
		}
	}

	void  UdpIndexModule::OnVoteRemoveCacheContent(MD5 md5, u_int file_lenght)
	{
		if( is_running_ == false ) return;
		INDEX_EVENT("Remove ContentMD5 from hashmap MD5 = " << md5.ToString());

		hash_map<pair_hash, ContentHashmapStruct>::iterator u_it = content_hash_map_.find(make_pair(md5,file_lenght));
		if(u_it != content_hash_map_.end())
		{
			multimap<UINT64, pair_hash>::iterator um_iter = content_multimap_.find((UINT64)u_it->second.index_.QuadPart);
			while (um_iter != content_multimap_.end() && um_iter->first == (UINT64)u_it->second.index_.QuadPart)
			{
				if (um_iter->second == pair_hash(make_pair(md5,file_lenght)))
				{
					content_multimap_.erase(um_iter);
					break;
				}
				um_iter++;
			}
			content_hash_map_.erase(u_it);
		}
	}

	void UdpIndexModule::OnQueryTrackerList(boost::asio::ip::udp::endpoint& end_point, 
		protocal::QueryTrackerListRequestPacket::p packet)
	{		
		if( is_running_ == false ) return;

		statistic_->SubmitQueryTrackerListRequest(packet->GetBuffer().length_);

		QueryTrackerListResponsePacket::p response_packet;
		if (tracker_list_packet_ == QueryTrackerListResponsePacket::p())
		{
			response_packet = QueryTrackerListResponsePacket::CreatePacket(packet->GetTransactionID(),1,0,vector<TRACKER_INFO>());
		}
		else
		{
			response_packet = QueryTrackerListResponsePacket::CreatePacket(packet->GetTransactionID(), 0, tracker_list_packet_->GetTrackerGroupCount(),tracker_list_packet_->GetTrackerInfo());
		}

		// 直接读内存中的TrackerList Buffer返回

		//response_packet->AttachEndPointInfo(packet->GetTransactionID());

		statistic_->SubmitQueryTrackerListResponse(response_packet->GetBuffer().length_);

		INDEX_EVENT("UdpIndexModule::OnQueryTrackerList() 返回TrackerList");
		DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
	}

	void UdpIndexModule::OnQueryStunServerList(boost::asio::ip::udp::endpoint& end_point, 
		QueryStunServerListRequestPacket::p packet)
	{
		if( is_running_ == false ) return;

		statistic_->SubmitQueryStunListRequest(packet->GetBuffer().length_);

		QueryStunServerListResponsePacket::p response_packet;
		if (stun_list_packet_ == QueryStunServerListResponsePacket::p())
		{
			response_packet = QueryStunServerListResponsePacket::CreatePacket(packet->GetTransactionID(),1,vector<STUN_SERVER_INFO>());
		}
		else
		{
			response_packet = QueryStunServerListResponsePacket::CreatePacket(packet->GetTransactionID(),0,stun_list_packet_->GetStunServerInfo());
		}

		//response_packet->AttachEndPointInfo(packet->GetTransactionID());

		statistic_->SubmitQueryStunListResponse(response_packet->GetBuffer().length_);

		INDEX_EVENT("UdpIndexModule::OnQueryStunServerList() 返回StunServerList");
		DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
	}

	void UdpIndexModule::OnQueryIndexServerList(boost::asio::ip::udp::endpoint& end_point, 
		QueryIndexServerListRequestPacket::p packet)
	{
		if( is_running_ == false ) return;

		statistic_->SubmitQueryIndexListRequest(packet->GetBuffer().length_);

		QueryIndexServerListResponsePacket::p response_packet;
		if (index_list_packet_ == QueryIndexServerListResponsePacket::p())
		{
			response_packet = QueryIndexServerListResponsePacket::CreateErrorPacket(packet->GetTransactionID(),1);
		}
		else
		{
			response_packet = QueryIndexServerListResponsePacket::CreatePacket(packet->GetTransactionID(), index_list_packet_->GetModIndexMap(), index_list_packet_->GetIndexServers());
		}

		//response_packet->AttachEndPointInfo(packet->GetTransactionID());

		statistic_->SubmitQueryIndexListResponse(response_packet->GetBuffer().length_);

		INDEX_EVENT("UdpIndexModule::OnQueryStunServerList() 返回StunServerList");
		DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
	}

	void UdpIndexModule::OnSQLQueryRidByUrl(boost::asio::ip::udp::endpoint& end_point, 
		protocal::QueryRidByUrlRequestPacket::p request_packet, 
		protocal::QueryRidByUrlResponsePacket::p resposne_packet)
	{		
		if( is_running_ == false ) return;

		url_sql_succed_query_times_ ++;

		QueryRidByUrlResponsePacket::p send_packet = 
			QueryRidByUrlResponsePacket::ParseFromBinary(resposne_packet->GetBuffer());
		if(!send_packet)
		{
			assert(0);
		}
		
		u_long ep_ip;
		u_short ep_port;
		framework::network::EndpointToIpPort(end_point, ep_ip, ep_port);

		send_packet->AttatchEndPointInfo(request_packet->GetTransactionID(),request_packet->GetSessionID(),
			ep_ip, ep_port);
		INDEX_EVENT("UdpIndexModule::OnSQLQueryRidByUrl() 返回结果");
		//CONSOLE_LOG(send_packet->GetResourceID());

		if (resposne_packet->GetFileLength() >= min_file_length_)
		{
			statistic_->SubmitQueryRidByUrlResponse(send_packet->GetBuffer().length_);			
			DoSendPacket(end_point, send_packet, request_packet->GetPeerVersion());
			//CONSOLE_LOG("Response URL = " << request_packet->GetUrlString());
		}
		else
		{
			INDEX_EVENT("UdpIndexModule::OnSQLQueryRidByUrl() Too Little a File Length!");

            OnQueryRidByUrlDenial(end_point, request_packet);

			url_sql_file_too_small_times_++;
		}

        stringstream ss;
        ss << "1\r\n" << request_packet->GetUrlString() << "\r\n" 
            << request_packet->GetReferString() << "\r\n" 
            << resposne_packet->GetResourceID().ToString();
        StatisticStringPacket::p stat_packet = StatisticStringPacket::CreatePacket(
            request_packet->GetTransactionID(), request_packet->GetPeerVersion(), time(NULL),
            request_packet->GetPeerGuid(), protocal::StatisticRequestPacket::PEER_QUERY_URL_SUCCESS,
            ss.str());
        //DoSendPacket(statistic_endpoint_, stat_packet);

		//首先 拼包(Buffer和 Buffer_Send) 发送到相应的endpoint

		LARGE_INTEGER l_i;

		string url_tmp = Url2Mini(request_packet->GetUrlString(),request_packet->GetReferString());
		hash_map<string, UrlHashmapStruct>::iterator it = url_hash_map_.find(url_tmp);
		if (it != url_hash_map_.end())
		{
			//如该url对应记录在hashmap中已经存在：
			//在Multimap中找到该记录  用QueryPerformanceCounter()改变index值. 并在hashmap中更新index值
			INDEX_EVENT("url在hashmap中已经存在 url = " << url_tmp);
			INDEX_EVENT("更新访问时间");

			multimap<UINT64, string>::iterator mit = url_multimap_.find((UINT64)it->second.index_.QuadPart);
			while (mit != url_multimap_.end() && mit->first == (UINT64)it->second.index_.QuadPart)
			{
				if (mit->second == url_tmp)
				{
					QueryPerformanceCounter(&l_i);
					it->second.index_ = l_i;
					url_multimap_.erase(mit);
					url_multimap_.insert(make_pair((UINT64)l_i.QuadPart, url_tmp));
					break;
				}
				mit++;
			}
		}
		else
		{
			// 如果不存在
			// QueryPerformanceCounter()生成index  加入hashmap与multimap中

			INDEX_EVENT("url在hashmap中不存在 url = " << url_tmp);
			INDEX_EVENT("加入哈希表中");

			UrlHashmapStruct uhs;
			uhs.packet = resposne_packet;
			QueryPerformanceCounter(&l_i);
			uhs.index_ = l_i;
			url_hash_map_.insert(make_pair(url_tmp,uhs));
			url_multimap_.insert(make_pair((UINT64)l_i.QuadPart, url_tmp));
		}

		multimap<UINT64, string>::iterator iter = url_multimap_.begin();
		
		while (url_hash_map_.size() > max_url_cache_size_)
		{
			//淘汰Cache中长时间没有访问的数据

			INDEX_EVENT("UdpIndexModule::OnSQLQueryRidByUrl() URL Hashmap中删除");
			INDEX_EVENT("UdpIndexModule::OnSQLQueryRidByUrl() 删除 URL = " << iter->second);

			url_hash_map_.erase(url_hash_map_.find(iter->second));
			url_multimap_.erase(iter++);
		}
	}

	void UdpIndexModule::OnSQLQueryRidByContent(boost::asio::ip::udp::endpoint& end_point, 
		protocal::QueryRidByContentRequestPacket::p request_packet, 
		protocal::QueryRidByContentResponsePacket::p resposne_packet)
	{		
		if( is_running_ == false ) return;

		content_sql_succed_query_times_ ++;

		QueryRidByContentResponsePacket::p send_packet = 
			QueryRidByContentResponsePacket::ParseFromBinary(resposne_packet->GetBuffer());
		if(!send_packet)
		{
			assert(0);
		}

		u_long ep_ip;
		u_short ep_port;
		framework::network::EndpointToIpPort(end_point, ep_ip, ep_port);

		send_packet->AttatchEndPointInfo(request_packet->GetTransactionID(),0,
			ep_ip, ep_port);
		INDEX_EVENT("UdpIndexModule::OnSQLQueryRidByContent() 返回结果");
		//CONSOLE_LOG(send_packet->GetResourceID());

		if (resposne_packet->GetFileLength() >= min_file_length_)
		{
			statistic_->SubmitQueryRidByContentResponse(send_packet->GetBuffer().length_);
			DoSendPacket(end_point, send_packet, request_packet->GetPeerVersion());
		}
		else
		{
			INDEX_EVENT("UdpIndexModule::OnSQLQueryRidByContent() Too Little a File Length!");
            OnQueryRidByCttDenial(end_point, request_packet);
			content_sql_file_too_small_times_++;
		}


		//首先 拼包(Buffer和 Buffer_Send) 发送到相应的endpoint

		LARGE_INTEGER l_i;

		hash_map<pair_hash, ContentHashmapStruct>::iterator it = 
			content_hash_map_.find(make_pair(request_packet->GetContentSenseMD5(),request_packet->GetFileLength()));
		if (it != content_hash_map_.end())
		{
			//如该Content对应记录在hashmap中已经存在：
			//在Multimap中找到该记录  用QueryPerformanceCounter()改变index值. 并在hashmap中更新index值
			INDEX_EVENT("Content在hashmap中已经存在 Content = " << request_packet->GetContentSenseMD5().ToString());
			INDEX_EVENT("更新访问时间");

			multimap<UINT64, pair_hash>::iterator mit = content_multimap_.find((UINT64)it->second.index_.QuadPart);
			while (mit != content_multimap_.end() && mit->first == (UINT64)it->second.index_.QuadPart)
			{
				if (mit->second == make_pair(request_packet->GetContentSenseMD5(),request_packet->GetFileLength()))
				{
					QueryPerformanceCounter(&l_i);
					it->second.index_ = l_i;
					content_multimap_.erase(mit);
					content_multimap_.insert(make_pair((UINT64)l_i.QuadPart, make_pair(request_packet->GetContentSenseMD5(),request_packet->GetFileLength())));
					break;
				}
				mit++;
			}	
		}
		else
		{
			// 如果不存在
			// QueryPerformanceCounter()生成index  加入hashmap与multimap中

			INDEX_EVENT("Content在hashmap中不存在 Content = " << request_packet->GetContentSenseMD5());
			INDEX_EVENT("加入哈希表中");

			ContentHashmapStruct uhs;
			uhs.packet = resposne_packet;
			QueryPerformanceCounter(&l_i);
			uhs.index_ = l_i;
			content_hash_map_.insert(make_pair(make_pair(request_packet->GetContentSenseMD5(),request_packet->GetFileLength()),uhs));
			content_multimap_.insert(make_pair((UINT64)l_i.QuadPart, make_pair(request_packet->GetContentSenseMD5(),request_packet->GetFileLength())));
		}

		multimap<UINT64, pair_hash>::iterator iter = content_multimap_.begin();

		while (content_hash_map_.size() > max_content_cache_size_)
		{
			//淘汰Cache中长时间没有访问的数据

			INDEX_EVENT("UdpIndexModule::OnSQLQueryRidByContent() Content Hashmap中删除");
			INDEX_EVENT("UdpIndexModule::OnSQLQueryRidByContent() 删除 Content = " << iter->second.pr_.first.ToString());

			content_hash_map_.erase(content_hash_map_.find(iter->second));
			content_multimap_.erase(iter++);
		}      
	}

	void UdpIndexModule::OnQueryRidByUrlNoFound(boost::asio::ip::udp::endpoint& end_point, 
		QueryRidByUrlRequestPacket::p packet)
	{
		if( is_running_ == false ) return;
		ResponseServerPacket::p response_packet = ResponseServerPacket::CreatePacket(
			QueryRidByUrlResponsePacket::ACTION, packet->GetTransactionID(), 1);

		if (!response_packet)
		{
			assert(0);
		}
		INDEX_EVENT("UdpIndexModule::OnQueryRidByUrlNoFound() 没查到");
		DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
		statistic_->SubmitQueryRidByUrlResponse(response_packet->GetBuffer().length_);
		//CONSOLE_LOG("NO Found URL = " << packet->GetUrlString());
		url_sql_faild_query_times_++;
	}

	void UdpIndexModule::OnQueryRidByContentNoFound(boost::asio::ip::udp::endpoint& end_point, 
		QueryRidByContentRequestPacket::p packet)
	{
		if( is_running_ == false ) return;
		ResponseServerPacket::p response_packet = ResponseServerPacket::CreatePacket(
			QueryRidByContentResponsePacket::ACTION, packet->GetTransactionID(), 1);

		if (!response_packet)
		{
			assert(0);
		}
		INDEX_EVENT("UdpIndexModule::OnQueryRidByContentNoFound() 没查到");
		DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
		statistic_->SubmitQueryRidByContentResponse(response_packet->GetBuffer().length_);
		content_sql_faild_query_times_++;
	}

	void UdpIndexModule::OnQueryRidByContentClash(boost::asio::ip::udp::endpoint& end_point, 
		QueryRidByContentRequestPacket::p packet)
	{
		if( is_running_ == false ) return;
		ResponseServerPacket::p response_packet = ResponseServerPacket::CreatePacket(
			QueryRidByContentResponsePacket::ACTION, packet->GetTransactionID(), 2);

		if (!response_packet)
		{
			assert(0);
		}
		INDEX_EVENT("UdpIndexModule::OnQueryRidByContentClash() 冲突");
		DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
		statistic_->SubmitQueryRidByContentResponse(response_packet->GetBuffer().length_);
		content_sql_faild_query_times_++;
	}

    void UdpIndexModule::OnQueryRidByUrlDenial(boost::asio::ip::udp::endpoint& end_point, 
        QueryRidByUrlRequestPacket::p packet)
    {
        if( is_running_ == false ) return;
        ResponseServerPacket::p response_packet = ResponseServerPacket::CreatePacket(
            QueryRidByUrlResponsePacket::ACTION, packet->GetTransactionID(), 4);

        if (!response_packet)
        {
            assert(0);
        }
        INDEX_EVENT("UdpIndexModule::OnQueryRidByUrlDenial() 拒绝查询");
        DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
        statistic_->SubmitQueryRidByUrlResponse(response_packet->GetBuffer().length_);
        url_sql_faild_query_times_++;
    }

    void UdpIndexModule::OnQueryRidByCttDenial(boost::asio::ip::udp::endpoint& end_point, 
        QueryRidByContentRequestPacket::p packet)
    {
        if( is_running_ == false ) return;
        ResponseServerPacket::p response_packet = ResponseServerPacket::CreatePacket(
            QueryRidByContentResponsePacket::ACTION, packet->GetTransactionID(), 4);

        if (!response_packet)
        {
            assert(0);
        }
        INDEX_EVENT("UdpIndexModule::OnQueryRidByUrlDenial() 拒绝查询");
        DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
        statistic_->SubmitQueryRidByUrlResponse(response_packet->GetBuffer().length_);
        content_sql_faild_query_times_++;
    }

    void UdpIndexModule::OnAddRidUrlDenial(boost::asio::ip::udp::endpoint& end_point, 
        AddRidUrlRequestPacket::p packet)
    {
        if( is_running_ == false ) return;
        ResponseServerPacket::p response_packet = ResponseServerPacket::CreatePacket(
            AddRidUrlResponsePacket::ACTION, packet->GetTransactionID(), 4);

        if (!response_packet)
        {
            assert(0);
        }
        INDEX_EVENT("UdpIndexModule::OnAddRidUrlDenial() 拒绝插入");
        DoSendPacket(end_point, response_packet, packet->GetPeerVersion());
        statistic_->SubmitAddUrlRidResponse(response_packet->GetBuffer().length_);
    }

	void UdpIndexModule::PrintCache(int op, string str, u_int int_value)
	{
		if( is_running_ == false ) return;
		switch(op)
		{
			case 1://Content XXXX xxxx
			{
				MD5 content_tmp;
				content_tmp.Parse(str);
 				hash_map<pair_hash,ContentHashmapStruct>::iterator it = 
					content_hash_map_.find(make_pair(content_tmp,int_value));
 				if(it != content_hash_map_.end())
 				{
					CONSOLE_OUTPUT("Content: " << it->first.pr_.first.ToString());
					CONSOLE_OUTPUT("FileLength: " << it->first.pr_.second);
 					CONSOLE_OUTPUT("QPC: " << (INT64)it->second.index_.QuadPart);
 				}
 				else
				{
 					CONSOLE_OUTPUT("Content No Found!");
 				}
				break;
			}
			case 2://URL XXXX
			{
				string url_tmp = str;
				hash_map<string, UrlHashmapStruct>::iterator it = url_hash_map_.find(url_tmp);
				if(it != url_hash_map_.end())
				{
					CONSOLE_OUTPUT("RID: " << it->second.packet->GetResourceID().ToString());
					CONSOLE_OUTPUT("QPC: " << (INT64)it->second.index_.QuadPart);
				}
				else
				{
					CONSOLE_OUTPUT("URL No Found!");
				}
				break;
			}
			case 3://LIST Content QPC
			{
 				multimap<UINT64, pair_hash>::iterator mit = content_multimap_.begin();
 				for (size_t i = 0; i < 5 && i < content_multimap_.size(); i++)
 				{
 					CONSOLE_OUTPUT(i << ": " << mit->second.pr_.first.ToString() << "-" << mit->second.pr_.second << "\t" << mit->first);
 					mit++;
 				}
 				if (content_multimap_.empty())
 				{
 					CONSOLE_OUTPUT("The Content Cache is Empty!");
 				}
 				else
				{
					CONSOLE_OUTPUT("------------------------------");
 				}
				mit = content_multimap_.end();
 				for (int i = content_multimap_.size() - 1; i >= 0 && content_hash_map_.size() - i <= 5; i --)
				{
 					mit--;
					CONSOLE_OUTPUT(i << ": " << mit->second.pr_.first.ToString() << "-" << mit->second.pr_.second << "\t" << mit->first);
 				}
				break;
			}
			case 4://LIST Content Total
			{
 				multimap<UINT64, pair_hash>::iterator mit = content_multimap_.begin();
				int i = 0;
				while (mit != content_multimap_.end())
				{
					CONSOLE_OUTPUT(i << ": " << mit->second.pr_.first.ToString() << "-" << mit->second.pr_.second << "\t" << mit->first);
					mit++;
					i++;
 				}
 				CONSOLE_OUTPUT("Content Count: " << content_multimap_.size());
				break;
			}
			case 5://LIST URL QPC
			{
				multimap<UINT64, string>::iterator mit = url_multimap_.begin();
				for (size_t i = 0; i < 5 && i < url_multimap_.size(); i++)
				{
					CONSOLE_OUTPUT(i << ": " << mit->second  << "\t" << mit->first);
					mit++;
				}
				if (url_multimap_.empty())
				{
					CONSOLE_OUTPUT("The URL Cache is Empty!");
				}
				else
				{
					CONSOLE_OUTPUT("------------------------------");
				}
				mit = url_multimap_.end();
				for (int i = url_multimap_.size() - 1; i >= 0 && url_hash_map_.size() - i <= 5; i --)
				{
					mit--;
					CONSOLE_OUTPUT(i << ": " << mit->second  << "\t" << mit->first);
				}
				break;
			}
			case 6://LIST URL Total
			{
				multimap<UINT64, string>::iterator mit = url_multimap_.begin();
				int i = 0;
				while (mit != url_multimap_.end())
				{
					CONSOLE_OUTPUT(i << ": " << mit->second);
					mit++;
					i++;
				}
				CONSOLE_OUTPUT("URL Count: " << url_multimap_.size());
				break;
			}
			case 7://Status
			{
				bool is_timer = (str == "timer");

				if (str == "cache" || str == "timer")
				{
					OutputInfo("==================================================================", is_timer);
					CONSOLE_LOG("");
					if (is_timer)
					{
						STATISTIC_LOG("");
					}
					OutputInfo("本次统计持续时间             : "<<url_cache_timer_.GetElapsed()/1000/60/60<<":"<<url_cache_timer_.GetElapsed()/1000/60%60<<":"<<url_cache_timer_.GetElapsed()/1000%60, is_timer);
					OutputInfo("--------------------------- ADD STATUS ---------------------------", is_timer);
					OutputInfo("add_ridurl_times_            = "<< add_ridurl_times_, is_timer);
					OutputInfo("add_busy_times_              = "<< add_busy_times_, is_timer);
					OutputInfo("-------------------------URL CACHE STATUS-------------------------", is_timer);
					OutputInfo("url_sql_file_too_small_times_= "<<url_sql_file_too_small_times_, is_timer);
					OutputInfo("url_cache_attach_times_      = "<<url_cache_attach_times_, is_timer);
					OutputInfo("url_sql_faild_query_times_   = "<<url_sql_faild_query_times_, is_timer);
					OutputInfo("url_sql_succed_query_times_  = "<<url_sql_succed_query_times_, is_timer);
					OutputInfo("url_cache_query_times_       = "<<url_cache_query_times_, is_timer);
					OutputInfo("url_busy_times_              = "<< url_busy_times_, is_timer);
					if (url_cache_query_times_ != 0)
					{
						OutputInfo("url cache attach rate:         "<<(0.0+url_cache_attach_times_) / url_cache_query_times_, is_timer);
					}
					OutputInfo("url_multimap_.size(): "<<url_multimap_.size(), is_timer);
					OutputInfo("url_hash_map_.size(): "<<url_hash_map_.size(), is_timer);
					OutputInfo("url cache max size  : "<<max_url_cache_size_, is_timer);
 					OutputInfo("-----------------------Content CACHE STATUS------------------------", is_timer);
 					OutputInfo("content_cache_attach_times_      = "<<content_cache_attach_times_, is_timer);
 					OutputInfo("content_sql_faild_query_times_   = "<<content_sql_faild_query_times_, is_timer);
 					OutputInfo("content_sql_succed_query_times_  = "<<content_sql_succed_query_times_, is_timer);
 					OutputInfo("content_cache_query_times_       = "<<content_cache_query_times_, is_timer);
					OutputInfo("content_sql_file_too_small_times_= "<<content_sql_file_too_small_times_, is_timer);
					OutputInfo("content_busy_times_              = "<< content_busy_times_, is_timer);
					if (content_cache_query_times_ != 0)
 					{
 						OutputInfo("content cache attach rate:         "<<(0.0+content_cache_attach_times_) / content_cache_query_times_, is_timer);
 					}
					OutputInfo("content_multimap_.size(): "<<content_multimap_.size(), is_timer);
					OutputInfo("content_hash_map_.size(): "<<content_hash_map_.size(), is_timer);
					OutputInfo("content cache max size  : "<<max_content_cache_size_, is_timer);
					OutputInfo("-----------------------------SQL STATUS---------------------------", is_timer);
					OutputInfo("数据库写入队列等待长度：waiting_add_url_rid_no_          = "<<waiting_add_url_rid_no_, is_timer);
					OutputInfo("数据库查询队列等待长度：waiting_query_rid_by_content_no_ = "<<waiting_query_rid_by_content_no_, is_timer);
					OutputInfo("数据库查询队列等待长度：waiting_query_rid_by_url_no_     = "<<waiting_query_rid_by_url_no_, is_timer);
					OutputInfo("-------------------------------------------------------------------", is_timer);
				}
				if (str == "net" || str == "timer")
				{
					pair<string, string> str_fmt;
					string str_hd = boost::str(boost::format("%9s%10s%10s%10s%10s%10s") 
						%"次数" %"字节数" %"频率" %"速度" %"瞬时频率" %"瞬时速度");

					OutputInfo("----------------------------Net Status----------------------------", is_timer);
					CONSOLE_LOG("");
					if (is_timer)
					{
						STATISTIC_LOG("");
					}
					OutputInfo("操作            " << str_hd ,is_timer);

					IndexStatisticInfo statistic_info = statistic_->TotalInfo();					
					str_fmt = FormatOutputStr(statistic_info);
					OutputInfo("AllRequest      " << str_fmt.first ,is_timer);
					OutputInfo("AllResponse     " << str_fmt.second ,is_timer);

					statistic_info = statistic_->QueryRidByUrlInfo();
					str_fmt = FormatOutputStr(statistic_info);
					OutputInfo("RidByUrlRequest " << str_fmt.first ,is_timer);
					OutputInfo("RidByUrlResponse" << str_fmt.second ,is_timer);

 					statistic_info = statistic_->QueryRidByContentInfo();
 					str_fmt = FormatOutputStr(statistic_info);
 					OutputInfo("RidByContentRqst" << str_fmt.first ,is_timer);
 					OutputInfo("RidByContentRsps" << str_fmt.second ,is_timer);

					statistic_info = statistic_->AddUrlRidInfo();
					str_fmt = FormatOutputStr(statistic_info);
					OutputInfo("AddRequest      " << str_fmt.first ,is_timer);
					OutputInfo("AddResponse     " << str_fmt.second ,is_timer);
					OutputInfo("-------------------------------------------------------------------", is_timer);
				}
				if (str == "database" || str == "timer")
				{
					DataBase::p database_ = DataBase::Create();

					DataBaseThreadPoolQuery::IOS().post(boost::bind(&DataBase::DatasInfo, database_, database_, is_timer));
				}
				break;
			}
		}
	}

	pair<string, string> UdpIndexModule::FormatOutputStr(IndexStatisticInfo statistic_info)
	{
		string str_rq = boost::str(boost::format("%9u %9u %9f %9f %9f %9f")
			%statistic_info.request_count_
			%statistic_info.request_bytes_
			%statistic_info.request_frequency_
			%statistic_info.request_speed_
			%statistic_info.recent_minute_request_frequency_
			%statistic_info.recent_minute_request_speed_);
		string str_rp = boost::str(boost::format("%9u %9u %9f %9f %9f %9f")
			%statistic_info.response_count_
			%statistic_info.response_bytes_
			%statistic_info.response_frequency_
			%statistic_info.response_speed_
			%statistic_info.recent_minute_response_frequency_
			%statistic_info.recent_minute_response_speed_);
		return make_pair(str_rq, str_rp);
	}

    void UdpIndexModule::OnVisitKey(boost::asio::ip::udp::endpoint &end_point, protocal::StatisticRequestPacket::PEER_ACTION peer_action, Guid peer_guid, size_t peer_vertion, size_t tran_id, string key)
    {
        INDEX_EVENT("UdpIndexModule()::OnStatisticURL");
        StatisticIndexUrlPacket::p send_packet = StatisticIndexUrlPacket::CreatePacket(
            tran_id, peer_vertion, time(NULL), peer_guid, peer_action, key);
        if (!send_packet)
        {
            RELEASE_LOG("UdpIndexModule::OnVisitKey() Failed to Create Packet");
            INDEX_EVENT("UdpIndexModule::OnVisitKey() Failed to Create Packet");
            return;
        }
        INDEX_EVENT("UdpIndexModule::OnVisitKey() " << peer_action);
        DoSendPacket(end_point, send_packet, peer_action);
    }

	void UdpIndexModule::OnPrintDatabaseInfo(map<size_t,size_t> vtct, bool is_timer)
	{
		OutputInfo("---------------------------------DatabaseInfo------------------------------",is_timer);
        OutputInfo("\n\tDBNum\t\tDataCount",is_timer);
        for (map<size_t,size_t>::iterator it = vtct.begin(); it != vtct.end(); it++)
		{
            OutputInfo("\t" << it->first << "\t\t" << it->second, is_timer);
		}
		OutputInfo("\n-------------------------------------------------------------------------",is_timer);
		OutputInfo("\n=========================================================================",is_timer);
	}
}