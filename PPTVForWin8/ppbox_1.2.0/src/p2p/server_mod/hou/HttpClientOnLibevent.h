//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _HTTP_CLIENT_ON_LIBEVENT_
#define _HTTP_CLIENT_ON_LIBEVENT_

#include <event.h> 
#include <evhttp.h>  
#include <framework/timer/TimeCounter.h>


namespace hou
{
    class UdpRequest;

    class HttpConnectionsRecycle
    {
    public:
        void AddConnection(evhttp_connection * connection)
        {
            
            assert(connection);
            connections_to_free_.push_back(connection);
        }

        void Recycle()
        {
            for(size_t i = 0; i < connections_to_free_.size(); ++i)
            {
                evhttp_connection_free(connections_to_free_[i]);
            }

            connections_to_free_.clear();
        }

    private:
        std::vector<evhttp_connection *> connections_to_free_;
    };

    class HttpClientOnLibevent
        :public boost::enable_shared_from_this<HttpClientOnLibevent>
    {
    public:
        HttpClientOnLibevent(boost::shared_ptr<HttpConnectionsRecycle> connection_recycle);

        ~HttpClientOnLibevent();

        enum ResponseType
        {
            Success,
            TimeOut,
            DownloadFailed,
            ConnectionFailed,
            ParseFailed,
        };
        
        typedef boost::function<void  (const std::string &, const HttpClientOnLibevent::ResponseType &, bool , int , boost::uint32_t)> DownloadCallBack;
        static void Run();
        void Create(const std::string & host, unsigned int port, int time_out_in_sec, DownloadCallBack callback);
        void AddHeadField(const std::string & key, const std::string & value);
        void AsyncRead(const std::string & path);
        void HandleChunkedReceive(const std::string & receive_chunked, int bytes_receive);
        void HandleDownloadedResult(const HttpClientOnLibevent::ResponseType & response_type, bool is_gzip, int error_code);
        static bool Init();
        static void Exit();
        
    private:
        struct evhttp_connection * connection_;
        struct evhttp_request * request_;
        static struct event_base* evbase_;
        std::string receive_buffer_;
        DownloadCallBack callback_;
        framework::timer::TimeCounter timer_counter_;
        int receive_bytes_;
        boost::shared_ptr<HttpConnectionsRecycle> connection_recycle_;
    };
}

#endif