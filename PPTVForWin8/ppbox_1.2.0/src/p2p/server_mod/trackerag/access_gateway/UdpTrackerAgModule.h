//#pragma once
#ifndef UDPTRACKERAGMODULE_H_CK_20111122
#define UDPTRACKERAGMODULE_H_CK_20111122

#include "Common.h"
#include "framework/configure/Config.h"
#include "framework/timer/Timer.h"
#include "framework/timer/AsioTimerManager.h"

//#include "UdpServer.h"
#include "protocol/UdpServer.h"

#include "protocol/TrackerPacket.h"
#include "protocol/Packet.h"
#include "TrackerRequestHandler.h"
#include "ListRequestCache.h"
#include "RidListCache.h"
#include "access_gateway/TrackeragStatistic.h"


//#include "Timer.h"
using namespace std;
extern log4cplus::Logger g_logger;
extern log4cplus::Logger g_stat_logger;
using namespace udptrackerag;

namespace udptrackerag
{
    enum CheckResult
    {
        PEERID_CHECK_FAILED = -1,
        FREQUENT_CHECK_FAILED = -2,
        RESOURCEID_CHECK_FAILED = -3,
    };

    class UdpTrackerAgModule 
        : public boost::noncopyable
        , public boost::enable_shared_from_this<UdpTrackerAgModule>
        , public protocol::IUdpServerListener
        //		, public ITimerListener
    {
    public:

        typedef boost::shared_ptr<UdpTrackerAgModule> p;

    public:

        void Start(
            u_short local_udp_port                                // 本地监听UDP端口
            );

        void Stop();

        void InitConfigValues();

        //        void OnUdpRecv(boost::asio::ip::udp::endpoint& end_point, Buffer& buf);
        virtual void OnUdpRecv(protocol::Packet const & packet);

        template<typename T>
        void DoSendPacket(boost::shared_ptr<T> & packet , boost::uint16_t peer_version )
        {
            if( false == is_running_ )
            {
                LOG4CPLUS_WARN(g_logger,"UdpTrackerAgModule::DoSendPacket: TracerModule is not running. Return.");
                return;
            }
            this->udp_server_->send_packet(*packet , peer_version );
        }

        void OnTimerElapsed(const Timer::pointer * pointer);       

    private:
        void DispatchAction(const boost::asio::ip::udp::endpoint& end_point, boost::uint8_t action, ::protocol::ServerPacket const & packet);
        void OnCheckListAlive();
        //void OnDumpInfo();
        void OnReloadConfig();
        void OnRidListCache();
    private:      
        static UdpTrackerAgModule::p s_instance;
        //protocol::UdpServer::pointer udp_server_;
        boost::shared_ptr<protocol::UdpServer> udp_server_;
        volatile bool is_running_;     
        int location_peer_percent_;
        int keep_alive_ms_;
        int check_alive_ms_;
        int udp_server_receive_count_;
        bool random_shuffle_;
        int list_result_timeout_ms_;

    private:        
        framework::timer::PeriodicTimer::pointer check_alive_timer_;
        framework::timer::PeriodicTimer::pointer reload_config_timer_;
        framework::timer::PeriodicTimer::pointer dump_info_timer_;
        framework::timer::PeriodicTimer::pointer rid_list_cache_timer_;

	private:

		TrackeragStatistic::p trackerag_statistic_;
		//framework::timer::PeriodicTimer::pointer statistic_timer_;
		//framework::timer::PeriodicTimer::pointer resource_dump_timer_;

    private:

        UdpTrackerAgModule();

        void out_put_count_stats();       

        int PeerIdCheck(const Guid& peer_id);
        int RidCheck(const RID& rid);       

        template <typename ListPackType>
        void OnRequestComm(const boost::asio::ip::udp::endpoint& end_point,const ListPackType& list_request,unsigned request_ip)
        {
            LOG4CPLUS_DEBUG(g_logger,"UdpTrackerAgModule::OnRequestComm: " << end_point<<" action:"<<int(list_request.PacketAction)<<" end point:"<<end_point);
			trackerag_statistic_->AddCmdTimes(list_request.PacketAction);
			trackerag_statistic_->AddIpTimes(end_point.address().to_string());
			


            if( false == is_running_ )
            { 
                LOG4CPLUS_WARN(g_logger,"UdpTrackerAgModule::OnRequestComm: Tracker Module is not running. Return.");
                return;
            }
			//++list_endpoint_request_[end_point];
            const RID& resource_id = list_request.request.resource_id_;
            const Guid& peer_guid = list_request.request.peer_guid_;

			trackerag_statistic_->AddRidTimes(resource_id);
            int check_ret = 0;
            if (( check_ret =PeerIdCheck(peer_guid)) < 0 || (check_ret = RidCheck(resource_id)) < 0)
            {
                LOG4CPLUS_WARN(g_logger,"check failed,ret is:"<<check_ret<<" version:"<<int(list_request.peer_version_)<<" endpoint:"<<end_point);
                return;
            }   

            //发送数据。
            int send_num = TrackerRequestHandler::Instance()->TrackerListRequest(resource_id,peer_guid,list_request.request.request_peer_count_,list_request,request_ip);
            if(send_num <= 0)
            {
                LOG4CPLUS_WARN(g_logger,"TrackerListRequest send num is"<<send_num<<" rid:"<<resource_id<<" pid:"<<peer_guid<<" count:"<<list_request.request.request_peer_count_);
                return;
            }
            else
            {
                trackerag_statistic_->SubmitTrackerRequest(list_request.length(), send_num);
                LOG4CPLUS_DEBUG(g_logger,"TrackerListRequest send num is"<<send_num<<" rid:"<<resource_id<<" pid:"<<peer_guid<<" count:"<<list_request.request.request_peer_count_);
            }           

            //添加到等待的回复的cache里
            ListRequestCache::Instance()->AddRequest(resource_id,end_point,list_request.PacketAction,list_request.transaction_id_,
                list_request.peer_version_,list_request.request.request_peer_count_,send_num);
           
        }
        
        template <typename ListPackType>
        void OnReponseComm(const boost::asio::ip::udp::endpoint& tracker_end_point,ListPackType &list_response)
        {
            LOG4CPLUS_DEBUG(g_logger,"UdpTrackerAgModule::OnRequestComm: " << tracker_end_point<<" action:"<<int(list_response.PacketAction));

			trackerag_statistic_->AddListEndpointResponseTimes(tracker_end_point);
			if (list_response.response.peer_infos_.size() == 0)
			{
				trackerag_statistic_->AddListEndpointResponse0Times(tracker_end_point);

			}
			trackerag_statistic_->AddCmdTimes(list_response.PacketAction + 0x100);

            if( false == is_running_ )
            { 
                LOG4CPLUS_WARN(g_logger,"UdpTrackerAgModule::OnRequestComm: Tracker Module is not running. Return.");
                return;
            }
            RID resource_id = list_response.response.resource_id_;          
            int check_ret = 0;
            if ((check_ret = RidCheck(resource_id)) < 0)
            {
                LOG4CPLUS_WARN(g_logger,"check failed,ret is:"<<check_ret<<" version:"<<int(list_response.peer_version_)<<" endpoint:"<<tracker_end_point);
                return;
            }   

			trackerag_statistic_->AddTrackerListCountTimes(list_response.response.peer_infos_.size());
            //添加到cache里，如果cache已经收到了足够的数据，就可以返回给客户端了。
            int ret = ListRequestCache::Instance()->AddResponse(resource_id,list_response.transaction_id_,tracker_end_point,list_response.response.peer_infos_);
            
            LOG4CPLUS_DEBUG(g_logger,"add response ret:"<<ret);
            if( 0 > ret)
            {
                //当接收到已经超时的包的时候，就会走到这里。
                LOG4CPLUS_INFO(g_logger,"add response failed,ret:"<<ret);
                //++unexpected_response_;
				trackerag_statistic_->AddUnexpectedResponse();
            }
            else if(0 == ret)
            {
                //没有在等待的了，大部分情况下，走到这里的机会要比走到 0>ret分支的机会要大很多，如果不是这样，那可能是超时设置得太小了
                //可以回复给客户端了
				//trackerag_statistic_->SubmitCommitResponse(list_response.length());
                vector<protocol::CandidatePeerInfo> peer_infos;
                boost::asio::ip::udp::endpoint end_point;
                boost::uint8_t action;
                boost::uint16_t peer_version;
                int get_ret = ListRequestCache::Instance()->GetResponse(peer_infos,end_point,action,peer_version,resource_id,list_response.transaction_id_,location_peer_percent_,random_shuffle_);
                if (get_ret != 0)
                {
                    LOG4CPLUS_ERROR(g_logger,"GetResponse failed,ret:"<<get_ret);
                    return;
                }

                int avg_count = 0;
                int response_count = 0;
                int count_ret = ListRequestCache::Instance()->GetResponseCount(resource_id,list_response.transaction_id_,avg_count,response_count);
                if (count_ret < 0)
                {
                    LOG4CPLUS_ERROR(g_logger,"GetAvgResponseCount failed,ret:"<<count_ret<<" maybe the result not exist");                    
                }
                else
                {
                    LOG4CPLUS_DEBUG(g_logger,"GetAvgResponseCount ret:"<<count_ret<<" avg_count:"<<avg_count<<" response_count:"<<response_count); 
                    if(response_count >= 2)
                    {
                        //这里做了一个假设，至少要有2个结果，才认为是有效的数据。如果只有一个结果，那么说明已经是达到最小请求的request的个数了，没必要再更新了。
                        RidListCache::Instance()->SetListResult(resource_id,avg_count,peer_infos);   
                    }
                }                           

                ListRequestCache::Instance()->RemoveRequestInfo(resource_id,list_response.transaction_id_);

                //如果没有查询到结果，就考虑使用cache的结果，由于cache里会把优先级设置到最低，因此在资源多的时候，不会照成被返回peer的负载问题，
                //而在资源少（也就是设为低优先级也被用）的情况，也是应该返回的。
                //资源多，但是走到这里的原因，很可能是因为到tracker的丢包。
                if(0 == peer_infos.size())
                {   //要返回peer数为0的tracker数            
                    int req_count = RidListCache::Instance()->GetListResult(resource_id,peer_infos);  
                    if(0 != peer_infos.size())
                    {
                        LOG4CPLUS_INFO(g_logger,"use cache2,rid:"<<resource_id<<" peers size:"<<peer_infos.size()<<" reqcount:"<<req_count);
                        //++use_cache_result_;
						trackerag_statistic_->AddUseCacheResult();
                    }
                }
				else
				{
					trackerag_statistic_->AddHitCmdTimes(list_response.PacketAction);
				}

				trackerag_statistic_->AddTotalResult();
                SendResponse(resource_id,peer_infos,end_point,action,list_response.transaction_id_,peer_version);
				trackerag_statistic_->AddListCountTimes(peer_infos.size());
				trackerag_statistic_->AddListResponseNum(response_count);
            }
            else
            {   
                //不做啥事，还有没收到的回复，继续等吧。
            }
        }

        //把回复发送给客户端
        void SendResponse(const RID& resource_id,vector<protocol::CandidatePeerInfo>& peer_infos,boost::asio::ip::udp::endpoint& end_point,
            boost::uint8_t& action,uint32_t transaction_id,uint16_t peer_version);

        unsigned GetUdpServerReceiveCount();

    public:

        static UdpTrackerAgModule::p Instance() 
        { 
            if( !s_instance )
            {
                s_instance.reset(new UdpTrackerAgModule());
            }

            return s_instance; 
        }


        AsioTimerManager * asio_timer_manager_;
        //		boost::asio::io_service ios_;
    };
}

#endif//UDPTRACKERAGMODULE_H_CK_20111122