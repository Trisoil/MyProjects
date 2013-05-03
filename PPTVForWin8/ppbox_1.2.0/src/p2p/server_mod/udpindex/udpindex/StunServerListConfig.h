#pragma once

#include "stdafx.h"
#include "protocal/IndexPacket.h"



namespace udpindex
{
	class StunServerListConfig: public boost::enable_shared_from_this<StunServerListConfig>
		, public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<StunServerListConfig> p;

	public:
		static StunServerListConfig& Inst() { return *inst_; }
		static p Create() { return p(new StunServerListConfig()); }

		void LoadFile(const TCHAR *file_name);

		QueryStunServerListResponsePacket::p GetStunServerListPacket();

	private:
		static p inst_;
		StunServerListConfig(){}
		vector<STUN_SERVER_INFO> stun_info_;
	};
}
