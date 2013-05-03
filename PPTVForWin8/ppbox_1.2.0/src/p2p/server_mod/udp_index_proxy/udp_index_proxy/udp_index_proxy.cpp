#include "stdafx.h"
#include "base/guid.h"
#include "framework/MainThread.h"
#include "framework/Log.h"
#include "UdpIndexProxyModule.h"
#include "Config.h"

#include <string>
#include <iostream>

using namespace std;
using namespace framework;
using namespace udp_index_proxy;

int _tmain(int argc, _TCHAR* argv[])
{
    Log::Inst().Start();

    Config::Inst().LoadConfig(L"udp_index_proxy.config.txt");
    MainThread::Inst().Start();
    MainThread::IOS().post(boost::bind(&UdpIndexProxyModule::Start, UdpIndexProxyModule::Inst()));

    string line;
    while (cin >> line)
    {
        if (line == "exit")
        {
            MainThread::IOS().post(boost::bind(&UdpIndexProxyModule::Stop, UdpIndexProxyModule::Inst()));
            break;
        }
    }

    MainThread::Inst().Stop();

    Log::Inst().Stop();

    return 0;
}

