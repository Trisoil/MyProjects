//-------------------------------------------------------------
//     Copyright (c) 2012 PPLive Inc.  All rights reserved.
//-------------------------------------------------------------
#include "PushClient.h"
#include "PushPressHandler.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>

using namespace std;
boost::asio::io_service g_ios;
ns_pplive::ns_push::PushClient client;

unsigned int g_total_send = 0;
unsigned int g_total_recv = 0;

unsigned int g_send_last = 0;
unsigned int g_recv_last = 0;
//boost::mutex g_mu;

void FillPlayHistoryVec(vector<PlayHistoryItem>& play_history_vec)
{
    ifstream fin("play_history.txt");
    while (!fin.eof())
    {
        PlayHistoryItem item;
        fin >> item.video_name_;
        fin >> item.downloaded_segment_num_;
        fin >> item.continuously_played_duration_;
        play_history_vec.push_back(item);
    }

    fin.close();

    //if (play_history_vec.size() > 16)
    //{
    //    play_history_vec.resize(16);
    //}
}

void QueryPushTask(int pversion, string ip, unsigned short port, int peer_bw, string peer_id, int peer_space, int nat_type, int online_percent)
{
    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);
    vector<PlayHistoryItem> play_history_vec;
    FillPlayHistoryVec(play_history_vec);
    vector<PushTaskItem> push_task_vec;

    boost::uint8_t error_code = 0;
    cout << "QueryPush: " << ip <<":" << port << " peer-bw " << peer_bw << " used_space=" << peer_space << endl;
    int ret = client.QueryPushTask(pversion, g_ios, endpoint_, play_history_vec,boost::uint32_t(rand()), Guid(peer_id), peer_bw, peer_space, peer_bw, 2*1024*1024, nat_type, online_percent, push_task_vec, error_code, 3);
    if (0 != ret)
    {
        cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
        return;
    }
    cout<<"push task count:"<<push_task_vec.size()<<endl;
    for(unsigned i=0;i<push_task_vec.size();++i)
    {
        cout << endl;
        cout << "RID: " << push_task_vec[i].rid_info_.rid_ << endl;
        cout << "URL: " << push_task_vec[i].url_ << endl;
        cout << "Refer Url: " << push_task_vec[i].refer_url_ << endl;
        cout << "channel_id_" << push_task_vec[i].channel_id_ << endl;
    }
}

void QueryPushTask1(int pversion, string ip, unsigned short port, string peer_id)
{
    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);
    vector<PlayHistoryItem> play_history_vec;
    FillPlayHistoryVec(play_history_vec);
    
    for (vector<PlayHistoryItem>::iterator it = play_history_vec.begin(); it != play_history_vec.end(); ++it)
    {
        vector<PushTaskItem> push_task_vec;
        vector<PlayHistoryItem> play_history_vec1;
        play_history_vec1.push_back(*it);

        boost::uint8_t error_code = 0;
        int ret = client.QueryPushTask(pversion, g_ios, endpoint_, play_history_vec1,boost::uint32_t(rand()), Guid(peer_id), 100, 0, 100, 2*1024*1024, 0, 50, push_task_vec, error_code, 3);
        if (0 != ret)
        {
            cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
            return;
        }
        cout<< it->video_name_ << "   push task count:"<<push_task_vec.size()<<endl;
        if (push_task_vec.empty())
        {
            continue;
        }

        //for(unsigned i=0;i<push_task_vec.size();++i)
        //{
        //    cout << endl;
        //    cout << "RID: " << push_task_vec[i].rid_info_.rid_ << endl;
        //    cout << "URL: " << push_task_vec[i].url_ << endl;
        //    cout << "Refer Url: " << push_task_vec[i].refer_url_ << endl;
        //    cout << "channel_id_" << push_task_vec[i].channel_id_ << endl;
        //}
    }
}

void run()
{
    while(1)
    {
#ifdef BOOST_WINDOWS_API
        ::Sleep(1000);
#else
        sleep(1);
#endif
        cout << "send: " << g_total_send 
            << "\trecv: " << g_total_recv
            << "\t丢包数: " << static_cast<int>(g_total_send - g_total_recv)
            <<"\t收包率: " <<g_total_recv*1.0/g_total_send
            <<"\t平均发包: " << g_total_send - g_send_last
            <<"\t平均收包：" << g_total_recv - g_recv_last
            << endl;

        g_send_last = g_total_send;
        g_recv_last = g_total_recv;
//        g_mu.lock();
//        g_total_send = 0;
//        g_total_recv = 0;
//        g_mu.unlock();
    }
}

int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        cout << "usage: <.exe mod ip port arg4 arg5 nat_type online_prt>" << endl;
        cout << "\t mod 1: V3, one time test. arg4=peerbw, arg5=used_space_in_bytes, nat_type, online_percent(0-100)" << endl;
        cout << "\t mod 2: V2, one time test. arg4=peerbw, arg5=used_space_in_bytes" << endl;
        cout << "\t mod 3: pressure test. arg4=NumOfThreads" << endl;
        return -1;
    }

    if (atoi(argv[1]) == 1 || atoi(argv[1]) == 2) {
        int peer_space = 0;
        int nat_type = 0;
        int online_percent = 0;
        if (argc >=6 ) {
            peer_space = atoi(argv[5]);
        }
        if (argc >=7 ) {
            nat_type = atoi(argv[6]);
        }
        if (argc >=8 ) {
            online_percent = atoi(argv[7]);
        }
        int pversion = 3;
        if (atoi(argv[1]) == 2) {
            pversion = 2;
        }
        QueryPushTask(pversion, argv[2], atoi(argv[3]), atoi(argv[4]), "123456", peer_space, nat_type, online_percent);
    }else if (atoi(argv[1]) == 3 || atoi(argv[1]) == 4) {  
        boost::thread t(run);
        std::string ip = argv[2];
        unsigned int port = atoi(argv[3]);
        boost::asio::ip::address_v4 av4;
        av4 = boost::asio::ip::address_v4::from_string(ip);
        boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address(av4),port);

        int thread_count = atoi(argv[4]);
        ns_pplive::ns_push::PushPressHandler handler(thread_count,endpoint);
        handler.StartThreads();
        handler.WaitThreadsTermination();
    }

	return 0;
}