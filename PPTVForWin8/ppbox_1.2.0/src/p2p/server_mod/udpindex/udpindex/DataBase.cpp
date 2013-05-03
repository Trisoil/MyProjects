#include "stdafx.h"
#include "udpindex/DataBase.h"
#include "protocal/StatisticPacket.h"
#include "protocal/IndexPacket.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "IndexFunc.h"

namespace udpindex
{
	bool DataBase::StartConnection(size_t mod, bool is_new)
	{
		if (is_have_a_connection_)
		{
			CloseConnection();
		}
		db_mod_ = mod;
		is_new_ = is_new;

		if(DataBaseConnectionPool::Inst().RequestConnection(mysql_, db_mod_, pool_index_, is_new_))
		{
			INDEX_EVENT("DataBase::StartConnection() Connect Succeed "<<pool_index_);
			is_have_a_connection_ = true;
			return true;
		}
		else
		{
			RELEASE_LOG("DataBase::StartConnection() Connect Failed");
			is_have_a_connection_ = false;
			return false;
		}
	}

	void DataBase::CloseConnection()
	{
		if (is_have_a_connection_ == false)return;

		DataBaseConnectionPool::Inst().FreeConnection(mysql_, db_mod_, pool_index_, is_new_);
		is_have_a_connection_ = false;
		INDEX_EVENT("DataBase::CloseConnection() Database Closed!"<<pool_index_);
	}

	void DataBase::DatasInfo(DataBase::p its_ptr, bool is_timer)
	{
		vector<size_t> dt_counts;
		map<size_t,size_t> vtct;
		for (size_t i = 0; i < 256; i++)
		{
            size_t db_num = DataBaseConnectionPool::Inst().Mod2DBNum(i,true);
            if (vtct.find(db_num) != vtct.end())
            {
                continue;
            }

            vtct.insert(make_pair(db_num,CountDataDB(i)));			
		}

		MainThread::IOS().post(boost::bind(&UdpIndexModule::OnPrintDatabaseInfo, UdpIndexModule::Inst(),
			vtct, is_timer));
	}

    size_t DataBase::CountDataDB(size_t mod)
    {
        DataBaseConnection dbc(shared_from_this(), mod, true);
        if(is_have_a_connection_ == false) return 0;

        string val = "";
        if(!mysql_->Stat("curr_items",val))
        {
            return 0;
        }
        return atoi(val.c_str());
    }
	

	bool DataBase::Set(size_t mod, const RidStruct &rid_info, bool is_new)
 	{
		DataBaseConnection dbc(shared_from_this(), mod, is_new);
		if(is_have_a_connection_ == false) return false;
        if(!mysql_->Set("R_"+rid_info.rid_.ToString(), rid_info.tt_str()))
        {
            return false;
        }

		return true;
	}
	bool DataBase::Set(size_t mod, const UrlStruct &url_info, bool is_new)
	{
		DataBaseConnection dbc(shared_from_this(), mod, is_new);
		if(is_have_a_connection_ == false) return false;

        if(!mysql_->Set("U_"+Url2MD5(Url2Mini(url_info.url_,url_info.refer_url_)).ToString(), url_info.tt_str()))
        {
            return false;
        }

		return true;
	}
	bool DataBase::Set(size_t mod, const CttStruct &ctt_info, bool is_new)
	{
		DataBaseConnection dbc(shared_from_this(), mod, is_new);
		if(is_have_a_connection_ == false) return false;

        if(!mysql_->Set("C_"+Ctt2MD5(ctt_info.ctt_md5_,ctt_info.ctt_bytes_,ctt_info.file_length_).ToString(), ctt_info.tt_str()))
        {
            return false;
        }

		return true;
	}
	
	bool DataBase::Get(size_t mod, RidStruct &rid_struct, bool is_new)
	{
		DataBaseConnection dbc(shared_from_this(), mod, is_new);
		if(is_have_a_connection_ == false) return false;
        string rid_info_str = "";
        //RELEASE_LOG("Get rid = " << rid_struct.rid_.ToString());
        if (!mysql_->Get("R_"+rid_struct.rid_.ToString(), rid_info_str))
        {
            return false;
        }
        //RELEASE_LOG("!Get rid = " << rid_struct.rid_.ToString());
        rid_struct = RidStruct(rid_info_str);
        return true;
	}
	bool DataBase::Get(size_t mod, UrlStruct &url_struct, bool is_new)
	{
		DataBaseConnection dbc(shared_from_this(), mod, is_new);
		if(is_have_a_connection_ == false) return false;

        string url_info_str = "";
        if(!mysql_->Get("U_"+Url2MD5(Url2Mini(url_struct.url_,url_struct.refer_url_)).ToString(), url_info_str))
        {
            return false;
        }
        url_struct = UrlStruct(url_info_str);
        return true;
	}
	bool DataBase::Get(size_t mod, CttStruct &ctt_struct, bool is_new)
	{
		DataBaseConnection dbc(shared_from_this(), mod, is_new);
		if(is_have_a_connection_ == false) return false;

        string ctt_info_str = "";
        if(!mysql_->Get("C_"+Ctt2MD5(ctt_struct.ctt_md5_,ctt_struct.ctt_bytes_,ctt_struct.file_length_).ToString(), ctt_info_str))
        {
            return false;
        }
        ctt_struct = CttStruct(ctt_info_str);
        return true;
	}
}
