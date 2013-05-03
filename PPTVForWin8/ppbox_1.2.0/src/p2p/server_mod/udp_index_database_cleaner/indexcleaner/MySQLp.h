#pragma once
#include "StdAfx.h"
#include "mysql.h"

#ifndef MYSQL_PLUS
#define MYSQL_PLUS

namespace udpindex
{
	struct DataBaseHost 
	{
		string ip_; 
		string usr_;
		string pwd_;
		u_short port_;
		string dbname_;
	};

	class MySQLp: public boost::enable_shared_from_this<MySQLp>
		, public boost::noncopyable
		//这是一个对MySQL C API的封装类 类名是MySQL Plus的缩写
	{
	public:
		typedef boost::shared_ptr<MySQLp> p;

		static p Create(const DataBaseHost &host);

	public:		
		bool Query(const string &query);
		void Stop();

		inline const vector<vector<string>> &GetResults(){ return results_; }
		inline const bool &IsGoodConnection(){return is_good_connection_;}

	private:
		MYSQL mysql_;
		vector<vector<string>> results_;
		DataBaseHost host_;

		char reconnect_flag_;
		bool is_good_connection_;

		MySQLp(const DataBaseHost &host);
	};
}

#endif