// Main.cpp

#include "ppbox/daemon/Common.h"
#include <iostream>
#include <string>

const std::string g_strAlive = "alive";
const std::string g_strVod = "vod_worker";
const std::string g_strLive = "live_worker";

int main(int argc, char * argv[])
{

    if (2 != argc)
    {
        std::cout<<"Wrong Param"<<std::endl;
        return 0;
    }
    std::cout<<"argc :"<<argc<<" argv:"<<argv[1]<<std::endl;

    if (g_strAlive == argv[1])
    {
        return PPAlive_Main(argc,argv);
    }
    else if (g_strVod == argv[1])
    {
        return PPLive_Main(argc,argv);
    }
    else if (g_strLive == argv[1])
    {
        return PPVod_Main(argc,argv);    
    }
    else
    {
        std::cout<<"Wrong Param"<<std::endl;
        return 0;
    }

    return 0;
}
