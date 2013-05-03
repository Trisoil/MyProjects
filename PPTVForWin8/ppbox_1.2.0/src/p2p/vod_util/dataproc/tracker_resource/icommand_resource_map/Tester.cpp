#include "StdAfx.h"
#include "Tester.h"
#include <vector>

Tester::Tester( std::vector <string> *ips , std::vector <int> *ports ) 
{
	this->ips_ = ips;
	this->ports_ = ports;
    this->udp_server_ = new protocol::UdpServer( io_service_ , this );
    protocol::register_tracker_packet( *this->udp_server_ );
	this->msg_index_ = 0;
	this->end_test_ = 0;

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

    asio_timer_manager_ = new framework::timer::AsioTimerManager( io_service_ , boost::posix_time::seconds(1) );
    this->tests_timer_ = new framework::timer::PeriodicTimer( 
        *this->asio_timer_manager_ , 1000 , boost::bind( &Tester::on_timer , this ) );
    this->tests_timer_->start();

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
	int msg_count = 0;
	string test_tracker_ip;
	int test_tracker_port;

	if (end_test_ == 1) return;

	for (msg_count = 0; msg_count < MAX_MSG_PER_SECOND; msg_count ++) {
		if (msg_index_ >= this->ips_->size()) {
			printf("Reached end of tracker list. Exit.\n");
			end_test_ = 1;
			break;
		}
		test_tracker_ip = (*ips_)[msg_index_];
		test_tracker_port = (*ports_)[msg_index_];
		boost::asio::ip::udp::endpoint ep( boost::asio::ip::address_v4::from_string(test_tracker_ip) , test_tracker_port );
		protocol::InternalCommandPacket internalcommand_request_packet(protocol::Packet::NewTransactionID(), 
			INTERNAL_COMMAND_MAGIC_NUMBER, TYPE_ICOMMAND_DUMPRESOURCE, ep);

		printf("icommand packet send to %s  %d\n", test_tracker_ip.c_str(), test_tracker_port);
		this->udp_server_->send_packet( internalcommand_request_packet, protocol::PEER_VERSION );
		msg_index_ ++;
	}
	if (end_test_ == 1) this->io_service_.stop();
}

void Tester::OnUdpRecv(protocol::Packet const & packet)
{
	printf("discard incoming UDP packets.\n");
}
