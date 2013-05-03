#include "StdAfx.h"
#include "MySQLp.h"
#include "errmsg.h"

namespace udpindex
{
	MySQLp::p MySQLp::Create(const DataBaseHost &host)
	{
		return p(new MySQLp(host));
	}

	MySQLp::MySQLp(const DataBaseHost &host)
		:is_good_connection_(false), host_(host)
	{
		reconnect_flag_ = 1;//1表示自动重连 0表示不自动重连

		mysql_init(&mysql_);
		mysql_options(&mysql_,MYSQL_OPT_RECONNECT, &reconnect_flag_);

		if (mysql_real_connect(&mysql_, host_.ip_.c_str(), host_.usr_.c_str(), host_.pwd_.c_str(), host_.dbname_.c_str(), host_.port_, 0, 0))
		{
			is_good_connection_ = true;
			//INDEX_EVENT("MySQLp::Conect() Connect Succeed ");
			//INDEX_EVENT("MySQLp::Conect() IP = " << host_.ip_);
		}
		else
		{
			CONSOLE_LOG("MySQLp::Conect() Connect Failed");
			CONSOLE_LOG("MySQLp::Conect() IP = " << host_.ip_ << "\tport = " << host_.port_);
		}
	}

	bool MySQLp::Query(const string &query)
		//执行SQL语句 成功返回真 并将结果存放在 resluts_中 如果没有结果 resluts_.size() == 0
	{
		MYSQL_RES *results;
		MYSQL_ROW record;
		int num_fields;	 

		results_.clear();

		if(mysql_query(&mysql_, query.c_str()))
		{
			u_int err = mysql_errno(&mysql_);
			
			RELEASE_LOG("数据库操作失败，错误代码 " << err << ": " << mysql_error(&mysql_)); 
			//INDEX_EVENT("数据库操作失败，错误代码 " << err); 
			
			return false;
		}

		if(mysql_field_count(&mysql_))
		{
			results = mysql_store_result(&mysql_);
			num_fields = mysql_num_fields(results);

			while((record = mysql_fetch_row(results)))
			{
				vector<string> rslt_line;
				for(int i = 0; i < num_fields; i++ )
				{
					rslt_line.push_back(string(record[i]));
				}
				results_.push_back(rslt_line);
				rslt_line.clear();
			}
			mysql_free_result(results);
		}
		return true;
	}

	void MySQLp::Stop()
	{
		is_good_connection_ = false;
		mysql_close(&mysql_);
		results_.clear();
	}
}