//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_APP_H
#define SUPER_NODE_APP_H

namespace super_node
{
    class SuperNodeService;

    class App
        : public boost::enable_shared_from_this<App>
    {
    public:
        int Run(bool run_as_background);
        
        static const std::string ServiceVersion;

        static boost::shared_ptr<App> Instance();

        void ReloadConfiguration();
        
        void Stop();

        bool IsRunning() const
        {
            return service_;
        }

        void UpdateConfig();

    private:
        App(){}

        void HandleConsoleInput();
        void WaitForSignal();
    private:
        boost::shared_ptr<SuperNodeService> service_;
        static const std::string HelpString;
    };
}

#endif // SUPER_NODE_APP_H
