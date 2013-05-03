#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "request_parser.hpp"
#include "request.hpp"

char * g_peer_ip = "127.0.0.1";
unsigned short g_peer_port = 9000;
int g_step = 10;

using boost::asio::ip::tcp;

class HttpSession :
    public boost::enable_shared_from_this<HttpSession>
{
public:
    HttpSession(boost::asio::io_service& io_service)
        : http_socket_(io_service)
    {
        this->long_connection_read_buffer_.resize( 1024 * 4 );
    }

    tcp::socket& socket()
    {
        return http_socket_;
    }

    void send_error_respond() {
        // 发送错误的回应
        this->http_socket_.close();
    }

    void send_http_200_respond() {
        // 给回应

        boost::asio::streambuf respond;
        std::ostream respond_stream(&respond);
        respond_stream << "HTTP/1.0 200 ok\r\n";
        respond_stream << "\r\n";

        // Send the request.
        boost::asio::write(http_socket_, respond);
    }

    void handle_request(const http::server::request& req)
    {
        // 
        size_t pos = req.uri.rfind("&start=");
        std::string request_uri( req.uri.c_str() , pos );
        if( pos == req.uri.npos ) {
            // 
            return;
        }

        int start_time = 0;
        int interval = 0;
        sscanf( req.uri.c_str() + pos + strlen("&start=") , "%d" , &start_time );
        printf("start_time: %d\n" , start_time );

        pos = req.uri.rfind("&interval=");
        if( pos == req.uri.npos ) {
            // 
            return;
        }
        sscanf( req.uri.c_str() + pos + strlen("&interval=") , "%d" , &interval );

        boost::asio::io_service io_service;

        // 去到 peer 那里拉这个数据

        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(g_peer_ip, "9000");
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        boost::asio::ip::tcp::socket control_s(io_service);
        {
            // 发起控制连接
            control_s.connect(*endpoint_iterator);

            boost::asio::streambuf request;
            std::ostream request_stream(&request);

            request_stream << "GET " << request_uri.c_str() << "&start=" 
                << start_time << "&kpl=true&interval=" << interval
                << " HTTP/1.0\r\n";
            request_stream << "Host: " << g_peer_ip << "\r\n";
            request_stream << "Accept: */*\r\n";
            request_stream << "Connection: close\r\n\r\n";

            boost::asio::write( control_s , request);
        }

        bool first_piece = true;
        while( true ) {
            boost::asio::ip::tcp::socket s(io_service);
            boost::system::error_code ec;
            s.connect(*endpoint_iterator);

            boost::asio::streambuf request;
            std::ostream request_stream(&request);
            std::ostringstream oss;

            std::string ss =  std::string(req.uri.c_str() , pos);

            oss << "GET " << request_uri.c_str() << "&start=" 
                << start_time << "&kpl=false&interval=" << interval
                << " HTTP/1.0\r\n";

            // printf("%s\n" , oss.str().c_str() );
            request_stream << "GET " << request_uri.c_str() << "&start=" 
                << start_time << "&kpl=false&interval=" << interval
                << " HTTP/1.0\r\n";
            request_stream << "Host: " << g_peer_ip << "\r\n";
            request_stream << "Accept: */*\r\n";
            request_stream << "Connection: close\r\n\r\n";

            // Send the request.
            boost::asio::write(s, request);
            boost::asio::streambuf response;
            boost::system::error_code error;

            //            int readed = boost::asio::read(s, response,boost::asio::transfer_at_least(1), error);
            int readed = boost::asio::read_until( s , response , "\r\n");

            // Check that response is OK.
            std::istream response_stream(&response);
            std::string http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);
            if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
                std::cout << "Invalid response\n";
                s.close();
                return ;
            }

            if (status_code == 200) {
                // Read the response headers, which are terminated by a blank line.
                boost::asio::read_until(s, response, "\r\n\r\n");

                boost::asio::streambuf data_response;

                int readed_count = 0;
                while (int readed = boost::asio::read(s, data_response,boost::asio::transfer_at_least(1), error)) {
                    if( readed_count == 0 && !first_piece ) {
                        // 如果是这种情况,需要去掉拿到的 13 个字节
                        data_response.consume(13);
                    }


                    // 推送给前端
                    int writed = boost::asio::write( this->http_socket_ , data_response );
                    //                    printf("readed: %d , writed: %d\n" , readed , writed);
                    readed_count++;
                }

                /*
                if (error != boost::asio::error::eof) {
                // 断开了，这里不处理，认为是正常

                // 出错了
                throw boost::system::system_error(error);
                }
                */

                first_piece = false;

                // 增加 interval
                start_time += interval;
            } else if( status_code == 404 ) {
                // 忽略
            }

            // 进行 sleep
#ifdef WIN32
            Sleep( 1000 );
#else
            sleep( 1 );
#endif
        }
    }

    void start()
    {
        boost::asio::streambuf request;
        int readed = boost::asio::read_until( http_socket_ , request , "\r\n\r\n");
        std::istream request_stream(&request);
        const char* header=boost::asio::buffer_cast<const char*>(request.data());

        http::server::request http_request;
        http::server::request_parser http_request_parser;
        boost::tribool result;
        boost::tie(result, boost::tuples::ignore) = 
            http_request_parser.parse(http_request, header, header + readed);

        send_http_200_respond();

        if(result) {
            // 成功了
            handle_request( http_request );
        } else {
            printf("parse error\n");
        }

        /*
        // 分析数据
        if( strncmp( get_request.c_str() , "GET " , 4 ) != 0 ) {
        // 出错
        this->send_error_respond();
        return;
        }

        // get 哪个文件
        size_t pos = get_request.rfind(' ');
        if( pos == get_request.npos ) {
        this->send_error_respond();
        }

        */

        http_socket_.close();
    }

    void handle_read_request(const boost::system::error_code& error)
    {
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
        }
        else
        {
        }
    }

private:
    tcp::socket http_socket_;
    std::string long_connection_read_buffer_;
};

typedef boost::shared_ptr<HttpSession> HttpSessionPtr;

class HttpServer
{
public:
    HttpServer(boost::asio::io_service& io_service,
        const tcp::endpoint& endpoint)
        : io_service_(io_service),
        acceptor_(io_service, endpoint)
    {
        HttpSessionPtr new_session(new HttpSession(io_service_));
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&HttpServer::handle_accept, this, new_session,
            boost::asio::placeholders::error));
    }

    void handle_accept(HttpSessionPtr session,
        const boost::system::error_code& error)
    {
        if (!error)
        {
            try {
                session->start();
            } catch( ... ) {
                //
                printf("exception catched\n");
            }

            HttpSessionPtr new_session(new HttpSession(io_service_));
            acceptor_.async_accept(new_session->socket(),
                boost::bind(&HttpServer::handle_accept, this, new_session,
                boost::asio::placeholders::error));
        }
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};

typedef boost::shared_ptr<HttpServer> HttpServerPtr;
typedef std::list<HttpSessionPtr> HttpSessionList;


int main( int argc , char ** argv )
{
    boost::asio::io_service ios;
    tcp::endpoint endpoint(tcp::v4(), 8999);

    HttpServer server(ios , endpoint);
    // 监听

    ios.run();

    return 0;
}
