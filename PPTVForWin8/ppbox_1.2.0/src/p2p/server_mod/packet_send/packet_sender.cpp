#include "packet_sender.h"

#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
//#include <network/UdpServer.h>
#include <boost/array.hpp>

using namespace boost::asio::ip;

#include <boost/asio/buffer.hpp>

#include <framework/string/Uuid.h>
#include <framework/system/BytesOrder.h>

static unsigned long local_ip;
static time_t start_time = time(0);

#include <boost/date_time/posix_time/posix_time.hpp>
#include <framework/network/Interface.h>
#include "protocol/CheckSum.h"
#include <util/buffers/SubBuffers.h>
#include <util/buffers/BufferCopy.h>




packet_sender::packet_sender( std::vector<std::string> & file_list )
{
    this->file_list_ = file_list;
}

packet_sender::~packet_sender(void)
{
}

void packet_sender::run()
{
    // 获取本地IP地址
    //	local_ip = 
    std::vector<framework::network::Interface> interfaces;
    ::framework::network::enum_interface( interfaces );

    for( size_t i = 0; i < interfaces.size(); i++ ) {
        unsigned long tmp_ip = inet_addr( interfaces[i].addr.to_string().c_str() );
        if( tmp_ip == inet_addr( "127.0.0.1" ) )
            continue;
        local_ip = ntohl( tmp_ip );
        break;
    }

    // 
    if( this->file_list_.size() == 0 )
        return;

    // 
    for( size_t i = 0; i < this->file_list_.size(); i++ ) {
        // 
        sender_executer exec( file_list_[i] );
        exec.exec();
    }
}


sender_executer::sender_executer( std::string & filename )
:socket_(io_service_)
{
    file_name_ = filename;
}

sender_executer::~sender_executer()
{
}

void sender_executer::exec()
{
    if( !this->parse_packet() )
        return;

    //
    if (sender_init())
    {
        if (pak_.is_continuously_send)
        {
            this->exec_packet_continuously();
        }
        else
        {
            this->exec_packet();
        }
    }
    
    return;
}

bool sender_executer::parse_packet()
{
    TiXmlDocument doc;
    bool ret = doc.LoadFile( this->file_name_.c_str() );
    if( !ret ) {
        printf("%s could not open\n" , this->file_name_.c_str() );
        return false;
    }

    // 
    TiXmlNode* packet_node = doc.FirstChild("packet");
    if( !packet_node ) {
        return false;
    }

    TiXmlElement* temp_element = 0;
    TiXmlNode* temp_node = 0;

    // packet 节
    temp_element = packet_node->ToElement();
    pak_.name = temp_element->Attribute("name");
    pak_.interval = boost::lexical_cast<int>( temp_element->Attribute("interval") );
    pak_.loop_count = boost::lexical_cast<int>( temp_element->Attribute("loop_count") );
    pak_.every_interval_send = boost::lexical_cast<int>( temp_element->Attribute("every_interval_send") );
    pak_.local_port = boost::lexical_cast<unsigned short>( temp_element->Attribute("local_port") );
    pak_.is_continuously_send = boost::lexical_cast<unsigned short>( temp_element->Attribute("is_continuously_send") );

    // server 节
    temp_element = packet_node->FirstChild("server")->ToElement();
    pak_.server.ip = temp_element->Attribute("ip");
    pak_.server.port = boost::lexical_cast<unsigned short>( temp_element->Attribute("port") );
    pak_.server.protocol = temp_element->Attribute("protocol");

    // struct 节
    temp_node = packet_node->FirstChild("struct");
    for( TiXmlNode* piece_node = temp_node->FirstChild(); piece_node ; piece_node = piece_node->NextSibling() ) {
        // 
        temp_element = piece_node->ToElement();
        if( !temp_element )
            continue;

        // piece 节
        packet::piece pie;

        pie.length = boost::lexical_cast<int>( temp_element->Attribute("length") );
        if( temp_element->Attribute("is_check_sum") ) {
            pie.is_check_sum = boost::lexical_cast<bool>( temp_element->Attribute("is_check_sum") );
        }

        if( temp_element->Attribute("auto_inc") ) {
            pie.auto_inc = boost::lexical_cast<bool>( temp_element->Attribute("auto_inc") );
        }

        if( temp_element->Attribute("random_generate") ) {
            pie.is_random_generate = boost::lexical_cast<bool>( temp_element->Attribute("random_generate") );
        }

        if( temp_element->Attribute("reset_every_internal") ) {
            pie.is_reset_every_internal = boost::lexical_cast<bool>( temp_element->Attribute("reset_every_internal") );
        }

        // name
        const char * v = temp_element->Value();
        if( v ) {
            // 
            pie.name = v;
        }

        v = temp_element->GetText();
        if( v ) {
            pie.value = new char[ strlen(v) + 1 ];
            strcpy( pie.value , v );

            pie.origin_value = new char[ strlen(v) + 1 ];
            strcpy( pie.origin_value , v );
        }

        pak_.packet_pieces_.push_back( pie );
    }

    return true;
}

template<typename T>
void append_value( std::string & buf , T & v )
{
    buf.append( (const char *)&v , sizeof(T) );
}

template<>
void append_value( std::string & buf , framework::string::Uuid & v )
{
    buf.append( (const char *)&v.data() , 16 );
    //	buf.append( (const char *)&v , sizeof(T) );
}

template<typename T>
void process_piece( std::string & buf , packet::piece & p , packet & pak , bool new_interval )
{
    T v = 0;
    if( std::string(p.value) == std::string("{LOCALUDPPORT}") ) {
        v = boost::lexical_cast<int>( pak.local_port );
    } else if( std::string(p.value) == std::string("{LOCALIP}") ) {
        v = local_ip;
    } else if( std::string(p.value) == std::string("{NOW}") ) {
        tm tm1 = boost::posix_time::to_tm(boost::posix_time::second_clock::local_time());
        v = boost::lexical_cast<unsigned long>( (boost::uint32_t)mktime( &tm1 ) ) - 10;
    } else if( std::string(p.value) == std::string("{DURING}") ) {
        v = boost::lexical_cast<unsigned long>( time(0) - start_time );
    } else {
        v = (T)boost::lexical_cast<unsigned long>( std::string(p.value) );
    }

    // 
    if( p.auto_inc ) {
        // 增加
        if( new_interval && p.is_reset_every_internal ) {
            // 需要重置了
            v = boost::lexical_cast<T>( p.origin_value );
        }

        buf.append( (const char *)&v , sizeof(T) );

        v++;
        std::string s = boost::lexical_cast<std::string>( v );

        if( s.size() != strlen( p.value ) ) {
            delete []p.value;
            p.value = new char[ s.size() + 1 ];
        }

        strcpy( p.value , s.c_str() );

    } else {
        buf.append( (const char *)&v , sizeof(T) );
    }
}

bool sender_executer::sender_init()
{
    udp::resolver resolver(io_service_);
    udp::resolver::query query(udp::v4(), pak_.server.ip , "" );
    remote_endpoint_ = *resolver.resolve(query);
    remote_endpoint_.port( pak_.server.port );
    udp::endpoint local_endpoint;

    if( pak_.local_port ) 
    {
        local_endpoint.port( pak_.local_port );
    }
    else 
    {
        return false;
    }

    socket_.open(udp::v4() );
    socket_.bind( local_endpoint );
    return true;
}

bool sender_executer::exec_packet()
{
    int total_sended = 0;

    while( true ) {
        // 一直执行
        bool new_interval = true;

        for( size_t i = 0; i < pak_.every_interval_send; i++ ) 
        {
            send_packet(new_interval);            
            total_sended++;
            new_interval = false;
        }

        // sleep
#ifdef BOOST_WINDOWS_API
        Sleep( pak_.interval * 1000 );
#else 
        sleep(pak_.interval);
#endif

        if( total_sended >= pak_.loop_count * pak_.every_interval_send )
            break;
    }

    return true;
}

bool sender_executer::exec_packet_continuously()
{
    // 执行
#ifdef BOOST_WINDOWS_API
    boost::uint32_t every_packet_interval_in_milliseconds = pak_.interval * 1000000 / pak_.every_interval_send;
    LARGE_INTEGER m_liPerfFreq={0};
    QueryPerformanceFrequency(&m_liPerfFreq);
    LARGE_INTEGER last_time={0};
    QueryPerformanceCounter(&last_time);
    LARGE_INTEGER current_time={0};

#else
    boost::uint32_t every_packet_interval_in_microseconds = pak_.interval * 1000000000 / pak_.every_interval_send;
    timespec last_time;
    timespec current_time;
    clock_gettime(CLOCK_REALTIME, &last_time);

#endif

    int total_sended = 0;
    bool new_interval = true;
    while( true ) 
    {

#ifdef BOOST_WINDOWS_API

        QueryPerformanceCounter(&current_time);
   
        int time=( ((current_time.QuadPart - last_time.QuadPart) * 1000000)/m_liPerfFreq.QuadPart);
        
        if (time < every_packet_interval_in_milliseconds)
            continue;

        last_time = current_time;
#else

        clock_gettime(CLOCK_REALTIME, &current_time);
        int duration = (current_time.tv_sec - last_time.tv_sec) * 1000000000 + (current_time.tv_nsec - last_time.tv_nsec);
        if (duration < every_packet_interval_in_microseconds)
        {
            continue;
        }

        last_time = current_time;
#endif
        if (total_sended % pak_.every_interval_send == 0)
        {
            new_interval = true;
        }
        
        send_packet(new_interval);
        total_sended++;

        new_interval = false;
       
        if( total_sended >= pak_.loop_count * pak_.every_interval_send )
            break;
    }

    return true;
}

void sender_executer::send_packet(bool new_interval)
{
    bool is_caculate_check_sum = false;


    std::string buf;
    for( size_t i = 0; i < pak_.packet_pieces_.size(); i++ ) {
        packet::piece & p = pak_.packet_pieces_[i];

        if( p.is_random_generate ) {
#ifdef BOOST_WINDOWS_API
            srand( ::GetTickCount() );
#else
            timespec current_time;
            clock_gettime(CLOCK_REALTIME, &current_time);
            int duration = current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000;
            srand(duration);
#endif
            for( size_t i = 0; i < p.length; i++ ) {
                boost::uint8_t v = rand() % 0xFF;
                append_value( buf , v );
            }

        } else if( p.is_check_sum ) {
            // 计算 check sum
            boost::uint32_t v = 0;
            append_value( buf , v );
            is_caculate_check_sum = true;
        } else {
            switch( p.length ) {
                case 1:
                    // unsigned char
                    {
                        process_piece<boost::uint8_t>( buf , p , pak_  , new_interval );
                    }
                    break;
                case 2:
                    // unsigned short
                    process_piece<boost::uint16_t>( buf , p , pak_ , new_interval );
                    break;
                case 4:
                    process_piece<boost::uint32_t>( buf , p , pak_ , new_interval );
                    break;
                case 16:
                    // guid
                    {
                        framework::string::Uuid id;
                        id.from_string( p.value );
                        append_value( buf , id );
                    }
                    break;
                default:
                    // 出错，不能兼容的类型大小
                    printf("unsupported data type: %s\n" , p.name.c_str() );
                    break;
            }
        }
    }


    boost::asio::const_buffers_1 send_buffer(boost::asio::buffer(buf));
    if (is_caculate_check_sum)
    {
        boost::uint32_t &real_check_sum = const_cast<boost::uint32_t &>(
            *boost::asio::buffer_cast<boost::uint32_t const *>(*send_buffer.begin()));
        real_check_sum = check_sum_new(util::buffers::sub_buffers(send_buffer,4));         
    }

    int sended = socket_.send_to( send_buffer , remote_endpoint_ );
}
