//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "PerformanceTestService.h"
#include "MemoryCache/MemoryCachePerformanceTestService.h"
#include "LocalCache/LocalDiskCachePerformanceTestService.h"
#include "RemoteCache/RemoteCachePerformanceTestService.h"
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

using namespace super_node_test;

class PerformanceTest
{
public:
    PerformanceTest(const string& test_name, size_t test_run_time_in_seconds, boost::shared_ptr<IPerformanceTestService> test_service)
        : test_name_(test_name),
        test_run_time_in_seconds_(test_run_time_in_seconds),
        test_service_(test_service)
    {
    }

    const string& Name() const { return test_name_; }
    size_t RunTimeInSeconds() const { return test_run_time_in_seconds_; }
    boost::shared_ptr<IPerformanceTestService> TestService() const { return test_service_; }

private:
    string test_name_;
    size_t test_run_time_in_seconds_;
    boost::shared_ptr<IPerformanceTestService> test_service_;
};

void BuildPerformanceTests(std::vector<PerformanceTest>& performance_tests)
{
    performance_tests.clear();
    performance_tests.push_back(
        PerformanceTest(
            "MemoryCache Performance Test", 
            200,
            boost::shared_ptr<MemoryCachePerformanceTestService>(new MemoryCachePerformanceTestService())));
    
    performance_tests.push_back(
        PerformanceTest(
            "LocalDiskCache Performance Test", 
            600,
            boost::shared_ptr<LocalDiskCachePerformanceTestService>(new LocalDiskCachePerformanceTestService())));

    performance_tests.push_back(
        PerformanceTest(
            "RemoteCache Performance Test", 
            600,
            boost::shared_ptr<RemoteCachePerformanceTestService>(new RemoteCachePerformanceTestService())));
}

int main(int argc, char* argv[])
{
    using namespace log4cplus;
    log4cplus::PropertyConfigurator::doConfigure("SuperNodeService-logging.conf");

    std::vector<PerformanceTest> performance_tests;
    BuildPerformanceTests(performance_tests);

    for(size_t i = 0; i < performance_tests.size(); ++i)
    {
        performance_tests[i].TestService()->Start();
        std::cout<<"Running "<<performance_tests[i].Name()<<"..."<<std::endl;
#ifdef BOOST_WINDOWS_API
        Sleep(performance_tests[i].RunTimeInSeconds()*1000);
#else
        sleep(performance_tests[i].RunTimeInSeconds());
#endif
        performance_tests[i].TestService()->Stop();
    }

    return 0;
}
