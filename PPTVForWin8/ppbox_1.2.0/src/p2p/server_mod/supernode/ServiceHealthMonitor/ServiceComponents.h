//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_SERVICE_COMPONENTS_H
#define SUPER_NODE_SERVICE_COMPONENTS_H

namespace super_node
{
    class ServiceComponents
    {
    public:
        const static string LocalDiskCache;
        const static string LocalDiskPrefix;
        const static string MemoryCache;
        const static string UdpServer;
        const static string SessionManagement;
        const static string RemoteCache;
    };
}

#endif //SUPER_NODE_SERVICE_COMPONENTS_H