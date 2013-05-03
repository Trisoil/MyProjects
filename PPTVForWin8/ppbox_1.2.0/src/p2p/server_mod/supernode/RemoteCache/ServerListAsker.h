//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _SN_SERVER_LIST_ASKER_H_
#define _SN_SERVER_LIST_ASKER_H_

namespace super_node
{
    typedef boost::function< 
        void (const std::vector<std::string> &) 
        > DownloadServerListCallBack;

    class IServerListAsker
    {
    public:
        virtual void Start() = 0;
        virtual void UpdateConfig(boost::uint32_t interval, const std::string & place_identifier) = 0;
        virtual ~IServerListAsker(){};
    };

    class ServerListAsker
        :public boost::enable_shared_from_this<ServerListAsker>,
        public IServerListAsker
    {
    public:
        ServerListAsker(boost::uint32_t interval, const std::string & place_identifier, boost::shared_ptr<boost::asio::io_service> io_service, DownloadServerListCallBack callback);
        void Start();
        void Stop();
        void UpdateConfig(boost::uint32_t interval, const std::string & place_identifier);

    private:
        void DoAskForServerList();
        void HandleAskForServerList(){}

    private:
        DownloadServerListCallBack callback_;
        boost::uint32_t interval_;
        std::string  place_identifier_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
    };
}
#endif