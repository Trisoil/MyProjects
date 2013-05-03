//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ResourceSelector.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/bind.hpp>

namespace live_media_test
{
    void ResourceSelector::Start()
    {
        std::string channels_list,channels_visit_frequency_list;

        framework::configure::Config conf("LiveMediaServiceTestClient.conf");
        conf.register_module("LiveChannel")
            << CONFIG_PARAM_NAME_RDONLY("channels", channels_list)
            << CONFIG_PARAM_NAME_RDONLY("channels_visit_frequency", channels_visit_frequency_list);

        boost::algorithm::split(channels_, channels_list, boost::algorithm::is_any_of(","));
        std::vector<std::string> channels_visit_frequency_values;
        boost::algorithm::split(channels_visit_frequency_values, channels_visit_frequency_list, boost::algorithm::is_any_of(","));

        if (channels_visit_frequency_values.size() > channels_.size())
        {
            std::cout<<"bad channel/channel_visit_frequency value"<<std::endl;
            exit(-1);
        }

        size_t sum = 0;
        for(size_t i = 0; i < channels_visit_frequency_values.size(); ++i)
        {

            size_t frequency = boost::lexical_cast<size_t>(channels_visit_frequency_values[i]);
            channels_visit_frequency_.push_back(frequency);
            sum += frequency;
        }

        if (sum > 100)
        {
            std::cout<<"bad channel_visit_frequency value"<<std::endl;
            exit(-1);
        }

        if (channels_visit_frequency_values.size() < channels_.size())
        {
            size_t average_frequency_for_non_specified = (100-sum)/(channels_.size() - channels_visit_frequency_values.size());
            while(channels_visit_frequency_.size() < channels_.size())
            {
                channels_visit_frequency_.push_back(average_frequency_for_non_specified);
            }
        }

        DoStart();    
    }

    void ResourceSelector::DoStart()
    {
        started_ = true;
    }

    Resource ResourceSelector::SelectResource()
    {
        size_t number = rand()%100;

        for(size_t i = 0; i < channels_.size(); ++i)
        {
            if (number < channels_visit_frequency_[i])
            {
                return Resource(channels_[i]);
            }

            number -= channels_visit_frequency_[i];
        }

        return Resource(channels_[0]);
    }
}