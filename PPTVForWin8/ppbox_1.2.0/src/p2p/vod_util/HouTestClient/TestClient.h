//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _HOU_TEST_CLIENT_H_
#define _HOU_TEST_CLIENT_H_

class AsioServiceRunner;

class TestClient
    :public boost::enable_shared_from_this<TestClient>
{
public:
    TestClient(boost::uint32_t send_num)
        :send_num_in_1s_(send_num)
    {}

    void Start();
    void Stop();
    void SendTo();

private:
    boost::shared_ptr<boost::asio::ip::udp::socket> socket_;
    boost::shared_ptr<boost::asio::io_service> io_service_;
    boost::shared_ptr<AsioServiceRunner> service_runner_;
    std::string server_address_;
    boost::uint16_t server_port_;
    boost::shared_ptr<boost::asio::deadline_timer> timer_;
    boost::uint32_t send_num_in_1s_;
    std::string request_url_;
    int interval_;
    int packet_send_;
};

#endif