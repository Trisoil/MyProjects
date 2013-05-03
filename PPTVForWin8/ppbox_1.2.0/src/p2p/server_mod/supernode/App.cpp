//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "App.h"
#include "SuperNodeService.h"
#ifndef BOOST_WINDOWS_API
#include <signal.h>
#endif
#include <log4cplus/configurator.h>

namespace super_node
{

const std::string App::HelpString("command list:\n" \
    "help: show this help\n" \
    "version: show version\n" \
    "LoadConfig: load config\n" +
#ifdef SN_DUMP_OBJECT
    string("status: show object counts\n") +
#endif
    string("exit: stop the SuperNodeService\n"));

const std::string App::ServiceVersion = "1.1.0.3";

boost::shared_ptr<App> App::Instance()
{
    static boost::shared_ptr<App> application_instance;
    if (!application_instance)
    {
        application_instance.reset(new App());
    }

    return application_instance;
}

int App::Run(bool run_as_background)
{
    service_.reset(new SuperNodeService());

    std::cout<<"SuperNode Service is starting up..."<<std::endl;
    std::cout<<"It may take quite a while to load the cached resources on the local disks."<<std::endl;

#ifndef BOOST_WINDOWS_API
    // signals are handled only in the main thread;
    // block all signals, and let other threads started in [service_->Start()] inherit this;
    sigset_t  allsig;
    sigfillset(&allsig);
    if (pthread_sigmask(SIG_BLOCK, &allsig, 0) != 0)
        std::cout << "fail to blocks signals" << std::endl;
#endif
    if (service_->Start())
    {
        if (run_as_background)
        {
            WaitForSignal();
        }
        else
        {
            HandleConsoleInput();
        }

        return 0;
    }
    else
    {
        std::cout<<"SuperNode Service failed to start, and is stopping soon..."<<std::endl;
        service_->Stop();
        return 1;
    }
}

void App::UpdateConfig()
{
    if (service_)
        service_->LoadConfig();
}

void App::Stop()
{
    std::cout<<"Stopping SuperNode Service..."<<std::endl;

    if (service_)
    {
        service_->Stop();
        service_.reset();
    }

#ifdef SN_DUMP_OBJECT
    std::cout<<"Objects count:"<<std::endl;
    object_counter::get_counter()->dump_all_objects();
#endif
}

void App::HandleConsoleInput()
{
    while (true)
    {
        std::string command;
        std::cin >> command;

        if (command == "help")
        {
            std::cout << HelpString;
            continue;
        }
        if (command == "version")
        {
            std::cout << "Version: " << ServiceVersion << std::endl;
            continue;
        }
        if (command == "exit")
        {
            Stop();
            break;
        }
        if (command == "LoadConfig")
        {
            ReloadConfiguration();
            continue;
        }

#ifdef SN_DUMP_OBJECT
        if (command == "status")
        {
            object_counter::get_counter()->dump_all_objects();
            continue;
        }
#endif
        std::cout << "bad command: " << command << std::endl;
    }
}

#ifndef BOOST_WINDOWS_API
static void SignalHandler(int signal)
{
    switch (signal)
    {
    case SIGHUP:
        LOG4CPLUS_INFO(Loggers::Service(), "A signal to reload service configuration is received.");
        OperationsLogging::Log(OperationsLogEvents::ReceivedSignalToReloadConfig);
        
        App::Instance()->ReloadConfiguration();
        break;

    case SIGTERM:
    case SIGINT:
        LOG4CPLUS_INFO(Loggers::Service(), "A signal to stop service is received.");
        OperationsLogging::Log(OperationsLogEvents::ReceivedSignalToStopService);

        App::Instance()->Stop();

    case SIGUSR1:
        App::Instance()->UpdateConfig();
        break;
    }
}
#endif

void App::WaitForSignal()
{
#ifdef BOOST_WINDOWS_API
    while(true)
    {
        Sleep(1000);
    }
#else
    sigset_t allsig;
    sigfillset(&allsig);

    while(App::Instance()->IsRunning())
    {
        int rc, sig;
        rc = sigwait(&allsig, &sig);

        if (rc == 0)
            SignalHandler(sig);
        else
            std::cout << "sigwait(...) failed(" << errno << ": " << strerror(errno) << ")" << std::endl;
    }
#endif
}

void App::ReloadConfiguration()
{
    OperationsLogging::Log(OperationsLogEvents::ReloadingConfiguration);

    log4cplus::PropertyConfigurator::doConfigure("SuperNodeService-logging.conf");
    service_->LoadConfig();
}

}
