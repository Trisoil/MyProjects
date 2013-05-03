#include "StdAfx.h"
#include "DataManager.h"
#include "udpindex/DataBase.h"
#include "protocal/StatisticPacket.h"
#include "udpindex/UrlVote.h"

using namespace protocal;

namespace udpindex
{
	void DataManager::QueryRIDByUrl(boost::asio::ip::udp::endpoint& end_point, QueryRidByUrlRequestPacket::p request_packet)
	{
		//MainThread::IOS().post(
		//	boost::bind(&UdpIndexModule::FinishQueryRidByUrl, UdpIndexModule::Inst())
		//	);

        UdpIndexModule::Inst()->FinishQueryRidByUrl();

		// 根据这个url在数据库中查询到相应的RID

        RID rid_tmp;
		string url_str = Url2Mini(request_packet->GetUrlString(),request_packet->GetReferString());
		u_int mod_value = boost::hash_value(url_str)%256;

		bool is_send_packet_to_log_server_ = Config::Inst().GetInteger("index.ishavelogserver");

		if (is_send_packet_to_log_server_)
		{
			MainThread::IOS().post(
				boost::bind(&UdpIndexModule::OnVisitKey, UdpIndexModule::Inst(),
				DataBaseConnectionPool::Inst().GetStatEndPoint(mod_value),
				protocal::StatisticRequestPacket::PEER_QUERY_URL_SUCCESS, request_packet->GetPeerGuid(),
				request_packet->GetPeerVersion(), request_packet->GetTransactionID(), "U_"+Url2MD5(url_str).ToString()));
		}		

		UrlStruct url_struct;
        url_struct.url_ = request_packet->GetUrlString();
        url_struct.refer_url_ = request_packet->GetReferString();
		DataBase::p database = DataBase::Create();
		if(database->Get(mod_value, url_struct, true) && !url_struct.is_empty())
		{
			rid_tmp = url_struct.rid_;
		}
		else
        {
            url_struct.url_ = request_packet->GetUrlString();
            url_struct.refer_url_ = request_packet->GetReferString();

            if(DataBaseConnectionPool::Inst().IsDBChanged(mod_value) && database->Get(mod_value, url_struct, false) && !url_struct.is_empty())
            {
                rid_tmp = url_struct.rid_;
                database->Set(mod_value, url_struct, true);
            }
            else
            {
                MainThread::IOS().post(
                    boost::bind(&UdpIndexModule::OnQueryRidByUrlNoFound,UdpIndexModule::Inst(),
                    end_point,request_packet)
                    );
                return;
            }
        }

		mod_value = boost::hash_value(rid_tmp)%256;

		if (is_send_packet_to_log_server_)
		{
            MainThread::IOS().post(
                boost::bind(&UdpIndexModule::OnVisitKey, UdpIndexModule::Inst(),
                DataBaseConnectionPool::Inst().GetStatEndPoint(mod_value),
                protocal::StatisticRequestPacket::PEER_QUERY_URL_SUCCESS, request_packet->GetPeerGuid(),
                request_packet->GetPeerVersion(), request_packet->GetTransactionID(), "R_"+rid_tmp.ToString()));

		}

		RidStruct rid_struct;
        rid_struct.rid_ = rid_tmp;
		QueryRidByUrlResponsePacket::p response_packet;

		if(database->Get(mod_value, rid_struct, true) && !rid_struct.is_empty())
		{
			response_packet = QueryRidByUrlResponsePacket::CreatePacket(
				rid_struct.rid_, rid_struct.file_length_, rid_struct.block_size_, rid_struct.block_md5_s_, 
				rid_struct.ctt_md5_, rid_struct.ctt_bytes_);
			if (!response_packet)
			{
				assert(0);
				return;
			}
		}
		else
        {
            rid_struct.rid_ = rid_tmp;
            
            if(DataBaseConnectionPool::Inst().IsDBChanged(mod_value) && database->Get(mod_value, rid_struct, false) && !rid_struct.is_empty())
		    {
			    response_packet = QueryRidByUrlResponsePacket::CreatePacket(
				    rid_struct.rid_, rid_struct.file_length_, rid_struct.block_size_, rid_struct.block_md5_s_, 
				    rid_struct.ctt_md5_, rid_struct.ctt_bytes_);
			    if (!response_packet)
			    {
				    assert(0);
				    return;
			    }			
			    database->Set(mod_value, rid_struct, true);
		    }
		    else
		    {
			    MainThread::IOS().post(
				    boost::bind(&UdpIndexModule::OnQueryRidByUrlNoFound,UdpIndexModule::Inst(),
				    end_point,request_packet)
				    );
			    return;
		    }
        }
		INDEX_EVENT("DataBase:: QueryRIDByUrl() 在数据库中找到");
		MainThread::IOS().post(
			boost::bind(&UdpIndexModule::OnSQLQueryRidByUrl,UdpIndexModule::Inst(),
			end_point,request_packet, response_packet)
			);
	}
	void DataManager::QueryRIDByCtt(boost::asio::ip::udp::endpoint& end_point, QueryRidByContentRequestPacket::p request_packet)
	{
		//MainThread::IOS().post(
		//	boost::bind(&UdpIndexModule::FinishQueryRidByContent, UdpIndexModule::Inst())
		//	);
        UdpIndexModule::Inst()->FinishQueryRidByContent();

		// 根据这个ContentMD5在数据库中查询到相应的RID

		RID rid_tmp;
		u_short is_clash;
        MD5 ctt = request_packet->GetContentSenseMD5();
        size_t ctt_bytes = request_packet->GetContentBytes();
        size_t file_length = request_packet->GetFileLength();
		u_int mod_value = boost::hash_value(ctt)%256;
		bool is_send_packet_to_log_server_ = Config::Inst().GetInteger("index.ishavelogserver");

		if (is_send_packet_to_log_server_)
		{
            MainThread::IOS().post(
                boost::bind(&UdpIndexModule::OnVisitKey, UdpIndexModule::Inst(),
                DataBaseConnectionPool::Inst().GetStatEndPoint(mod_value),
                protocal::StatisticRequestPacket::PEER_QUERY_CONTENT_SUCCESS, request_packet->GetPeerGuid(),
                request_packet->GetPeerVersion(), request_packet->GetTransactionID(), 
                "C_"+Ctt2MD5(ctt, ctt_bytes, file_length).ToString()));
		}
		

		CttStruct ctt_struct;
        ctt_struct.ctt_md5_ = request_packet->GetContentSenseMD5();
        ctt_struct.ctt_bytes_ = request_packet->GetContentBytes();
        ctt_struct.file_length_ = request_packet->GetFileLength();
		DataBase::p database = DataBase::Create();
		if(database->Get(mod_value, ctt_struct, true) && !ctt_struct.is_empty())
		{
			is_clash = ctt_struct.is_clash_;
			rid_tmp = ctt_struct.rid_;
		}
		else
        {
            ctt_struct.ctt_md5_ = request_packet->GetContentSenseMD5();
            ctt_struct.ctt_bytes_ = request_packet->GetContentBytes();
            ctt_struct.file_length_ = request_packet->GetFileLength();
            if(DataBaseConnectionPool::Inst().IsDBChanged(mod_value) && database->Get(mod_value, ctt_struct, false) && !ctt_struct.is_empty())
		    {
			    is_clash = ctt_struct.is_clash_;
			    rid_tmp = ctt_struct.rid_;

			    database->Set(mod_value, ctt_struct, true);
		    }
		    else
		    {
			    MainThread::IOS().post(
				    boost::bind(&UdpIndexModule::OnQueryRidByContentNoFound,UdpIndexModule::Inst(),
				    end_point,request_packet)
				    );
			    return;
		    }
        }

		if (is_clash)
		{
			MainThread::IOS().post(
				boost::bind(&UdpIndexModule::OnQueryRidByContentClash,UdpIndexModule::Inst(),
				end_point,request_packet)
				);
			return;
		}


		mod_value = boost::hash_value(rid_tmp)%256;

		if (is_send_packet_to_log_server_)
		{
            MainThread::IOS().post(
                boost::bind(&UdpIndexModule::OnVisitKey, UdpIndexModule::Inst(),
                DataBaseConnectionPool::Inst().GetStatEndPoint(mod_value),
                protocal::StatisticRequestPacket::PEER_QUERY_CONTENT_SUCCESS, request_packet->GetPeerGuid(),
                request_packet->GetPeerVersion(), request_packet->GetTransactionID(), "R_"+rid_tmp.ToString()));

		}

		RidStruct rid_struct;
        rid_struct.rid_ = rid_tmp;
		QueryRidByContentResponsePacket::p response_packet;

		if(database->Get(mod_value, rid_struct, true) && !rid_struct.is_empty())
		{
			response_packet = QueryRidByContentResponsePacket::CreatePacket(
				rid_struct.rid_, rid_struct.file_length_, rid_struct.block_size_, rid_struct.block_md5_s_, 
				rid_struct.ctt_md5_, rid_struct.ctt_bytes_);
			if (!response_packet)
			{
				assert(0);
				return;
			}
		}
		else 
        {
            rid_struct.rid_ = rid_tmp;
            if(DataBaseConnectionPool::Inst().IsDBChanged(mod_value) && database->Get(mod_value, rid_struct, false) && !rid_struct.is_empty())
		    {
			    response_packet = QueryRidByContentResponsePacket::CreatePacket(
				    rid_struct.rid_, rid_struct.file_length_, rid_struct.block_size_, rid_struct.block_md5_s_, 
				    rid_struct.ctt_md5_, rid_struct.ctt_bytes_);
			    if (!response_packet)
			    {
				    assert(0);
				    return;
			    }			
			    database->Set(mod_value, rid_struct, true);
		    }
		    else
		    {
			    MainThread::IOS().post(
				    boost::bind(&UdpIndexModule::OnQueryRidByContentNoFound,UdpIndexModule::Inst(),
				    end_point,request_packet)
				    );
			    return;
		    }
        }
		INDEX_EVENT("DataBase:: QueryRIDByUrl() 在数据库中找到");
		MainThread::IOS().post(
			boost::bind(&UdpIndexModule::OnSQLQueryRidByContent,UdpIndexModule::Inst(),
			end_point,request_packet, response_packet)
			);
	}

	void DataManager::AddUrl(AddRidUrlRequestPacket::p packet)
	{
		//MainThread::IOS().post(
		//	boost::bind(&UdpIndexModule::FinishAddUrlRid, UdpIndexModule::Inst())
		//	);

        UdpIndexModule::Inst()->FinishAddUrlRid();
		//在数据库中插入RID URL二元组

		INDEX_EVENT("DataBase::AddRidUrl()");

		bool is_send_packet_to_log_server_ = Config::Inst().GetInteger("index.ishavelogserver");

		string mini_url = Url2Mini(packet->GetResourceInfo().url_info_s_[0].url_,packet->GetResourceInfo().url_info_s_[0].refer_url_);

		u_int url_mod = boost::hash_value(mini_url)%256;

		if (is_send_packet_to_log_server_)
		{
            MainThread::IOS().post(
                boost::bind(&UdpIndexModule::OnVisitKey, UdpIndexModule::Inst(),
                DataBaseConnectionPool::Inst().GetStatEndPoint(url_mod),
                protocal::StatisticRequestPacket::PEER_ADD_RESOURCE, packet->GetPeerGUID(),
                packet->GetPeerVersion(), packet->GetTransactionID(), "U_"+Url2MD5(mini_url).ToString()));

		}

		DataBase::p database = DataBase::Create();

		RID rid_tmp;
		UrlStruct url_info, url_struct;
		url_info.url_ = packet->GetResourceInfo().url_info_s_[0].url_;
		url_info.refer_url_ = packet->GetResourceInfo().url_info_s_[0].refer_url_;
		url_info.type_ = packet->GetResourceInfo().url_info_s_[0].type_;
		url_info.rid_ = packet->GetResourceInfo().rid_;

        url_struct.url_ = packet->GetResourceInfo().url_info_s_[0].url_;
        url_struct.refer_url_ = packet->GetResourceInfo().url_info_s_[0].refer_url_;

        if(!database->Get(url_mod, url_struct, true) || url_struct.is_empty())
        {
            database->Set(url_mod, url_info, true);
        }
        else if (url_struct.rid_ != url_info.rid_ && UrlVote::Inst().VoteUrl(Url2MD5(mini_url).ToString(), url_info.rid_.ToString()))
        {
            database->Set(url_mod, url_info, true);
            MainThread::IOS().post(
                boost::bind(&UdpIndexModule::OnVoteRemoveCacheURL, UdpIndexModule::Inst(), mini_url));
        }
	}
	void DataManager::AddRid(AddRidUrlRequestPacket::p packet)
	{
		RID rid = packet->GetResourceInfo().rid_;
		if (rid.IsEmpty()) return;

		u_int rid_mod = boost::hash_value(rid)%256;

		bool is_send_packet_to_log_server_ = Config::Inst().GetInteger("index.ishavelogserver");

		if (is_send_packet_to_log_server_)
		{
            MainThread::IOS().post(
                boost::bind(&UdpIndexModule::OnVisitKey, UdpIndexModule::Inst(),
                DataBaseConnectionPool::Inst().GetStatEndPoint(rid_mod),
                protocal::StatisticRequestPacket::PEER_ADD_RESOURCE, packet->GetPeerGUID(),
                packet->GetPeerVersion(), packet->GetTransactionID(), "R_"+rid.ToString()));
		}

		RidStruct rid_info, rid_struct;
		rid_info.rid_ = packet->GetResourceInfo().rid_;
		rid_info.ctt_md5_ = packet->GetContentMD5();
		rid_info.block_md5_s_ = packet->GetResourceInfo().md5_s_;
		rid_info.block_num_ = packet->GetResourceInfo().block_num_;
		rid_info.block_size_ = packet->GetResourceInfo().block_size_;
		rid_info.file_length_ = packet->GetResourceInfo().file_length_;
		rid_info.ctt_bytes_ = packet->GetContentBytes();

        rid_struct.rid_ = packet->GetResourceInfo().rid_;

		DataBase::p database = DataBase::Create();

		if(!database->Get(rid_mod, rid_struct, true) || rid_struct.is_empty())
		{
			database->Set(rid_mod, rid_info, true);	
		}
		else
		{
			if (rid_struct.ctt_md5_.IsEmpty())
			{
				database->Set(rid_mod, rid_info, true);
			}
		}
	}
	void DataManager::AddCtt(AddRidUrlRequestPacket::p packet)
	{
		if(packet->GetContentMD5().IsEmpty()) return;

        MD5 ctt = packet->GetContentMD5();
        size_t ctt_bytes = packet->GetContentBytes();
        size_t file_length = packet->GetResourceInfo().file_length_;
		u_int ctt_mod = boost::hash_value(ctt)%256;

		bool is_send_packet_to_log_server_ = Config::Inst().GetInteger("index.ishavelogserver");

		if (is_send_packet_to_log_server_)
		{
            MainThread::IOS().post(
                boost::bind(&UdpIndexModule::OnVisitKey, UdpIndexModule::Inst(),
                DataBaseConnectionPool::Inst().GetStatEndPoint(ctt_mod),
                protocal::StatisticRequestPacket::PEER_ADD_RESOURCE, packet->GetPeerGUID(),
                packet->GetPeerVersion(), packet->GetTransactionID(), 
                "C_"+Ctt2MD5(ctt, ctt_bytes, file_length).ToString()));
		}


		CttStruct ctt_info, ctt_struct;
		ctt_info.ctt_md5_ = packet->GetContentMD5();
		ctt_info.file_length_ = packet->GetResourceInfo().file_length_;
		ctt_info.rid_ = packet->GetResourceInfo().rid_;
		ctt_info.ctt_bytes_ = packet->GetContentBytes();
		ctt_info.is_clash_ = 0;

        ctt_struct.ctt_md5_ = packet->GetContentMD5();
        ctt_struct.file_length_ = packet->GetResourceInfo().file_length_;
        ctt_struct.ctt_bytes_ = packet->GetContentBytes();

		DataBase::p database = DataBase::Create();

		if (!database->Get(ctt_mod, ctt_struct, true) || ctt_struct.is_empty())
		{
			database->Set(ctt_mod, ctt_info, true);
		}
		else
		{
			if (ctt_struct.is_clash_) return;
				
			if (ctt_struct.rid_ != ctt_info.rid_)
			{
                ctt_info.is_clash_ = 1;
				database->Set(ctt_mod, ctt_info, true);

				MainThread::IOS().post(
					boost::bind(&UdpIndexModule::OnVoteRemoveCacheContent, UdpIndexModule::Inst(),
					packet->GetContentMD5(), packet->GetResourceInfo().file_length_)
					);
			}
		}
	}
}
