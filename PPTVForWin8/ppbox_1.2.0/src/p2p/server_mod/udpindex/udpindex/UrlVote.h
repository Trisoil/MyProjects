#pragma once
//用于缓存数据库插入的Cache
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include "base/guid.h"
#include "UdpIndexModule.h"
#include "Lock.h"
#include <map>
using namespace std;

namespace udpindex
{
	class UrlVote
		:public boost::enable_shared_from_this<UrlVote>
		,public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<UrlVote> p;
		static UrlVote &Inst(){return *inst_;};

	public:
        bool VoteUrl(string url, string rid_str);

        void ClearUp();

        string GetVoteInfo();

	public:
		~UrlVote(){is_running_=false;}

	private:
		UrlVote():is_running_(true){}

	private:
		static p inst_;

	private:
		bool is_running_;
        CriticalSection critical_section_;

        const static size_t max_vote_count_ = 5;

        map<pair<string/*URL*/, string/*RID*/>, size_t/*VoteCount*/> vote_map_;
	};

}