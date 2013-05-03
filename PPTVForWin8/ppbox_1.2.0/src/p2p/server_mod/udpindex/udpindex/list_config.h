#pragma once

/************************************************************************/
/* list config                                                          */
/************************************************************************/
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <vector>

#include "framework/io/stdfile.h"
#include "protocal/structs.h"

#include "udpindex/TokyoTyrantClient.h"

using namespace TokyoTyrant;

using framework::io::StdFile;
using namespace std;

namespace udpindex
{
	struct DBInfo
	{
		UINT8 old_mode;
		bool if_need_trans;
		UINT8 new_mode;
	};

	//bool Str2DBHost(const string& host_str, DataBaseHost& host);

	bool Str2IndexInfo(const string& str, INDEX_SERVER_INFO& index_info);
	//////////////////////////////////////////////////////////////////////////
	// class ListConfig

	class ListConfig 
	{
	public:
		static bool GetIndexServerList(const tstring& filename, vector<INDEX_SERVER_INFO>& index_list,
			vector<UINT8>& mode_list);
		static bool GetDBList(const tstring& filename, vector<HostInfo>& db_list, vector<DBInfo>& db_info
			, vector<boost::asio::ip::udp::endpoint>& stat_list);
	};
} // namespace udpindex
