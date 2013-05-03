#include "stdafx.h"
#include "UdpCleanerModule.h"
#include "udp_index_cleaner/LogThread.h"
#include "udp_index_cleaner/MergeThread.h"
#include "udp_index_cleaner/LogManager.h"
#include "udp_index_cleaner/MergeManager.h"
#include <io.h>

namespace udplog
{
	string GetNowTimes()
	{
		const size_t MAX_LOG_HEADER_SIZE = 1024;
		char log_header[MAX_LOG_HEADER_SIZE + 1];
		SYSTEMTIME time;
		::GetLocalTime(&time);

		int outputCount = _snprintf_s(log_header, MAX_LOG_HEADER_SIZE, 
			"%04d-%02d-%02d %02d:%02d:%02d.%03d",
			time.wYear,time.wMonth,time.wDay,
			time.wHour,time.wMinute,time.wSecond, time.wMilliseconds
			);
		return std::string(log_header);
	}

	tstring GetNowTimeForLogT()
	{
		const size_t MAX_LOG_HEADER_SIZE = 1024;
		TCHAR log_header[MAX_LOG_HEADER_SIZE + 1];
		SYSTEMTIME time;
		::GetLocalTime(&time);

		int outputCount = _sntprintf_s(log_header, MAX_LOG_HEADER_SIZE, 
			TEXT("%04d-%02d-%02d %02d-%02d-%02d.%03d"),
			time.wYear,time.wMonth,time.wDay,
			time.wHour,time.wMinute,time.wSecond, time.wMilliseconds
			);
		return tstring(log_header);
	}

    //////////////////////////////////////////////////////////////////////////
    // UdpCleanerModule

    UdpCleanerModule::p UdpCleanerModule::inst_(new UdpCleanerModule());

    void UdpCleanerModule::CheckLog()
    {

        tstring moduleName = _T("index_cleaner_log"); 
        framework::mswin::Module module;
        framework::io::Path config_path;
        module.BuildLocalFilePath(config_path, moduleName.c_str());
        tstring filename = config_path.GetString();
        tstring filename_tmp = filename + _T("_tmp");

        if (!access(w2b(filename).c_str(),0))
        {
            remove(w2b(filename_tmp).c_str());
            rename(w2b(filename).c_str(), w2b(filename_tmp).c_str());
        }
    }

	void UdpCleanerModule::Start(u_short local_udp_port) 
    {
        CONSOLE_LOG("正在启动UdpCleanerModule...");
        if (true == is_running_)return ;

        CheckLog();

		udp_server_ = UdpServer::create(shared_from_this());	//UDP

		if( false == udp_server_->Listen(local_udp_port) )
		{
			CONSOLE_LOG("监听 " << local_udp_port << " 端口失败");
			return;
		}
		CONSOLE_LOG("监听 " << local_udp_port << " 端口成功");
        
		//设定定时器
		check_time_timer_ = framework::timer::PeriodicTimer::create(60*1000, shared_from_this());

        mem_record_ = MemRecord::Create();

        last_clean_time_ = DateNowStr();

        lastwritetime_ = last_clean_time_.second;

        last_clean_time_.first = "";

        last_clean_time_.second = boost::str(boost::format("%02d") %Config::Inst().GetInteger("cleaner.clean_clock",0) );

        LogThread::IOS().post(boost::bind(&LogManager::Start, LogManager::Inst()));
        MergeThread::IOS().post(boost::bind(&MergeManager::Start, MergeManager::Inst()));
		udp_server_->Recv(2000);
		check_time_timer_->Start();
        is_running_ = true;
    }

    void UdpCleanerModule::Stop()
    {
        if (false == is_running_)  return ;
        is_running_ = false;
		check_time_timer_->Stop();
		udp_server_->Close();
        LogThread::IOS().post(boost::bind(&LogManager::Stop, LogManager::Inst()));
        MergeThread::IOS().post(boost::bind(&MergeManager::Stop, MergeManager::Inst()));
		CONSOLE_LOG("UdpCleanerModule成功停止...");
    }


    void UdpCleanerModule::OnUdpRecv(boost::asio::ip::udp::endpoint& end_point, Buffer& buf)
    {
		if (false == is_running_)
		{
			RELEASE_LOG("UdpCleanerModule::OnUdpRecv: Statistic Module is not running. Return.");
			return;
		}

		// 检查校验，如果校验失败，不处理
		if (protocal::Cryptography::Decrypt(buf) == false)
		{
			RELEASE_LOG("UdpCleanerModule::OnUdpRecv::DecryptPacket Failed, from " << end_point);
			return; //(注释此行则关闭checksum)
		}

		// 检查包头部正确， 如果读取包头部失败，不处理
		protocal::Packet::p packet = Packet::ParseFromBinary(buf);
		if( ! packet )
		{
			RELEASE_LOG("UdpCleanerModule::OnUdpRecv::ParsePacket Failed, from " << end_point);
			return;
		}

		if( packet->GetAction() == 0xD2)
		{
            StatisticIndexUrlPacket::p request_packet = StatisticIndexUrlPacket::ParseFromBinary(buf);
            if (!request_packet)
            {
                return;
            }
            OnVisit(end_point, request_packet);
		}
		else
		{
			RELEASE_LOG("Unknown Action：0x" << std::hex << packet->GetAction() << ", from " << end_point);
		}
    }

    void UdpCleanerModule::OnPrintMem()
    {
        map<string, time_t>::iterator itr = mem_record_->record_.begin();
        CONSOLE_OUTPUT("------------------------------------------------------------");
        for (; itr != mem_record_->record_.end(); ++itr)
        {
            CONSOLE_OUTPUT("  " << itr->first << " : " << itr->second);
        }
        CONSOLE_OUTPUT("------------------------------------------------------------");
    }

    void UdpCleanerModule::OnVisit(boost::asio::ip::udp::endpoint& end_point, StatisticIndexUrlPacket::p packet)
    {
        string key_str = packet->GetUrl();
        time_t visit_time = packet->GetTimeStamp();
        //mem_record_->record_.insert(make_pair(key_str, visit_time));
        mem_record_->record_[key_str] = visit_time;
        LogThread::IOS().post(
            boost::bind(&LogManager::OnVisit, LogManager::Inst(), key_str, visit_time)
            );
    }

    void UdpCleanerModule::DoSendPacket(boost::asio::ip::udp::endpoint& end_point, protocal::Packet::p packet)
    {
    }

    void UdpCleanerModule::OnTimerElapsed(framework::timer::Timer::p pointer, u_int times)
    {
		if( false == is_running_ )
		{
			return;
		}
		if (pointer == check_time_timer_)
		{
            pair<string/*date*/,string/*hour*/> clean_time = DateNowStr();
            last_clean_time_.second = boost::str(boost::format("%02d") % (Config::Inst().GetInteger("cleaner.clean_clock",0)));

            if (lastwritetime_ != clean_time.second)
            {
                lastwritetime_ = clean_time.second;
                MemRecord::p mem_record_tmp = mem_record_;
                mem_record_ = MemRecord::Create();
                LogThread::IOS().post(boost::bind(&LogManager::ChangeFile, LogManager::Inst()));
                MergeThread::IOS().post(boost::bind(&MergeManager::OnWrite, MergeManager::Inst(),mem_record_tmp,atoi(lastwritetime_.c_str())));
            }

            if (last_clean_time_.first != clean_time.first && last_clean_time_.second == clean_time.second)
            {
                last_clean_time_.first = clean_time.first;
                MergeThread::IOS().post(boost::bind(&MergeManager::OnMerge, MergeManager::Inst()));                
            }
		} 
    }
}
