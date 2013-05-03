#include "StdAfx.h"
#include "Tester.h"

Tester::Tester( char * ip , unsigned short port ) : test_tracker_ip_(ip) , test_tracker_port_(port) , retry_count_(0) , test_result_(0)
{
    this->udp_server_ = new protocol::UdpServer( io_service_ , this );
    protocol::register_tracker_packet( *this->udp_server_ );

}

Tester::~Tester(void)
{
}

bool Tester::start()
{
    // 随机绑定端口
    local_udp_port_ = 19050;
    if( false == udp_server_->Listen(local_udp_port_) )
    {
        printf("bind port failed: %d\n" , local_udp_port_);
        throw "bind port";
    }

    this->udp_server_->Recv(100);

    my_guid_.generate();

    asio_timer_manager_ = new framework::timer::AsioTimerManager( io_service_ , boost::posix_time::seconds(1) );
    asio_timer_manager_->start();

    this->tests_timer_ = new framework::timer::PeriodicTimer( 
        *this->asio_timer_manager_ , 1000 , boost::bind( &Tester::on_timer , this ) );
    this->tests_timer_->start();

    RID test_rid;
    test_rid.generate();
    local_resource_set_.insert( test_rid );

    protocol::REPORT_RESOURCE_STRUCT test_add_rid;
    test_add_rid.ResourceID = test_rid;
    test_add_rid.Type = 1; // for add
    update_resource_set_.push_back( test_add_rid );

    test_status_ = want_test_report;
    this->io_service_.run();
    // 
    return true;
}

bool Tester::stop()
{
    return true;
}

void Tester::on_timer()
{
    switch( this->test_status_ ) {
        case want_test_report:
            // 开始测试吧
            {

                std::vector<uint32_t> real_ips;
                real_ips.push_back( ::inet_addr( "192.168.43.6" ) );

                boost::asio::ip::udp::endpoint ep( boost::asio::ip::address_v4::from_string(test_tracker_ip_) , test_tracker_port_ );

                protocol::ReportPacket report_request(
                    protocol::Packet::NewTransactionID(),
                    protocol::PEER_VERSION,
                    my_guid_ ,
                    local_resource_set_.size(),
                    0,
                    local_udp_port_,
                    protocol::PEER_VERSION,
                    0,
                    0,
                    0,
                    real_ips, 
                    update_resource_set_,
                    // nightsuns: 服务器固定的NAT类型是 PUBLIC
                    //NAT_PUBLIC,
                    5 ,
                    // nightsuns: 服务器暂时硬编码
                    255 ,
                    60 * 24 ,
                    //			app->GenUploadPriority(),
                    //			app->GetIdleTimeInMins(),
                    0 , 
                    0 , 
                    0 ,
                    ep
                    );

                this->udp_server_->send_packet( report_request , report_request.peer_version_ );

                test_status_ = testing_report;

                test_start_time_ = ::GetTickCount();
            }
            break;
        case testing_report:
            {
                // 如果一段时间还没有收到，发送3次包
                if( ::GetTickCount() - test_start_time_ > 3000 ) {
                    // 1秒还未收到，这里继续发一个
                    retry_count_ ++;

                    if( retry_count_ > 3 ) {
                        // 超过了三次出错
                        //this->test_status_ = ending_test;
                        this->test_status_ = want_test_icommand;
                        this->test_result_ = -1;
                    } else {
                        test_status_ = want_test_report;
                    }

                    this->on_timer();
                }
            }
            break;
        case want_test_list:
            {
                // 测试 list 在这里
                boost::asio::ip::udp::endpoint ep( boost::asio::ip::address_v4::from_string(test_tracker_ip_) , test_tracker_port_ );
                protocol::ListPacket list_request_packet(protocol::Packet::NewTransactionID(), 
                    protocol::PEER_VERSION, *local_resource_set_.begin(), this->my_guid_, 50, ep);
                
                this->udp_server_->send_packet( list_request_packet , list_request_packet.peer_version_ );

                test_status_ = testing_list;

                test_start_time_ = ::GetTickCount();
            }
            break;
        case testing_list:
            {
                // 如果一段时间还没有收到，发送3次包
                if( ::GetTickCount() - test_start_time_ > 3000 ) {
                    // 1秒还未收到，这里继续发一个
                    retry_count_ ++;

                    if( retry_count_ > 3 ) {
                        // 超过了三次出错
                        this->test_status_ = ending_test;
                        this->test_result_ = -1;
                    } else {
                        test_status_ = want_test_list;
                    }

                    this->on_timer();
                }
            }
            break;
        case want_test_icommand:
            {
                boost::asio::ip::udp::endpoint ep( boost::asio::ip::address_v4::from_string(test_tracker_ip_) , test_tracker_port_ );
                protocol::InternalCommandPacket internalcommand_request_packet(protocol::Packet::NewTransactionID(), 
                    INTERNAL_COMMAND_MAGIC_NUMBER, TYPE_ICOMMAND_DUMPRESOURCE, ep);
                
                printf("test icommand packet send\n");
                this->udp_server_->send_packet( internalcommand_request_packet, protocol::PEER_VERSION );

                test_status_ = testing_icommand;

                test_start_time_ = ::GetTickCount();
            }
            break;
        case testing_icommand:
            {
                // 如果一段时间还没有收到，发送3次包
                if( ::GetTickCount() - test_start_time_ > 3000 ) {
                    // 1秒还未收到，这里继续发一个
                    retry_count_ ++;

                    if( retry_count_ > 3 ) {
                        // 超过了三次出错
                        this->test_status_ = ending_test;
                        this->test_result_ = -1;
                    } else {
                        test_status_ = want_test_icommand;
                    }

                    this->on_timer();
                }
            }
            break;
        case ending_test:
            {
                this->io_service_.stop();
            }
            break;
    }
}

void Tester::OnUdpRecv(protocol::Packet const & packet)
{
    switch( this->test_status_ ) {
        case want_test_report:
            {
            }
            break;
        case testing_report:
            {
                // 返回了
                if( packet.PacketAction == protocol::ReportPacket::Action ) {
                    protocol::ReportPacket &report_packet = (protocol::ReportPacket &)packet;

                    // 对了
                    printf("testing report returned ok\n");
                    this->retry_count_ = 0;

                    this->test_status_ = want_test_list;
                    this->on_timer();
                }
            }
            break;
        case want_test_list:
            {
            }
            break;
        case testing_list:
            {
                // 
                if( packet.PacketAction == protocol::ListPacket::Action ) {
                    // 对了
                    protocol::ListPacket & list_packet = (protocol::ListPacket &)packet;
                    this->retry_count_ = 0;

                    printf("list command returned ok\n");
                    this->test_status_ = want_test_icommand;

                    // 断言资源数一定大于0
                    if( list_packet.response.peer_infos_.size() ) {
                        this->test_result_ = 1;
                    } else {
                        this->test_result_ = -1;
                    }

                    this->on_timer();
                }
            }
            break;
        case want_test_icommand:
            {
            }
            break;
        case testing_icommand:
            {
            }
            break;
        case ending_test:
            {
            }
            break;
    }
}
