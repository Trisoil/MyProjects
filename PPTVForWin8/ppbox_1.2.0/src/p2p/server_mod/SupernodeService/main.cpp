//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "App.h"
#include "CommandLineOptions.h"
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>


#if 1
using namespace super_node;

int main(int argc, char* argv[])
{
    log4cplus::PropertyConfigurator::doConfigure("SuperNodeService-logging.conf");

    LOG4CPLUS_INFO(Logger::getRoot(), "SuperNodeService Launched");

    CommandLineOptions options;
    if (!options.TryParse(argc, argv))
    {
        options.ShowHelp();
        return 1;
    }

    if (options.ShouldShowHelp()) 
    {
        options.ShowHelp();
        return 0;
    }

    if (options.ShouldShowVersion())
    {
        std::cout << App::ServiceVersion <<std::endl;
        return 0;
    }

    int app_result = App::Instance()->Run(options.RunAsBackground());

    return app_result;
}
#endif




#if 0
extern void test_ResourceMetaInfoDB(int argc, char* argv[]);
extern void test_BigFileLocalDisk(int argc, char* argv[]);
extern void test_performance_ResourceMetaInfoDB(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    log4cplus::PropertyConfigurator::doConfigure("SuperNodeService-logging.conf");
//    test_ResourceMetaInfoDB(argc, argv);
    test_BigFileLocalDisk(argc, argv);
//    test_performance_ResourceMetaInfoDB(argc, argv);

}
#endif
