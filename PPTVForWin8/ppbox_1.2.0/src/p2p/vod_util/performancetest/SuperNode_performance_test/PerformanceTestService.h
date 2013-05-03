//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_PERFORMANCE_TEST_SERVICE_H
#define SUPER_NODE_PERFORMANCE_TEST_SERVICE_H

namespace super_node_test
{
    class IPerformanceTestService
    {
    public:
        virtual ~IPerformanceTestService(){}

        virtual void Start() = 0;
        virtual void Stop() = 0;
    };
}

#endif  // SUPER_NODE_PERFORMANCE_TEST_SERVICE_H
