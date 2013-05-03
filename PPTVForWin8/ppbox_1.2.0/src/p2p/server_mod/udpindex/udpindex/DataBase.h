#pragma once

#include "framework/MainThread.h"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include "udpindex/UdpIndexModule.h"
#include "udpindex/DataBaseConnectionPool.h"

#include "udpindex/TokyoTyrantClient.h"
#include "framework/util/strings.h"

namespace udpindex
{
    struct CttStruct //Content信息结构
    {
        MD5 ctt_md5_;
        size_t file_length_;
        RID rid_;
        size_t ctt_bytes_;
        u_short is_clash_;

        CttStruct():ctt_md5_(),file_length_(0),rid_(),ctt_bytes_(0),is_clash_(0){}

        CttStruct(string cttstr)
            :ctt_md5_(),file_length_(0),rid_(),ctt_bytes_(0),is_clash_(0)
        //Version\r\nCtt\r\nFileLength\r\nCrashFlag\r\nCttBytes\r\nRid
        {
            vector<string> vs = framework::util::splite(cttstr, "\r\n");
            if (vs.size() < 6)
            {
                return;
            }
            ctt_md5_.Parse(vs[1]);
            try
            {
                file_length_ = boost::lexical_cast<size_t>(vs[2]);
            }
            catch (...)
            {
            	file_length_ = 0;
            }
            rid_.Parse(vs[5]);
            try
            {
                ctt_bytes_ = boost::lexical_cast<size_t>(vs[4]);
            }
            catch (...)
            {
                ctt_bytes_ = 0;
            }
            try
            {
                is_clash_ = boost::lexical_cast<u_short>(vs[3]);
            }
            catch (...)
            {
                is_clash_ = 0;
            }
        }
        const string tt_str() const
        {
            const size_t bsize = 10240;
            char buff[bsize] = {0};
            sprintf_s(buff,"%u\r\n%s\r\n%u\r\n%u\r\n%u\r\n%s",
                1,ctt_md5_.ToString().c_str(),
                file_length_,is_clash_,ctt_bytes_,
                rid_.ToString().c_str());
            //CONSOLE_OUTPUT("Ctt" << buff);
            return string(buff);
        }
        bool is_empty()
        {
            return ctt_md5_.IsEmpty()&&!file_length_&&rid_.IsEmpty()&&!ctt_bytes_;
        }
    };
    struct RidStruct //Resource信息结构
    {
        RID rid_;
        MD5 ctt_md5_;
        vector<MD5> block_md5_s_;
        size_t block_num_;
        size_t block_size_;
        size_t file_length_;
        size_t ctt_bytes_;

        RidStruct():rid_(),ctt_md5_(),block_md5_s_(),block_num_(0),block_size_(0),file_length_(0),ctt_bytes_(0){}
        
        RidStruct(string ridstr)
            :rid_(),ctt_md5_(),block_md5_s_(),block_num_(0),block_size_(0),file_length_(0),ctt_bytes_(0)
        //Version\r\nRid\r\nFileLength\r\nBlockSize\r\nBlockNum\r\nBlockMD5\r\nCtt\r\nCttBytes
        {
            vector<string> vs = framework::util::splite(ridstr, "\r\n");
            if (vs.size() < 8)
            {
                return;
            }
            rid_.Parse(vs[1]);
            try
            {
                file_length_ = boost::lexical_cast<size_t>(vs[2]);
            }
            catch(...)
            {
                file_length_ = 0;
            }
            try
            {
                block_size_ = boost::lexical_cast<size_t>(vs[3]);
            }
            catch(...)
            {
                block_size_ = 0;
            }
            try
            {
                block_num_ = boost::lexical_cast<size_t>(vs[4]);
            }
            catch(...)
            {
                block_num_ = 0;
            }
            for (size_t i = 0; block_num_*32 == vs[5].length() && i < block_num_ && (i+1)*32 <= vs[5].length(); i++)
            {
                MD5 md5_tmp;
                string md5_str = vs[5].substr(i*32, 32);
                md5_tmp.Parse(md5_str);
                block_md5_s_.push_back(md5_tmp);
            }
            ctt_md5_.Parse(vs[6]);
            try
            {
                ctt_bytes_ = boost::lexical_cast<size_t>(vs[7]);
            }
            catch(...)
            {
                ctt_bytes_ = 0;
            }
        }
        const string tt_str() const
        {
            const size_t bsize = 10240;
            char buff[bsize];
            string block_md5_s_str = "";
            for(size_t i = 0; i < block_md5_s_.size(); i++)
            {
                block_md5_s_str += block_md5_s_[i].ToString();
            }
            sprintf_s(buff,"%u\r\n%s\r\n%u\r\n%u\r\n%u\r\n%s\r\n%s\r\n%u",
                1,rid_.ToString().c_str(),file_length_,block_size_,block_num_,block_md5_s_str.c_str(),ctt_md5_.ToString().c_str(),ctt_bytes_);
            //CONSOLE_OUTPUT("RID " << buff);
            return string(buff);
        }
        bool is_empty()
        {
            return rid_.IsEmpty()&&block_md5_s_.empty()&&!block_num_&&!block_size_&&!file_length_;
        }
    };
    struct UrlStruct //URL信息结构
    {
        string url_;
        string refer_url_;
        u_short type_;
        RID rid_;

        UrlStruct():url_(),refer_url_(),type_(0),rid_(){}

        UrlStruct(string urlstr)
            :url_(),refer_url_(),type_(0),rid_()
        //Version\r\nRef\r\nType\r\nRid\r\nUrl
        {
            vector<string> vs = framework::util::splite(urlstr, "\r\n");
            if (vs.size() < 5)
            {
                return;
            }
            url_ = vs[4];
            refer_url_ = vs[1];
            try
            {
                type_ = boost::lexical_cast<u_short>(vs[2]);
            }
            catch(...)
            {
                type_ = 0;
            }
            rid_.Parse(vs[3]);
        }
        const string tt_str() const
        {
            const size_t bsize = 10240;
            char buff[bsize];
            sprintf_s(buff,"%u\r\n%s\r\n%u\r\n%s\r\n%s",
                1,refer_url_.c_str(),type_,rid_.ToString().c_str(),url_.c_str());
            //CONSOLE_OUTPUT("URL " << buff);
            return string(buff);
        }
        bool is_empty()
        {
            return url_.empty()&&rid_.IsEmpty();
        }
    };

	class DataBase
		: public boost::noncopyable
		, public boost::enable_shared_from_this<DataBase>
	{
	public:
		typedef boost::shared_ptr<DataBase> p;
		static p Create() { return p(new DataBase()); }

	private:
		TokyoTyrantClient::p mysql_;
		size_t pool_index_;

		bool is_have_a_connection_;
		bool is_new_;

		size_t db_mod_;

		DataBase():is_have_a_connection_(false){}
	public:
		~DataBase(){}
	public:
		bool StartConnection(size_t mod, bool is_new);
		void CloseConnection();

		void DatasInfo(DataBase::p its_ptr, bool is_timer);

	public:
		bool Set(size_t mod, const RidStruct &rid_info, bool is_new);
		bool Set(size_t mod, const UrlStruct &url_info, bool is_new);
		bool Set(size_t mod, const CttStruct &content_info, bool is_new);

		bool Get(size_t mod, RidStruct &rid_struct, bool is_new);
		bool Get(size_t mod, UrlStruct &url_struct, bool is_new);
		bool Get(size_t mod, CttStruct &ctt_struct, bool is_new);

	private:

		size_t CountDataDB(size_t mod);
	};

	
	class DataBaseConnection
	{
	public:
		DataBaseConnection(DataBase::p data_base, size_t mod, bool is_new) : data_base_(data_base) { data_base_->StartConnection(mod, is_new); }
		~DataBaseConnection() { data_base_->CloseConnection(); }
	private:
		DataBase::p data_base_;
	};
}