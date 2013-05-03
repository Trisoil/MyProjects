//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _RESPONSE_SENDER_H_
#define _RESPONSE_SENDER_H_

namespace live_media
{
    class AsioServiceRunner;
    class ResponseTask;
    class ChannelManager;

    class ResponseSender
        : public boost::enable_shared_from_this<ResponseSender>
    {
    public:
        ResponseSender(size_t id, boost::shared_ptr<ChannelManager> manager);

        void Start();
        void Stop();

        void SendResponse(boost::shared_ptr<ResponseTask> response_task);

        void DoSendTask(boost::shared_ptr<ResponseTask> response_task);

    private:

        boost::shared_ptr<AsioServiceRunner> runner_;
        boost::shared_ptr<boost::asio::io_service> io_service_;

        boost::shared_ptr<ChannelManager> manager_;
    };
}

#endif //_RESPONSE_SENDER_H_