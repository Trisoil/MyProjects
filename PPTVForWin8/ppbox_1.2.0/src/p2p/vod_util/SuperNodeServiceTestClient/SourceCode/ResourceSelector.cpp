//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ResourceSelector.h"
#include "ResourcesListDownloader.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/bind.hpp>

namespace super_node_test
{
    void ResourceSelector::Start()
    {
        framework::configure::Config conf("SuperNodeServiceTestClient.conf");
        conf.register_module("CDN")
            << CONFIG_PARAM_NAME_RDONLY("resource_list", resource_list_)
            << CONFIG_PARAM_NAME_RDONLY("resources_size", resources_size_file_);

        if (resource_list_ == "latest")
        {
            boost::shared_ptr<ResourceListDownloader> downloader(new ResourceListDownloader(io_service_));
            downloader->Start(boost::bind(&ResourceSelector::OnResourceListDownloaded, this, _1));
        }
        else
        {
            DoStart();    
        }
    }

    void ResourceSelector::DoStart()
    {
        ParseResourcesSize();
        ParseResourcesList();
        started_ = true;
    }

    void ResourceSelector::OnResourceListDownloaded(const string& resource_list_name)
    {
        if (resource_list_name.length() > 0)
        {
            std::cout<<"Using resource list:"<<resource_list_name<<std::endl;
            resource_list_ = resource_list_name;
            DoStart();
        }
        else
        {
            std::cout<<"Failed to get resource list"<<std::endl;
            exit(1);
        }
    }

    void ResourceSelector::ParseResourcesSize()
    {
        std::ifstream stream(resources_size_file_.c_str());
        resources_size_.clear();

        const int MaxLineSize = 1024;
        char line_text[MaxLineSize];

        int line_count(0);
        while (stream)
        {
            if (!stream.getline(line_text, MaxLineSize))
            {
                break;
            }

            if (line_count++ == 0)
            {
                //to skip the header line
                continue;
            }

            std::string d;

            std::string resource_size_text(line_text);
            int resource_name_suffix_pos = resource_size_text.rfind(".mp4");
            if (resource_name_suffix_pos <= 0 || resource_name_suffix_pos >= resource_size_text.size() - 4)
            {
                std::cout<<"bad line encountered:"<<resource_size_text<<std::endl;
                continue;
            }
            
            size_t file_size(0);
            std::string resource_name = resource_size_text.substr(0, resource_name_suffix_pos + 4);
            std::istringstream input_stream(resource_size_text.substr(resource_name_suffix_pos + 4));
            input_stream >> file_size;

            if (file_size > 0)
            {
                resources_size_.insert(std::make_pair(resource_name, file_size));
            }
        }
    }

    void ResourceSelector::ParseResourcesList()
    {
        std::ifstream stream(resource_list_.c_str());

        resources_on_server_.clear();
        const int MaxLineSize = 1024;
        char line_text[MaxLineSize];

		hit_sum_ = 0;

        while (stream)
        {
            if (!stream.getline(line_text, MaxLineSize))
            {
                break;
            }

            string resource_info_text(line_text);

            std::vector<string> resource_info;
            boost::algorithm::split(resource_info, resource_info_text, boost::algorithm::is_any_of("|"));

            if (resource_info.size() >= 3)
            {
                string resource_name = resource_info[0];

                size_t hit_count(0);
                std::stringstream stream(resource_info[resource_info.size() - 1]);
                if (stream>>hit_count)
                {
                    hit_sum_ += hit_count ?  hit_count : 1;

                    for(size_t i = 1; i < resource_info.size() - 2; ++i)
                    {
                        resource_name += resource_info[i];
                    }

                    assert(resource_name.rfind(".mp4") == resource_name.size() - 4);

                    resources_on_server_.insert(std::make_pair(hit_sum_, resource_name));
                }
                else
                {
                    assert(false);
                }
            }
            else
            {
                assert(false);
            }
        }
    }

    Resource ResourceSelector::SelectResource()
    {
        if (hit_sum_ > 0)
        {
            size_t number = ((rand() << 16) + rand()) % hit_sum_;

			std::map<size_t, string>::iterator iter = resources_on_server_.upper_bound(number);

			assert(iter != resources_on_server_.end());

			string resource_name = iter->second;
			std::map<string, size_t>::const_iterator resource_size_iter = resources_size_.find(resource_name);
			size_t resource_size = 1024*1024;
			if (resource_size_iter != resources_size_.end())
			{
				resource_size = resource_size_iter->second;
			}
			else
			{
				std::cout<<"Size for "<<resource_name<<" is not available"<<std::endl;
			}

			return Resource(resource_name, resource_size);
        }

        return Resource("", 0);
    }
}
