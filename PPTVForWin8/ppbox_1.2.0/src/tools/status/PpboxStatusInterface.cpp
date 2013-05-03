// PpboxStatusInterface.cpp

#include "tools/status/Common.h"
#define PPBOX_SOURCE
#include "tools/status/PpboxStatusInterface.h"
#include "tools/status/PeerWorkStatus.h"
#include "tools/status/DataStruct.h"
#include "tools/status/TcpTransferClient.h"
using namespace ppbox::status;

#include <util/archive/BinaryIArchive.h>
#include <util/serialization/stl/map.h>
using namespace util::serialization;

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

PeerWorkStatus peer;

#if __cplusplus
extern "C" {
#endif // __cplusplus

    PPBOX_DECL bool PPBOX_StatusStart(
        const char *ip_address)
    {
        if(ip_address ==  NULL)
            return false;
        std::string addr(ip_address);
        peer.set_address(addr);

        if(peer.start())
            return true;
        return false;
    }

    PPBOX_DECL bool PPBOX_StatusStop()
    {
        peer.terminate();
        return true;
    }

    PPBOX_DECL bool PPBOX_StatusGetInfo(
        PeerInfo * peer_info)
    {
        static PeerBlock *peer_block = NULL;
        if(peer_block)
        {
            delete []peer_block;
            peer_block = NULL;
        }
        boost::uint32_t n = 0;
        std::map<std::string, Block> blocks;
        int ret = peer.get_info(blocks);
        if (ret != ok)
            return false; // ret;
        boost::uint32_t len = blocks.size();
        peer_block = new PeerBlock[len];
        memset(peer_block,0,sizeof(PeerBlock)*len);
        for (std::map<std::string, Block>::const_iterator i = blocks.begin(); i != blocks.end(); ++i) 
        {
            memcpy(&peer_block[n].name[0],&i->first[0],i->first.size());
            peer_block[n].ptr = i->second.ptr;
            peer_block[n].size = i->second.size;
            n++;
        }
        memset(&peer_info->peer_pid[0],0,sizeof(peer_info->peer_pid));
        char const * needl = "_";
        char *buf_b = strstr(peer_block[0].name,needl);
        if (!buf_b)
            return false; // other_error;
        buf_b += strlen(needl);
        char *buf_e = strstr(buf_b,needl);
        if(!buf_e)
            memcpy(&peer_info->peer_pid[0],buf_b,strlen(buf_b));
        else
            memcpy(&peer_info->peer_pid[0],buf_b,buf_e - buf_b);
        std::cout << "receive pid: " << peer_info->peer_pid << std::endl;

        peer_info->block = peer_block;
        peer_info->number = len;
#ifdef BOOST_WINDOWS_API
        for (size_t i = 0; i < len; ++i) {
            OutputDebugString(peer_block[i].name);
            OutputDebugString("\r\n");
        }
#endif
        return ret == ok; // ret;
    }
#if __cplusplus
}
#endif // __cplusplus

#ifndef _LIB
int main(int argc, char *argv[])
{
    PeerInfo peer_info;
    STASTISTIC_INFO *temp_info;
    PPBOX_StatusStart("192.168.1.100:1802");
    while(1)
    {
        boost::xtime xt;
        boost::xtime_get(&xt, boost::TIME_UTC); 
        xt.sec += 1;
        boost::thread::sleep(xt); 
        memset(&peer_info,0,sizeof(PeerInfo));
        PPBOX_StatusGetInfo(&peer_info);
        if(peer_info.number == 0)
        {
            std::cout << "pass error:" <<  std::endl;
            continue;
        }

        temp_info = (STASTISTIC_INFO*)peer_info.block[0].ptr;

        std::cout << "LocalPeerVersion:" << temp_info->LocalPeerVersion << std::endl;
        std::cout << "P2PDownloaderCount:" << (boost::uint32_t)temp_info->P2PDownloaderCount << std::endl;
        std::cout << "TrackerCount:" << (boost::uint32_t)temp_info->TrackerCount << std::endl;
        std::cout << "LocalIpCount:" << (boost::uint32_t)temp_info->LocalIpCount << std::endl;
        std::cout << "TotalHttpNotOriginalDataBytes:" << temp_info->TotalHttpNotOriginalDataBytes << std::endl;
        std::cout << "IncomingPeersCount:" << (boost::uint32_t)temp_info->IncomingPeersCount << std::endl<< std::endl;
    }
    return 0;
}
#endif
