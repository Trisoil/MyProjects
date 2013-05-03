#pragma once

#include <ctime>
#include <vector>
#include <set>
#include <algorithm>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "udptracker/TrackerStatistic.h"

#include "struct/Structs.h"

//#include "IpLocater.h"

using namespace framework::timer;

namespace udptracker
{
    class CandidatePeerInfoEx : public protocol::CandidatePeerInfo
    {
    public:
        CandidatePeerInfoEx() : upload_bandwidth_kbs(0) , upload_limit_kbs(0) , upload_speed_kbs(0),internal_tcp_port(0),upnp_tcp_port(0)
        {
        }

        // 新增一些结构
        boost::int32_t upload_bandwidth_kbs;
        boost::int32_t upload_limit_kbs;
        boost::int32_t upload_speed_kbs;

        // peer 的地域信息
        //location_info peer_location_info;
        boost::uint32_t peer_location;

        //新增的tcp的信息
        unsigned short internal_tcp_port;
        unsigned short upnp_tcp_port;

    };

    typedef CandidatePeerInfoEx PeerInfo;

    enum STUN_NAT_TYPE
    {
        TYPE_ERROR = -1,
        TYPE_FULLCONENAT = 0,
        TYPE_IP_RESTRICTEDNAT,
        TYPE_IP_PORT_RESTRICTEDNAT,
        TYPE_SYMNAT,
        TYPE_PUBLIC,
    };

     inline StatisticType::NatStatisticType GetNatStatisitcType(size_t nat_type)
    {
        return static_cast<StatisticType::NatStatisticType>(++nat_type);
    }
}
