#pragma once

//#include "DataThread.h"
//#include "DataWriter.h"
#include "TrackerLogger.h"

#include <boost/lexical_cast.hpp>

namespace udptracker
{
    struct IResourcePeerCountHandler
    {
        // NOTE: 这个函数在MainThread中被调用
        virtual void operator() (const RID& rid, unsigned peer_count) const = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    // Default Implementation

    struct ResourcePeerCountHandler : IResourcePeerCountHandler
    {
        void operator() (const RID& rid, unsigned peer_count) const
        {
            //DataThread::IOS().post(boost::bind(&DataWriter::WriteRidCount, DataWriter::Inst(), rid, peer_count));
            
            //屏蔽
            //string line = rid.ToString() + " " + boost::lexical_cast<string>(peer_count);
            //DataThread::IOS().post(boost::bind(&DataWriter::WriteLine, DataWriter::Inst(), line));

            // TRACK_INFO("RID: " << rid << " COUNT: " << peer_count);
        }
    };
}
