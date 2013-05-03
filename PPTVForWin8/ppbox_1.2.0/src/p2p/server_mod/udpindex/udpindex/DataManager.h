#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include "udpindex/DataBaseConnectionPool.h"
#include "protocal/IndexPacket.h"

namespace udpindex
{
	class DataManager
		: public boost::noncopyable
		, public boost::enable_shared_from_this<DataManager>
	{
	public:
		typedef boost::shared_ptr<DataManager> p;
		static p Create() {return p(new DataManager());}

		void QueryRIDByUrl(boost::asio::ip::udp::endpoint& end_point, QueryRidByUrlRequestPacket::p request_packet);
		void QueryRIDByCtt(boost::asio::ip::udp::endpoint& end_point, QueryRidByContentRequestPacket::p request_packet);

		void AddUrl(AddRidUrlRequestPacket::p packet);
		void AddCtt(AddRidUrlRequestPacket::p packet);
		void AddRid(AddRidUrlRequestPacket::p packet);

        ~DataManager(){}
	private:
		DataManager(){}
	};
}