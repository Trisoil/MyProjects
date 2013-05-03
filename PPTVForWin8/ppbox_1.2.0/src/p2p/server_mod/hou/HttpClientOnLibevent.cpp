//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "Common.h"
#include "UdpRequest.h"
#include "HttpClientOnLibevent.h"

#include "framework/timer/TimeCounter.h"

const size_t ExpectReadSize = 2048;


namespace hou
{
    struct event_base* HttpClientOnLibevent::evbase_;
    
    void OnChunkedReceive(struct evhttp_request * req, void * http_client)
    {
        std::string response_string;
        while(true)
        {
            char response_data[ExpectReadSize];
            int receive_bytes = 0;
            receive_bytes = evbuffer_remove(req->input_buffer, (void *)response_data, ExpectReadSize-1);
            if (receive_bytes <= 0)
            {
                break;
            }

            response_data[receive_bytes] = 0;
            response_string += std::string(response_data, receive_bytes);
        }

        static_cast<HttpClientOnLibevent *>(http_client)->HandleChunkedReceive(response_string, response_string.length());
    }
    
    void OnResourceDownloaded(struct evhttp_request *req, void *http_client)
    {
        HttpClientOnLibevent::ResponseType response_type;
        bool is_gzip = false;
        int error_code = 0;
        

        if(req == NULL)
        {
            response_type = HttpClientOnLibevent::TimeOut;
            
        }
        else if (req->response_code == 0)
        {
            response_type = HttpClientOnLibevent::ConnectionFailed;
        }
        else if (req->response_code != 200)
        {
            error_code = req->response_code;
            response_type = HttpClientOnLibevent::DownloadFailed;
        }
        else
        {
            //libevent1.4,的chunked callback只针对chunked形式的response调用，与2.0.x不一样
            if (req->chunked == 0)
            {
                OnChunkedReceive(req, http_client);
            }
            
            const char * content_encoding_value = evhttp_find_header(req->input_headers, "Content-Encoding");
            if (content_encoding_value != NULL && strcmp(content_encoding_value, "gzip") == 0)
            {
                is_gzip = true;
            }

            response_type = HttpClientOnLibevent::Success;
            error_code = 200;
        }

        static_cast<HttpClientOnLibevent *>(http_client)->HandleDownloadedResult(response_type, is_gzip, error_code);
    }

    HttpClientOnLibevent::HttpClientOnLibevent(boost::shared_ptr<HttpConnectionsRecycle> connection_recycle)
    {
        receive_bytes_ = 0;
        connection_recycle_ = connection_recycle;
    }

    HttpClientOnLibevent::~HttpClientOnLibevent()
    {
        //不在这里直接free HTTP connection，而是交由connection_recycle在随后的一个时间点去free
        //原因在于这个 dtor可能会在request callback中调用到，
        //我们通过实验认为这会偶尔导致double free了event buffer，从而使得event buffer 内部的reference count出问题。
        //现在交由connection_recycle后，统一在每次event_loop之后（即不会在request callback stack中）去free。
        connection_recycle_->AddConnection(connection_);
    }

    bool HttpClientOnLibevent::Init()
    {
#ifdef WIN32
        WSADATA WSAData;
        WSAStartup(0x101, &WSAData);
#endif
      
        evbase_ = event_init();
        
        return evbase_;
    }

    void HttpClientOnLibevent::Create(const std::string & host, unsigned int port, int time_out_in_sec, DownloadCallBack callback)
    {
        connection_ = evhttp_connection_new(host.c_str(), port);
        evhttp_connection_set_timeout(connection_, time_out_in_sec);
        request_ = evhttp_request_new(OnResourceDownloaded, (void *)this);
        request_->chunk_cb = OnChunkedReceive;
        callback_ = callback;
    }

    void HttpClientOnLibevent::AddHeadField(const std::string & key, const std::string & value)
    {
        evhttp_add_header(request_->output_headers, key.c_str(), value.c_str()); 
    }

    void HttpClientOnLibevent::AsyncRead(const std::string & path)
    {
        if (evhttp_make_request(connection_, request_, EVHTTP_REQ_GET, path.c_str()) != 0)
        {
            std::cout << "evhttp_make_request() failed" << std::endl;
            LOG4CPLUS_WARN(Loggers::HouService(), "evhttp_make_request() failed");
            callback_(receive_buffer_, ConnectionFailed, false, 0, 0);
            return;
        }
        
        Run();
    } 

    void HttpClientOnLibevent::Run()
    {
        int event_loop_result = event_loop(EVLOOP_NONBLOCK);
        if (event_loop_result != 0 && event_loop_result != 1)
        {
            std::cout << "event_loop() failed" << std::endl;
            LOG4CPLUS_WARN(Loggers::HouService(), "event_loop() failed");
        }
    }
    
    void HttpClientOnLibevent::HandleChunkedReceive(const std::string & receive_chunked, int bytes_receive)
    {
        receive_buffer_ += receive_chunked;
        receive_bytes_ += bytes_receive;
    }

    void HttpClientOnLibevent::HandleDownloadedResult(const HttpClientOnLibevent::ResponseType & response_type, bool is_gzip, int error_code)
    {
        boost::uint32_t response_time = timer_counter_.elapse();
        if (response_type == Success)
        {
            callback_(receive_buffer_, response_type, is_gzip, error_code, response_time);
        }
        else
        {
            callback_(std::string(), response_type, is_gzip, error_code, 0);
        }
    }

    void HttpClientOnLibevent::Exit()
    {
        event_base_free(evbase_);
    }
}