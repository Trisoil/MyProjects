#include "stdafx.h"
#include "MySQLp.h"
#include "base/guid.h"
#include <list>
//#include <fstream>
#include "framework/io/stdfile.h"
#include "Config.h"
#include "framework/timer/Timer.h"
#include "BloomFilter.h"

#ifndef DATA_MIGRATION
#define DATA_MIGRATION

using namespace udpindex;
using namespace std;
using namespace framework::io;

namespace index_migration
{

	class DataMigration: public boost::enable_shared_from_this<DataMigration>
		, public boost::noncopyable, public framework::timer::ITimerListener
	{
	public:
		typedef boost::shared_ptr<DataMigration> p;

		static p Create();
		void Start(DataBaseHost database_out, DataBaseHost database_statistic);
		void Stop();

		//void Migrate(size_t begin_index, size_t end_index, size_t sample_num);

		void MigrateURL();
		void MigrateRID();

		//void CountMiniUrlBytes();

		//void OutputSample();

		bool IsRunning() const { return is_running_; }
		bool IsFinished() const {return is_finished_;}

	private:
 		vector<string> GetURL();
// 		vector<string> GetRID(string rid_str);
// 		bool InsertURL(vector<string> url_line);
// 		bool InsertRID(vector<string> rid_line);

		vector<string> GetRID();

		string Url2Mini(string url, string refer);

		DataMigration():is_running_(false){}

	private:
	//	list<pair<string/*URL*/, string/*RID*/>> sample_;
		bool is_running_;

		size_t time_start_;

		bool is_finished_;
// 
// 		boost::shared_ptr<MemCacheClient> mem_cache_client_;
// 		string mem_cache_server_;
// 		WSADATA wsaData_;

		MySQLp::p database_index_;
		MySQLp::p database_statistic_;

// 		ifstream httpserver_table_file_;
// 		ifstream resource_table_file_;
		
		boost::shared_ptr<StdFile> httpserver_table_file_;
		boost::shared_ptr<StdFile> resource_table_file_;

		boost::shared_ptr<BloomFilter> filter_;

		static const size_t buf_size_ = 10240;
		char readline_buf_[buf_size_];

		framework::timer::PeriodicTimer::p print_sample_timer_;

		static DataMigration::p inst_;
	public:
		static DataMigration::p Inst() { return inst_; };

		virtual void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);
	};

}
#endif