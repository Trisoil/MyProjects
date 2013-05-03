#include "tools/recorder/Common.h"
#include "tools/recorder/Define.h"
#include "tools/recorder/ChannelListManage.h"


#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <util/protocol/http/HttpClient.h>

#include <util/serialization/stl/vector.h>
#include <util/archive/ArchiveBuffer.h> 
#include <util/archive/XmlIArchive.h>

#include <framework/string/Parse.h>

#include "tools/recorder/Channel.h"

using namespace boost::system;
using namespace framework::logger;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ChannelListManage", 0)

BEGIN_NAME_SPACE

ChannelListManage::ChannelListManage(util::daemon::Daemon & daemon)
    : util::daemon::ModuleBase<ChannelListManage>(daemon, "ChannelListManage"), timer_(io_svc())
{

    std::string service_ip("0.0.0.0"), service_port("80"), sn_type("3");
    std::string strSize("5"),strTimeLong("10000");
   

    snc_url_ = "http://snc2.synacast.com/sn/channel_list.aspx";
    
    daemon.config().register_module("ChannelListManage")
        << CONFIG_PARAM_NAME_NOACC("service_ip", service_ip);

    daemon.config().register_module("ChannelListManage")
        << CONFIG_PARAM_NAME_NOACC("service_port", service_port);

    daemon.config().register_module("ChannelListManage")
        << CONFIG_PARAM_NAME_NOACC("m3ufilesize", strSize);

    daemon.config().register_module("ChannelListManage")
        << CONFIG_PARAM_NAME_NOACC("rootdir", config_.root_dir_);

    daemon.config().register_module("ChannelListManage")
        << CONFIG_PARAM_NAME_NOACC("core_addr", config_.core_addr_);

    daemon.config().register_module("ChannelListManage")
        << CONFIG_PARAM_NAME_NOACC("tstimelong", strTimeLong);
    daemon.config().register_module("ChannelListManage")
        << CONFIG_PARAM_NAME_NOACC("snc_url", snc_url_);


    LOG_S(Logger::kLevelDebug, "[service_ip] " << service_ip);
    LOG_S(Logger::kLevelDebug, "[service_port] " << service_port);
    LOG_S(Logger::kLevelDebug, "[m3ufilesize] " << strSize);
    LOG_S(Logger::kLevelDebug, "[rootdir] " << config_.root_dir_);
    LOG_S(Logger::kLevelDebug, "[tstimelong] " << strTimeLong);
    LOG_S(Logger::kLevelDebug, "[snc_url] " << snc_url_);
    LOG_S(Logger::kLevelDebug, "[core_addr] " << config_.core_addr_);

    framework::string::parse2(strSize,config_.m3u_size_);
    framework::string::parse2(strTimeLong,config_.file_t_long_);

    snc_url_ += "?ip=" + service_ip;
    snc_url_ += ":" + service_port;
    snc_url_ += "&type=" + sn_type;

    //snc_url_ = "http://192.168.1.100/ipad/a.xml";

    LOG_S(Logger::kLevelDebug, "[snc_url_] " << snc_url_.c_str());
}
ChannelListManage::~ChannelListManage()
{

}

boost::system::error_code ChannelListManage::startup()
{   
    timer_.expires_from_now(boost::posix_time::seconds(1));
    timer_.async_wait(boost::bind(&ChannelListManage::handle_timer, this, _1));

    return boost::system::error_code();    
}
void ChannelListManage::shutdown()
{
    std::map<std::string,Channel*>::iterator iter = ChannelList_.begin();
    for (; iter != ChannelList_.end(); ++iter )
    {
        iter->second->Exit();
    }
    ChannelList_.clear();

    //timer_->close()
    timer_.cancel();
   // boost::thread::sleep(boost::get_system_time()+boost::posix_time::milliseconds(1000));
}

void ChannelListManage::handle_timer( boost::system::error_code const & ec )
{
    if (!get_daemon().is_started()) {
        return;
    }

    std::string url(snc_url_);
    if (failed_channels_ != "")
    {
        url += "&failed=" + failed_channels_;
        failed_channels_.clear();
    }
        
    boost::shared_ptr<HttpClient> snc_client_ptr(new HttpClient(io_svc()));
    snc_client_ptr->async_fetch_get(url, boost::bind(&ChannelListManage::on_fetch_channels, this, _1, snc_client_ptr));

    timer_.expires_from_now(boost::posix_time::seconds(60));
    timer_.async_wait(boost::bind(&ChannelListManage::handle_timer, this, _1));
}


void ChannelListManage::on_fetch_channels( boost::system::error_code const & ec, boost::shared_ptr<HttpClient> snc_client_ptr )
{

     boost::system::error_code ec1;

     Channel* pChannel = NULL;
 
    if(!ec)
    {
        util::archive::XmlIArchive<> ia(snc_client_ptr->response().data());
        
        std::vector<channel> channels;
        ia>> SERIALIZATION_NVP(channels);
        
        //添加新的，去除旧的
  
        //创建一个 ChannelList_ 副本,用于去除与vector公共部分剩下的，就是需要删除的频道
        std::map<std::string,Channel*> tempCList;
        tempCList.insert(ChannelList_.begin(),ChannelList_.end());
        

        for (std::vector<channel>::iterator iter = channels.begin(); iter != channels.end(); ++iter)
        {
            if ( ChannelList_.find(iter->id) != ChannelList_.end() )
            {
                tempCList.erase(iter->id); //去掉公共部分
                continue;
            }
            //新增频道
            pChannel = new Channel(io_svc(),*iter,config_);
            assert(NULL != pChannel);
            ChannelList_[iter->id] = pChannel;
        }
        

        for(std::map<std::string,Channel*>::iterator iterTemp = tempCList.begin(); iterTemp != tempCList.end(); iterTemp++)
        {
            iterTemp->second->Exit(); //将去除的频道线程退出
            ChannelList_.erase(iterTemp->first);//去掉被删除的频道
        }

    }
    else
    {
        LOG_S(Logger::kLevelEvent, "ChannelListManage::on_fetch_channels failed ");
    }
    
    snc_client_ptr->close(ec1);
}
END_NAME_SPACE

