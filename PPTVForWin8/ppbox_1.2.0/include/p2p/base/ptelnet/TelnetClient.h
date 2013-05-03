//TelnetClient.h

#ifndef _PTELNET_TELNETCLIENT_H_
#define _PTELNET_TELNETCLIENT_H_

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "ptelnet/CodeConvert.h"
#include "ptelnet/LoginParse.h"
#include "ptelnet/ArgumentParse.h"

using boost::asio::ip::tcp;
using namespace std;

namespace ptelnet
{
     /* Telnet采用带内信令的方式
     * 0xff（255）叫做IAC(interpret as command)，该字节后面的一个字节才是命令字节
     */
    const unsigned char IAC = 255;   //Interpret as command
    const unsigned char DO = 253;    //选项协商
    const unsigned char DONT = 254;  //选项协商
    const unsigned char WILL = 251;  //选项协商
    const unsigned char WONT = 252;  //选项协商
    const unsigned char SB = 250;    //子选项开始
    const unsigned char SE = 240;    //子选项结束
    const unsigned char IS = '0'; 
    const unsigned char SEND = '1'; 
    const unsigned char INFO = '2'; 
    const unsigned char VAR = '0';
    const unsigned char VALUE = '1';
    const unsigned char ESC = '2';
    const unsigned char USERVAR = '3';

    class TelnetClient
    {
    public:
        enum { max_read_length = 512 };
        enum TelnetStat
        {
            Telnet_Stat_Inner,
            Telnet_Stat_Run,
            Telnet_Stat_RunComplete,
            Telnet_Stat_ExecScript,
            Telnet_Stat_ExecScriptComplete,
        };

        TelnetClient(boost::asio::io_service& io_service
            ,tcp::resolver::iterator endpoint_iterator
            ,ArgumentParse::ArgumentInfo arguinfo
            ,LoginParse::LoginInfo logininfo);
        void write(const char msg);
        void close();

    private:
        void connect_start(
            tcp::resolver::iterator endpoint_iterator);
        void connect_complete(
            const boost::system::error_code& error, 
            tcp::resolver::iterator endpoint_iterator);
        void read_start();
        void read_complete(
            const boost::system::error_code& error,
            size_t bytes_transferred);
        void do_write(const unsigned char msg);
        void do_write(const std::string msg);
        void do_close();
        void change_stat();
        void reset_stat();
        bool getline( unsigned char * bytes, int nBytes, int& ndx );
        void process_options();
        void respond_options();
        void arrange_reply(std::string strOption);
        void resopnse();

    private:
        tcp::socket socket_; 
        unsigned char read_msg_[max_read_length]; 
        ArgumentParse::ArgumentInfo arguinfo_;
        LoginParse::LoginInfo logininfo_;
        size_t input_;
        std::string cmd_;
        TelnetStat stat_;
        size_t cmdnum_;
        std::string cmdkey_;
        std::string m_strLine;
        std::string m_strNormalText;
        std::vector<std::string> m_ListOptions;
        std::string m_strResp;
    };
} //namespace ptelnet

#endif