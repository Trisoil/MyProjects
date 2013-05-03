//-------------------------------------------------------------
//     Copyright (c) 2011 PPLive Inc.  All rights reserved.
//-------------------------------------------------------------

#include "TrackerClient.h"
#include <iostream>
using namespace std;
boost::asio::io_service g_ios;
ns_pplive::ns_tracker::TrackerClient client;

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <fstream>

log4cplus::Logger g_logger;
int g_argc = 0;
char** g_argv;
int g_argindex = 1;
void readcin(string& value);
void readcin(int& value);
void readcin(double& value);

struct ProcessInternalCommandArgs
{
    int cmd_;
    string ip_;
    unsigned short port_;
    int isprint_;
    unsigned short gap_;
    unsigned short times_;

    bool is_init_;

    ProcessInternalCommandArgs() : is_init_(false)
    {

    }
    void LoadArgs(std::string filename)
    {
        ifstream fin(filename.c_str());
        if (!fin.is_open())
        {
            is_init_ = false;
            return ;
        }
        fin >> cmd_;
        fin >> ip_;
        fin >> port_;
        fin >> isprint_;
        fin >> gap_;
        fin >> times_;

        is_init_ = true;

        fin.close();
    }

    void output()
    {
        cout << "cmd:" << cmd_ << " ip:" << ip_ << " port:" << port_ << " isprint:" << isprint_ << " gap:" << gap_ << " times:" << times_ << endl; 
    }
};

void ProcessList()
{
    string rid;
    //cin>>rid;
    readcin(rid);
    string ip;
    cin>>ip;
    unsigned short port;
    cin>>port;
    string pid;
    cin>>pid;	

    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);
    vector<CandidatePeerInfo> candidate_peer_info;


    if (0 != client.ListPeer(candidate_peer_info,boost::uint32_t(rand()),RID(rid),Guid(pid),boost::uint16_t(50),endpoint_,g_ios,2.0))
    {
        cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
        return;
    }
    cout<<"peercount:"<<candidate_peer_info.size()<<endl;
    for(unsigned i=0;i<candidate_peer_info.size();++i)
    {
        //cout<<boost::asio::ip::address_v4((candidate_peer_info[i].DetectIP)).to_string()<<":"<<candidate_peer_info[i].DetectUdpPort<<" priotity:"<< int(candidate_peer_info[i].TrackerPriority)<<endl;
        cout<<"index:"<<i<<":"<<candidate_peer_info[i]<<endl;
    }

    //cin>>rid;
}

void ProcessQueryPeerCount()
{
    string rid;
    cin>>rid;
    string ip;
    cin>>ip;
    unsigned short port;
    cin>>port;

    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);

    //int QueryPeerCount(boost::uint32_t& peer_count,boost::uint32_t transaction_id,const RID& resource_id,const boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);
    uint32_t peer_count;

    if (0 != client.QueryPeerCount(peer_count,boost::uint32_t(rand()),RID(rid),endpoint_,g_ios,2.0))
    {
        cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
        return;
    }
    cout<<"peercount:"<<peer_count<<endl;	
    //cin>>rid;
};

void ProcessQueryResources()
{
    string peer_id;
    cin>>peer_id;
    string ip;
    cin>>ip;
    unsigned short port;
    cin>>port;

    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);

    vector<RID> recources;

    if (0 != client.QueryPeerResources(recources,boost::uint32_t(rand()),RID(peer_id),endpoint_,g_ios,2.0))
    {
        cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
        return;
    }

    cout<<"resource count:"<<recources.size()<<endl;
    for (unsigned i=0;i<recources.size();++i)
    {
        cout<<"rid "<<i<<": "<<recources[i]<<endl;
    }

};

void ProcessQueryTrackerStatistic()
{
    string ip;
    cin>>ip;
    unsigned short port;
    cin>>port;

    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);

    cout<<"ip:"<<ip<<" port:"<<port<<endl;

    string statistic;

    if (0 != client.QueryTrackerStatistic(statistic,boost::uint32_t(rand()),endpoint_,g_ios,2.0))
    {
        cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
        return;
    }
    cout<<"tracker statistic:"<<endl<<statistic<<endl;

}

void ProcessListTcp()
{
    string rid;
    cin>>rid;
    string ip;
    cin>>ip;
    unsigned short port;
    cin>>port;
    string pid;
    cin>>pid;	

    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);
    vector<CandidatePeerInfo> candidate_peer_info;


    if (0 != client.ListPeerTcp(candidate_peer_info,boost::uint32_t(rand()),RID(rid),Guid(pid),boost::uint16_t(50),endpoint_,g_ios,2.0))
    {
        cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
        return;
    }
    cout<<"peercount:"<<candidate_peer_info.size()<<endl;
    for(unsigned i=0;i<candidate_peer_info.size();++i)
    {
        cout<<boost::asio::ip::address_v4((candidate_peer_info[i].DetectIP)).to_string()<<":"<<candidate_peer_info[i].DetectUdpPort<<" priotity:"<< int(candidate_peer_info[i].TrackerPriority)<<endl;
    }

    //cin>>rid;
}

void ProcessListTcpWithIp()
{
    string rid;
    cin>>rid;
    string ip;
    cin>>ip;
    unsigned short port;
    cin>>port;
    string pid;
    cin>>pid;	
    unsigned requestip;
    cin>>requestip;

    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);
    vector<CandidatePeerInfo> candidate_peer_info;

    if (0 != client.ListPeerTcpWithIp(candidate_peer_info,boost::uint32_t(rand()),RID(rid),Guid(pid),boost::uint16_t(50),requestip,endpoint_,g_ios,2.0))
    {
        cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
        return;
    }
    cout<<"peercount:"<<candidate_peer_info.size()<<endl;
    for(unsigned i=0;i<candidate_peer_info.size();++i)
    {
        cout<<boost::asio::ip::address_v4((candidate_peer_info[i].DetectIP)).to_string()<<":"<<candidate_peer_info[i].DetectUdpPort<<" priotity:"<< int(candidate_peer_info[i].TrackerPriority)<<endl;
    }
}

void ProcessListWithIp()
{
    string rid;
    cin>>rid;
    string ip;
    cin>>ip;
    unsigned short port;
    cin>>port;
    string pid;
    cin>>pid;	
    unsigned requestip;
    cin>>requestip;

    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);
    vector<CandidatePeerInfo> candidate_peer_info;

    if (0 != client.ListPeerWithIp(candidate_peer_info,boost::uint32_t(rand()),RID(rid),Guid(pid),boost::uint16_t(50),requestip,endpoint_,g_ios,2.0))
    {
        cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
        return;
    }
    cout<<"peercount:"<<candidate_peer_info.size()<<endl;
    for(unsigned i=0;i<candidate_peer_info.size();++i)
    {
        cout<<boost::asio::ip::address_v4((candidate_peer_info[i].DetectIP)).to_string()<<":"<<candidate_peer_info[i].DetectUdpPort<<" priotity:"<< int(candidate_peer_info[i].TrackerPriority)<<endl;
    }
}


void ProcessListAll()
{
    string rid;
    cin>>rid;
    string pid;
    cin>>pid;	
    string iplistfile;
    cin>>iplistfile;
//文件格式为：每行都是 118.123.201.105:18003    
    ifstream loadfile_stream;
    loadfile_stream.open(iplistfile.c_str());
    if (loadfile_stream.fail())
    {
        cout<<"read file failed"<<endl;
        return;
    }
    string outfile;
    cin>>outfile;

    ofstream outfilestream;
    outfilestream.open(outfile.c_str());
    if(outfilestream.fail())
    {
        cout<<"out file failed"<<endl;
        return;
    }

    std::string line;
    while (getline(loadfile_stream,line)) 
    {
        size_t pos = line.find(":");
        if(pos == string::npos)
        {
            continue;
        }
        else
        {
            string ip = line.substr(0,pos);
            unsigned short port = atoi(line.substr(pos+1).c_str());
            boost::asio::ip::address_v4 av4;
            av4 = boost::asio::ip::address_v4::from_string(ip);
            boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);
             vector<CandidatePeerInfo> candidate_peer_info;
            if (0 != client.ListPeer(candidate_peer_info,boost::uint32_t(rand()),RID(rid),Guid(pid),boost::uint16_t(50),endpoint_,g_ios,1.0))
                if (0 != client.ListPeer(candidate_peer_info,boost::uint32_t(rand()),RID(rid),Guid(pid),boost::uint16_t(50),endpoint_,g_ios,1.0))
                    if (0 != client.ListPeer(candidate_peer_info,boost::uint32_t(rand()),RID(rid),Guid(pid),boost::uint16_t(50),endpoint_,g_ios,1.0))
                    {
                        //cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
                        //return;
                        outfilestream<<"process  failed,msg"<<client.GetErrMsg()<<endl;
                        continue;
                    }
            outfilestream<<endpoint_<<endl; 

            for (unsigned i = 0;i<candidate_peer_info.size();++i)
            { 
                outfilestream<<boost::asio::ip::address_v4((candidate_peer_info[i].DetectIP)).to_string()<<":"<<candidate_peer_info[i].DetectUdpPort<<" priority:"<<int(candidate_peer_info[i].TrackerPriority)
                    <<" version:"<<candidate_peer_info[i].PeerVersion<<endl;
            }

            outfilestream.flush();
            cout<<endpoint_<<" peercount:"<<candidate_peer_info.size()<<endl;            
        }
    }
}

void ProcessListRidsAll()
{
    string iplistfile;
    string ridfile;
    cin>>ridfile;
    ifstream ridfile_stream;
    set<string> rids;
    ridfile_stream.open(ridfile.c_str());
    if (ridfile_stream.fail())
    {
        cout<<"read file failed"<<endl;
        return;
    }
    string tmprid;
    while(getline(ridfile_stream,tmprid))
    {
        rids.insert(tmprid);
    }

    cin>>iplistfile;
    //文件格式为：每行都是 118.123.201.105:18003    
    string outfile;
    cin>>outfile;
    ofstream outfilestream;
    outfilestream.open(outfile.c_str());
    if(outfilestream.fail())
    {
        cout<<"out file failed"<<endl;
        return;
    }
    
    map<string,unsigned> rid_peercount;
    for(set<string>::iterator it = rids.begin();it != rids.end();++it)
    {
        ifstream loadfile_stream;
        loadfile_stream.open(iplistfile.c_str());
        if (loadfile_stream.fail())
        {
            cout<<"read file failed"<<endl;
            return;
        }
        //cout<<"process rid:"<<*it<<endl;
        std::string line;        
        while (getline(loadfile_stream,line)) 
        {
            size_t pos = line.find(":");
            if(pos == string::npos)
            {
                continue;
            }
            else
            {
                string ip = line.substr(0,pos);
                unsigned short port = atoi(line.substr(pos+1).c_str());
                boost::asio::ip::address_v4 av4;
                av4 = boost::asio::ip::address_v4::from_string(ip);
                boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);
                vector<CandidatePeerInfo> candidate_peer_info;
                //这里用的rid冒充peerid，以防被发现频率异常。
                if (0 != client.ListPeer(candidate_peer_info,boost::uint32_t(rand()),RID(*it),Guid(*it),boost::uint16_t(50),endpoint_,g_ios,1.0))
                {
                    //cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
                    //return;
                }
               // cout<<endpoint_<<" peercount:"<<candidate_peer_info.size()<<endl;     
                rid_peercount[*it] += candidate_peer_info.size();

            }
        }
        cout<<"rid:"<<*it<<" totalcount:"<<rid_peercount[*it]<<endl;
        outfilestream<<*it<<" "<<rid_peercount[*it]<<endl;
        outfilestream.flush();
        loadfile_stream.close();
    }
    
}

void ProcessInternalCommand()
{
    ProcessInternalCommandArgs args;
    args.LoadArgs("args.txt");
    args.output();
    if (!args.is_init_)
    {
        cout << "invalid args!" << endl;
        return ;
    }
    string ip = args.ip_;

    unsigned short port = args.port_;

    boost::uint16_t command_type = args.isprint_;

    if (command_type)
    {
        command_type = ENABLE_PRINT_RID_PEER_STATISTIC;
    }
    else
    {
        command_type = DISABLE_PRINT_RID_PEER_STATISTIC;
    }

    boost::uint16_t gap_time = args.gap_;
    boost::uint16_t times = args.times_;

    boost::uint32_t arg = 0;
    arg = ((arg | gap_time) << 16) | times;

    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);

    if (0 != client.InternalCommand(boost::uint32_t(rand()), endpoint_, command_type, arg, g_ios,2.0))
    {
        cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
        return;
    }

    cout << "Command is ok!" << endl;
}

void ProcessListRidIp()
{
    string ip;
    string ridfile;
    //cin>>ridfile;
    readcin(ridfile);
    ifstream ridfile_stream;
    set<string> rids;
    ridfile_stream.open(ridfile.c_str());
    if (ridfile_stream.fail())
    {
        cout<<"read file failed"<<endl;
        return;
    }
    string tmprid;
    while(getline(ridfile_stream,tmprid))
    {
        rids.insert(tmprid);
    }

    //cin>>ip;
     readcin(ip);
    int port =0;
    //cin>>port;
     readcin(port);

    double timeout=0.1;
   // cin>>timeout;
     readcin(timeout);

    //文件格式为：每行都是 118.123.201.105:18003    
    string outfile;
    //cin>>outfile;
     readcin(outfile);
    ofstream outfilestream;
    outfilestream.open(outfile.c_str());
    if(outfilestream.fail())
    {
        cout<<"out file failed"<<endl;
        return;
    }

    map<string,unsigned> rid_peercount;
    for(set<string>::iterator it = rids.begin();it != rids.end();++it)
    {
        boost::asio::ip::address_v4 av4;
        av4 = boost::asio::ip::address_v4::from_string(ip);
        boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);
        vector<CandidatePeerInfo> candidate_peer_info;
        //这里用的rid冒充peerid，以防被发现频率异常。
        if (0 != client.ListPeer(candidate_peer_info,boost::uint32_t(rand()),RID(*it),Guid(*it),boost::uint16_t(50),endpoint_,g_ios,timeout))
        {
            if (0 != client.ListPeer(candidate_peer_info,boost::uint32_t(rand()),RID(*it),Guid(*it),boost::uint16_t(50),endpoint_,g_ios,timeout))
            {
                continue;
            }
        }
        outfilestream<<*it<<" "<<candidate_peer_info.size()<<endl;
        outfilestream.flush();
    }
    outfilestream.close();
}

bool TestATracker(unsigned short port)
{
    //验证list，report是否正常
    //先上报一个奇怪的资源，然后去list，期待只得到一个结果。
    boost::uint16_t keepalive_interval;
    boost::uint32_t detected_ip;
    boost::uint16_t detected_udp_port;
    boost::uint16_t resource_count;
    Guid peer_id;
    peer_id.generate();
    std::vector<boost::uint32_t> real_ips;
    real_ips.push_back(1232423);
    vector<REPORT_RESOURCE_STRUCT> resource_ids;
    REPORT_RESOURCE_STRUCT res_id;
    res_id.ResourceID.generate();
    res_id.Type = 1;
    resource_ids.push_back(res_id);
    boost::asio::ip::address_v4 av4;
#ifdef BOOST_WINDOWS_API
    av4 = boost::asio::ip::address_v4::from_string("121.10.44.47");
    //av4 = boost::asio::ip::address_v4::from_string("115.238.140.39");
#else 
    av4 = boost::asio::ip::address_v4::from_string("127.0.0.1");
#endif
   
    boost::asio::ip::udp::endpoint end_point(boost::asio::ip::address(av4),port);
    if(0 != client.ReportPeer(keepalive_interval,detected_ip,detected_udp_port,resource_count,rand(),peer_id,1,1,5041,123124,1234,123456,342,real_ips,resource_ids,255,10,10,64,64,10,end_point,g_ios,0.3))
    {
        return false;
    }

    //(std::vector<CandidatePeerInfo>& candidate_peer_info,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
    //boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout)
    std::vector<CandidatePeerInfo> candidate_peer_info;
    if(0 != client.ListPeer(candidate_peer_info,rand(),res_id.ResourceID,peer_id,50,end_point,g_ios,0.3))
    {
        return false;
    }
    if(candidate_peer_info.size()>=1)
    {
        return true;
    }
    else
    {
        return false;
    }
}


//uint32_t GuidMod(const Guid& guid, uint32_t mod)
//{
//    boost::uint64_t buf[2];
//    memcpy(&buf, &guid.data(), sizeof(guid.data()));
//    buf[1] = framework::system::BytesOrder::little_endian_to_host_longlong(buf[1]);
//    return static_cast<uint32_t> (buf[1] % mod);
//}

bool TestATrackerAg(unsigned short port)
{
    Guid peer_id;
    peer_id.generate();
    boost::asio::ip::address_v4 av4;
#ifdef BOOST_WINDOWS_API
    av4 = boost::asio::ip::address_v4::from_string("118.123.212.12");
#else 
    av4 = boost::asio::ip::address_v4::from_string("127.0.0.1");
#endif

    //查询4个属于不同分组的rid，确保每个分组的情况都能测试到。
    boost::asio::ip::udp::endpoint end_point(boost::asio::ip::address(av4),port);

    RID rid=RID("5888E88E410875377C82FE58B9FE944C");
    //int tmp = GuidMod(rid,4);
    //发list请求，期待有返回
    std::vector<CandidatePeerInfo> candidate_peer_info;
    if(0 != client.ListPeer(candidate_peer_info,rand(),rid,peer_id,50,end_point,g_ios,0.3))
    {
        return false;
    }
    rid=RID("5888E88E410875377D82FE58B9FE944C");
    //tmp = GuidMod(rid,4);
     
    if(0 != client.ListPeer(candidate_peer_info,rand(),rid,peer_id,50,end_point,g_ios,0.3))
    {
        return false;
    }
    rid=RID("5888E88E410875377E82FE58B9FE944C");
    //tmp = GuidMod(rid,4);
    if(0 != client.ListPeer(candidate_peer_info,rand(),rid,peer_id,50,end_point,g_ios,0.3))
    {
        return false;
    }
    rid=RID("5888E88E410875377F82FE58B9FE944C");
    //tmp = GuidMod(rid,4);
    if(0 != client.ListPeer(candidate_peer_info,rand(),rid,peer_id,50,end_point,g_ios,0.3))
    {
        return false;
    }
    return true;
}

//验证本地的tracker是否都在正常工作
void LocalTrackerTest()
{
    int trackernum;
    //cin>>trackernum;
    readcin(trackernum);
    int successnum=0;
    for(unsigned short i=0;i<trackernum;++i)
    {
        if(TestATracker(i+18000))
        {
            ++successnum;
        }
    }
    cout<<successnum<<endl;
}

//验证本地的trackerag是否都在正常工作
void LocalTrackerAgTest()
{
    int trackernum;
    //cin>>trackernum;
    readcin(trackernum);
    int successnum=0;
    for(unsigned short i=0;i<trackernum;++i)
    {
        if(TestATrackerAg(i+17900))
        {
            ++successnum;
        }
    }
    cout<<successnum<<endl;
}

void readcin(int& value)
{
    if(g_argindex < g_argc)
    {
       value = atoi(g_argv[g_argindex]);
       ++g_argindex;
    }
    else
    {
        cin>>value;
    }
}

void readcin(double& value)
{
    if(g_argindex < g_argc)
    {
        value = atof(g_argv[g_argindex]);
        ++g_argindex;
    }
    else
    {
        cin>>value;
    }
}

void readcin(string& value)
{    
    if(g_argindex < g_argc)
    {
        value = g_argv[g_argindex];
        ++g_argindex;
    }
    else
    {
        cin>>value;
    }
}

#if 1
int main(int argc,char** argv)
{
    //cout<<"hello,usage:"<<endl;
    //cout<<"0(list) rid ip port pid"<<endl;

    g_argc = argc;
    g_argv = argv;

    //cout << "please input command: ";
    int cmd=-1;
    //cin>>cmd;
    readcin(cmd);
REQURY:
    switch(cmd)
    {
    case 0:
        ProcessList();
        break;
    case 1:
        ProcessQueryPeerCount();
        break;
    case 2:
        ProcessQueryResources();
        break;
    case 3:
        ProcessQueryTrackerStatistic();
        break;
    case 4:
        ProcessListTcp();
        break;
    case 5:
        ProcessListTcpWithIp();
        break;
    case 6:
        ProcessListAll();
        break;
    case 7:
        ProcessListRidsAll();
        break;
    case 8:
        ProcessListWithIp();
        break;
    case 9:
        LocalTrackerTest();
        break;
    case 10:
        LocalTrackerAgTest();
        break;
    case 11:
        ProcessInternalCommand();
        break;
    case 12:
        ProcessListRidIp();
        break;
    default:
        cout<<"error input cmd"<<endl;
        return 0;
    }

    //再一个输入，防止太快退出
    string tmp;
    //cin>>tmp;
    readcin(tmp);
    if(atoi(tmp.c_str()) != 0 || tmp == "0")
    {
        cmd = atoi(tmp.c_str());
        goto REQURY;
    }

    return 0;
}
#endif

#if 0
int main(int argc,char** argv)
{
    //cout<<"hello,usage:"<<endl;
    //cout<<"0(list) rid ip port pid"<<endl;
    
    g_argc = argc;
    g_argv = argv;

    cout << "please input command: ";
    int cmd=-1;
    //cin>>cmd;
    readcin(cmd);
REQURY:
    switch(cmd)
    {
    case 0:
        ProcessList();
        break;
    case 1:
        ProcessQueryPeerCount();
        break;
    case 2:
        ProcessQueryResources();
        break;
    case 3:
        ProcessQueryTrackerStatistic();
        break;
    case 4:
        ProcessListTcp();
        break;
    case 5:
        ProcessListTcpWithIp();
        break;
    case 6:
        ProcessListAll();
        break;
    case 7:
        ProcessListRidsAll();
        break;
    case 8:
        ProcessListWithIp();
        break;
    case 9:
        LocalTrackerTest();
        break;
    case 10:
        LocalTrackerAgTest();
        break;
    case 11:
        ProcessInternalCommand();
        break;
    default:
        cout<<"error input cmd"<<endl;
        return 0;
    }

    //再一个输入，防止太快退出
    string tmp;
    //cin>>tmp;
    readcin(tmp);
    if(atoi(tmp.c_str()) != 0 || tmp == "0")
    {
        cmd = atoi(tmp.c_str());
        goto REQURY;
    }

    return 0;
}
#endif

#if 0
int main(int argc,char* argv[])
{
    if(argc != 4)
    {
        cout<<"hello,usage:"<<endl;
        cout<<"exe rid ip port pid"<<endl;
        return 0;
    }

    string rid = string(argv[1]);    
    string ip= string(argv[2]);    
    unsigned short port = atoi(argv[3]);

    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);

    //int QueryPeerCount(boost::uint32_t& peer_count,boost::uint32_t transaction_id,const RID& resource_id,const boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);
    uint32_t peer_count;

    if (0 != client.QueryPeerCount(peer_count,boost::uint32_t(rand()),RID(rid),endpoint_,g_ios,2.0))
    {
        cout<<"process failed,msg:"<<client.GetErrMsg()<<endl;
        return -1;
    }
    cout<<"peercount:"<<peer_count<<endl;	
    return 0;
}
#endif