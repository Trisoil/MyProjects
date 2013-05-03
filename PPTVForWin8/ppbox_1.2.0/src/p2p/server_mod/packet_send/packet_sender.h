#pragma once

#include <vector>
#include <string>
#include "boost/asio.hpp"
#include <tinyxml/tinyxml.h>

struct packet
{
    packet() : interval(0) , every_interval_send(0) , local_port(5555)
    {
    }

    ~packet()
    {
    }

    std::string name;
    int interval;
    int every_interval_send;
    int loop_count;
    bool is_continuously_send;
    unsigned short local_port;

    struct
    {
        std::string ip;
        int port;
        std::string protocol;
    } server;

    struct piece
    {
        piece() : length(0) , is_check_sum(0) , auto_inc(0) , is_random_generate(0) , value(0) , origin_value(0) , is_reset_every_internal(false)
        {
        }

        ~piece()
        {
            //delete []value;
        }

        int length;
        bool is_check_sum;
        bool auto_inc;
        bool is_random_generate;
        bool is_reset_every_internal;
        char * origin_value;
        char * value;
        std::string name;
    };

    std::vector<piece> packet_pieces_;
};


class sender_executer
{
public:
    sender_executer( std::string & filename );
    ~sender_executer();

    void exec();
private:
    bool parse_packet();
    bool exec_packet();
    bool exec_packet_continuously();
    bool sender_init();
    void send_packet(bool is_new_interval);

private:
    std::string file_name_;
    boost::asio::io_service io_service_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    packet pak_;
};

class packet_sender
{
public:
    packet_sender( std::vector<std::string> & file_list );
    ~packet_sender(void);

    void run();

private:
    std::vector<std::string> file_list_;
};
