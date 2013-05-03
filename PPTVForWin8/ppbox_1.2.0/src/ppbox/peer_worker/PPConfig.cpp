#include "ppbox/peer_worker/Common.h"
#include "util/daemon/Daemon.h"
#include "ppbox/peer_worker/PPConfig.h"
#include "ppbox/peer_worker/WorkerModule.h"


namespace ppbox
{

    namespace peer_worker
    {

        MaxPeerConnection::MaxPeerConnection(WorkerModule& dam) : MidConfig<int,MaxPeerConnection>(dam)
        {
        }

        boost::system::error_code MaxPeerConnection::set_value(int i)
        {
            std::cout<<"****MaxPeerConnection*****set_value:"<<i<<std::endl;
            return boost::system::error_code();
        }

        MaxBufferMemory::MaxBufferMemory(WorkerModule& dam) : MidConfig<int,MaxBufferMemory>(dam)
        {
          
        }

        boost::system::error_code MaxBufferMemory::set_value(int i)
        {   
            std::cout<<"****MaxBufferMemory*****set_value:"<<i<<std::endl; 
            return boost::system::error_code();
        }

        PreAllocSubPieceCount::PreAllocSubPieceCount(WorkerModule& dam) :MidConfig<int,PreAllocSubPieceCount>(dam)
        {
           
        }

        boost::system::error_code PreAllocSubPieceCount::set_value(int i)
        {
            std::cout<<"****PreAllocSubPieceCount*****set_value:"<<i<<std::endl;
            return boost::system::error_code();
        }

        EnableUdpChecksum::EnableUdpChecksum(WorkerModule& dam) : MidConfig<int,EnableUdpChecksum>(dam)
        {

        }

        boost::system::error_code EnableUdpChecksum::set_value(int i)
        {
            std::cout<<"****EnableUdpChecksum*****set_value:"<<i<<std::endl;    
            return boost::system::error_code();
        }

        MaxPeerTunnel::MaxPeerTunnel(WorkerModule& dam) : MidConfig<int,MaxPeerTunnel>(dam)
        {

        }

        boost::system::error_code MaxPeerTunnel::set_value(int i)
        {
            std::cout<<"****MaxPeerTunnel*****set_value:"<<i<<std::endl;  
            return boost::system::error_code();
        }

        MaxPeerConnectorCount::MaxPeerConnectorCount(WorkerModule& dam) : MidConfig<int,MaxPeerConnectorCount>(dam)
        {

        }

        boost::system::error_code MaxPeerConnectorCount::set_value(int i)
        {
            std::cout<<"****MaxPeerConnectorCount*****set_value:"<<i<<std::endl;   
            return boost::system::error_code();
        }
    }
}

