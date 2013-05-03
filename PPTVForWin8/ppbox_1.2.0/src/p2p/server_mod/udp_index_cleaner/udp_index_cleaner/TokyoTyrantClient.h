#pragma once
#include <string>
#include <iostream>
#include <boost/asio.hpp>
using namespace std;

namespace TokyoTyrant
{

    struct HostInfo
    {
        string ip_;
        short port_;
        bool operator==(HostInfo hi)
        {
            return ip_ == hi.ip_ && port_ == hi.port_;
        }
        bool operator!=(HostInfo hi)
        {
            return ip_ != hi.ip_ || port_ != hi.port_;
        }
    };

    class TokyoTyrantClient:
        public boost::enable_shared_from_this<TokyoTyrantClient>
        , public boost::noncopyable
    {
    private:
        TokyoTyrantClient(void):is_running_(false),socket_(ios_){};
        TokyoTyrantClient(const string &ip_str, const short port)
            :is_running_(false),socket_(ios_)
        {
            Connect(ip_str, port);
        };
    public:
        ~TokyoTyrantClient(void){Disconnect();};

    public:
        typedef boost::shared_ptr<TokyoTyrantClient> p;
        static p Create(const string &ip_str, const short port);

    public:
        bool Get(const string &key, string &val);
        bool Set(const string &key, const string &val);
        bool Del(const string &key);
        bool Stat(const string &key, string &val);

        bool Connect(const string &ip_str, const short port);
        bool Disconnect();

        bool IsGoodConnection(){return is_running_;};

        string GetLastErr(){return error_code_.message();};

    public:
        HostInfo host_;

    private:
        boost::asio::io_service ios_;
        boost::system::error_code error_code_;
        boost::asio::ip::tcp::endpoint end_point_;
        boost::asio::ip::tcp::socket socket_;

        bool is_running_;
    };

}
