#ifndef _PPBOX_PPBOX_PEERWORKER_PPCONFIG_H_
#define _PPBOX_PPBOX_PEERWORKER_PPCONFIG_H_

#include <framework/configure/Config.h>
using namespace framework::configure;

namespace ppbox
{
    namespace peer_worker
    {
#define MaxPeerConnection_      "maxPeerConnection"
#define MaxBufferMemory_        "maxBufferMemory"
#define PreAllocSubPieceCount_  "preAllocSubPieceCount"
#define EnableUdpChecksum_      "enableUdpChecksum"
#define MaxPeerTunnel_          "maxPeerTunnel"
#define MaxPeerConnectorCount_  "maxPeerConnectorCount"


        class WorkerModule;


        template<typename T, typename C>
        struct MidConfig
            : public ConfigItemHelper<T,C>
        {
            MidConfig(WorkerModule& dam): ConfigItemHelper<T, C>(framework::configure::allow_get | framework::configure::allow_set),module_(dam){}
            WorkerModule& module_;
        };


        struct MaxPeerConnection
            : public MidConfig<int,MaxPeerConnection>
        {
            MaxPeerConnection(WorkerModule&);
            boost::system::error_code set_value(int i);
        };

        struct MaxBufferMemory 
            : public MidConfig<int,MaxBufferMemory>
        {
            MaxBufferMemory(WorkerModule&); 
            boost::system::error_code set_value(int i);
        };

        struct PreAllocSubPieceCount
            : public MidConfig<int,PreAllocSubPieceCount>
        {
            PreAllocSubPieceCount(WorkerModule&) ;
            boost::system::error_code set_value(int i);
        };

        struct EnableUdpChecksum 
            : public MidConfig<int,EnableUdpChecksum>
        {
            EnableUdpChecksum(WorkerModule&);
            boost::system::error_code set_value(int i);
        };

        struct MaxPeerTunnel
            : public MidConfig<int,MaxPeerTunnel>
        {
            MaxPeerTunnel(WorkerModule&);
            boost::system::error_code set_value(int i);
        };

        struct MaxPeerConnectorCount
            : public MidConfig<int,MaxPeerConnectorCount>
        {
            MaxPeerConnectorCount(WorkerModule&);
            boost::system::error_code set_value(int i);
        };
    }
}
#endif // _PPBOX_PPBOX_VERSION_H_
