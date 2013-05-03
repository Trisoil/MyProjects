#include "StdAfx.h"
#include "UrlVote.h"
#include "Config.h"
#include "IndexFunc.h"
#include "udpindex/Lock.h"

namespace udpindex
{
	UrlVote::p UrlVote::inst_ = UrlVote::p(new UrlVote());

    bool UrlVote::VoteUrl(string url, string rid_str)
    {
        Lock lock(&critical_section_);
        if (!is_running_)return false;

        map<pair<string,string>, size_t>::iterator it = vote_map_.find(make_pair(url,rid_str));
        if (it != vote_map_.end())
        {
            if (it->second < max_vote_count_)
            {
                it->second++;
            }
            else
            {
                vote_map_.erase(it);
                return true;
            }
        }
        else
        {
            vote_map_.insert(make_pair(make_pair(url,rid_str),1));
        }

        return false;
    }

    string UrlVote::GetVoteInfo()
    {
        Lock lock(&critical_section_);
        if (!is_running_)return "";
        stringstream ss;
        map<pair<string,string>, size_t>::iterator it = vote_map_.begin();
        for (; it != vote_map_.end(); ++it)
        {
            ss << "(" << it->first.first << "," << it->first.second << ") -> " << it->second << endl;
        }
        return ss.str();
    }

    void UrlVote::ClearUp()
    {
        Lock lock(&critical_section_);
        vote_map_.clear();
    }
}