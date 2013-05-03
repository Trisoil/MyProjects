#pragma once

#include "stdafx.h"
#include "protocal/IndexPacket.h"



namespace udpindex
{
	class TrackerListConfig: public boost::enable_shared_from_this<TrackerListConfig>
		, public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<TrackerListConfig> p;

	public:
		static TrackerListConfig& Inst() { return *inst_; }
		static p Create() { return p(new TrackerListConfig()); }
		
		void LoadFile(const TCHAR *file_name);

		QueryTrackerListResponsePacket::p GetTrackerListPacket();

	private:
		static p inst_;
		TrackerListConfig(){}
		UINT16 group_count;
		vector<TRACKER_INFO> tracker_info_;
	};
}
