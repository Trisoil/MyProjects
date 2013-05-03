#include "tools/recorder/Common.h"
#include "tools/recorder/Define.h"

#include "tools/recorder/Channel.h"

//#include "tools/recorder/ChannelListManage.h"

#include <boost/filesystem.hpp>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <ppbox/mux/Common.h>
#include <ppbox/mux/TsSegmentCreater.h>

#include <ppbox/demux/Common.h>
#include <ppbox/demux/LiveDemuxer.h>
using namespace ppbox::demux;
using namespace framework::logger;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("Channel", 0)
//using namespace server_mod::live::live_recorder;
BEGIN_NAME_SPACE

Channel::Channel(boost::asio::io_service& ioser,channel& ch,Conf_Param& conf_):
io_serv(ioser),
m_channel(ch),
m_state(cs_running),
m_fileMan(conf_.root_dir_,ch.id,conf_.m3u_size_),
m_iTsTimers(conf_.file_t_long_),
core_addr_(conf_.core_addr_),
work_(ioser)

{
    th_ = new boost::thread( boost::bind( &Channel::StartChannel, this) );
    assert(NULL != th_);
}

Channel::~Channel()
{
    if (NULL != th_)
    {
        delete th_;
        th_ = NULL;
    }
}

void Channel::StartChannel()
{

    std::string strUrl("pplive|"); 
    const std::string key = "synacast://";
    if (0 == m_channel.url.find(key))
    {
        strUrl += m_channel.url.substr(key.size(),m_channel.url.size() - key.size());
    }
    else
    {
        strUrl = m_channel.url;
    }

    LOG_S(Logger::kLevelDebug, "[strUrl] " << strUrl.c_str());

    //pplive://  --->  pplive|

    while (cs_threadexit != m_state)
    {
        //初始化 Mux getsample 
        m_fileMan.Destory();

        LiveDemuxer demuxer(
            io_serv,
            9001,
            10*1024*1024,
            10*1024);

        boost::system::error_code ec;

        demuxer.set_http_proxy(framework::network::NetName(core_addr_),ec);

        demuxer.open(strUrl, ec);
        if(ec)
        {
            LOG_S(Logger::kLevelDebug, "[LiveDemuxer open channel Failed] "<<m_channel.id.c_str());
            LOG_S(Logger::kLevelDebug, "code : "<<ec.value()<<" msg"<<ec.message().c_str());
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
            continue;
        }

        ppbox::mux::TsSegmentCreater ts_seg_creater(m_iTsTimers);
        ec.clear();
        ts_seg_creater.open(&demuxer, ec);
        if(ec)
        {
            demuxer.close(ec);
            LOG_S(Logger::kLevelDebug, "[TsSegmentCreater open channel Failed] "<<m_channel.id.c_str());
			LOG_S(Logger::kLevelDebug, "code : "<<ec.value()<<" msg"<<ec.message().c_str());
            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
            continue;
        }

        ppbox::mux::MuxTagEx tag;

        while (cs_threadexit != m_state)
        {
            //获取直播流写文件
            ts_seg_creater.read(tag, ec);
            //LOG_S(Logger::kLevelDebug, "[tag.time] " << tag.time);
            //LOG_S(Logger::kLevelDebug, "[tag.itrack] " << tag.itrack);
            //LOG_S(Logger::kLevelDebug, "[tag.is_sync] " << tag.is_sync);
            if (!ec) 
            {
                //std::cout << "**********************" << std::endl;
                m_fileMan.write((const char *)tag.tag_header_buffer, tag.tag_header_length);
                m_fileMan.write((const char *)tag.tag_data_buffer, tag.tag_data_length);
            } 
            else 
            {
                if (ec == boost::asio::error::would_block) 
                {
                    std::cout << "ec = would_block" << std::endl;
                    LOG_S(Logger::kLevelDebug, "[TsSegmentCreater open channel Failed] "<<m_channel.id.c_str());
                    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
                    continue;
                } 
                else if (ec == ppbox::mux::error::mux_segment_end) 
                {
                    std::cout << "ec = mux_segment_end" << std::endl;
                    //LOG_S(Logger::kLevelDebug, "[ec = mux_segment_end] "<<m_channel.id.c_str());
                    m_fileMan.changeFile();    
                } 
                else if (ec == ppbox::mux::error::mux_invalid_sample) 
                {
                    LOG_S(Logger::kLevelDebug, "[ec = mux_invalid_sample] "<<m_channel.id.c_str());
                    continue;
                } 
                else if (ec == ppbox::demux::error::no_more_sample) 
                {
                    std::cout << "ec = no_more_sample" << std::endl;
                    LOG_S(Logger::kLevelDebug, "[ec = no_more_sample] "<<m_channel.id.c_str());
                    break;
                }
                else
                {
                    LOG_S(Logger::kLevelDebug, "[ec = unkown error] "<<m_channel.id.c_str());
                    std::cout << "ec = unkown error" << std::endl;
                    break;   
                }
            }  

        }
        ts_seg_creater.close();
        demuxer.close(ec);
    }
    LOG_S(Logger::kLevelDebug, "[thread exit] "<<m_channel.id.c_str());
    SetThreadState(cs_exit);   
}

void Channel::handle_exit()
{
    this->close();
}

void Channel::Exit()
{

    SetThreadState(cs_threadexit);
}

void Channel::SetThreadState(Channel_State state)
{
    m_state = state;   
    if (cs_exit == m_state)
    {
        io_serv.post(boost::bind(&Channel::handle_exit, this));
    }
}

void Channel::close()
{
    delete this; 
}


std::string Channel::GetChannelId()
{
    return m_channel.id;
}

Channel_State Channel::GetChannelState()
{
    return m_state;
}

bool operator==(const Channel& str1,std::string psz2)
{
    return (str1.m_channel.id == psz2);
}


END_NAME_SPACE
