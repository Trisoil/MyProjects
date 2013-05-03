//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "Config.h"

namespace super_node
{
    bool Config::MergeConfig(const std::string & config_file1, const std::string & config_file2,
        std::map<std::string, std::map<std::string, std::string> > & merged_config)
    {
        std::map<std::string, std::map<std::string, std::string> > config1, config2;
        if (ReadConfig(config_file1, config1) && ReadConfig(config_file2, config2))
        {
            MergeConfig(config1, config2, merged_config);
            return true;
        }

        return false;
    }

    bool Config::ReadConfig(const std::string & config_file, std::map<std::string, std::map<std::string, std::string> > & config)
    {
        std::ifstream config_content(config_file.c_str());
        std::string line_text;
        std::string section_name;

        if (!config_content)
        {
            LOG4CPLUS_ERROR(Loggers::ConfigManager(), config_file << " does not exist.");
            return false;
        }

        while(!config_content.eof())
        {
            std::getline(config_content, line_text);

            line_text = Trim(line_text);

            if (line_text.length() == 0 || line_text.find('#') == 0)
            {
                continue;
            }

            if (line_text.find('[') == 0 && line_text.find(']') == line_text.length() - 1)
            {
                section_name = line_text.substr(1, line_text.length() - 2);
            }
            else
            {
                std::istringstream iss(line_text, std::istringstream::in);
                std::string key;

                if (GetKey(line_text, key))
                {
                    std::map<std::string, std::map<std::string, std::string> >::iterator iter = config.find(section_name);
                    if (iter == config.end())
                    {
                        std::map<std::string, std::string> key_content;
                        key_content.insert(std::make_pair(key, line_text));
                        config.insert(std::make_pair(section_name, key_content));
                    }
                    else
                    {
                        assert(iter->second.find(key) == iter->second.end());
                        iter->second.insert(std::make_pair(key, line_text));
                    }
                }
            }
        }
        return true;
    }

    void Config::WriteConfig(const std::string & config_file, const std::map<std::string, std::map<std::string, std::string> > & config)
    {
        std::ofstream config_content(config_file.c_str());

        for (std::map<std::string, std::map<std::string, std::string> >::const_iterator outter_iter = config.begin();
            outter_iter != config.end(); ++outter_iter)
        {
            config_content << '[' << outter_iter->first << ']' << std::endl;
            for (std::map<std::string, std::string>::const_iterator inner_iter = outter_iter->second.begin();
                inner_iter != outter_iter->second.end(); ++inner_iter)
            {
                config_content << inner_iter->second << std::endl;
            }
            
            config_content<<std::endl;
        }
    }

    void Config::MergeConfig(const std::map<std::string, std::map<std::string, std::string> > & config1,
        const std::map<std::string, std::map<std::string, std::string> > & config2,
        std::map<std::string, std::map<std::string, std::string> > & merged_config)
    {
        merged_config = config2;
        for (std::map<std::string, std::map<std::string, std::string> >::const_iterator outter_iter = config1.begin();
            outter_iter != config1.end(); ++outter_iter)
        {
            std::map<std::string, std::map<std::string, std::string> >::iterator merged_iter = merged_config.find(outter_iter->first);
            if (merged_iter == merged_config.end())
            {
                merged_config.insert(std::make_pair(outter_iter->first, outter_iter->second));
            }
            else
            {
                for (std::map<std::string, std::string>::const_iterator inner_iter = outter_iter->second.begin();
                    inner_iter != outter_iter->second.end(); ++inner_iter)
                {
                    merged_iter->second[inner_iter->first] = inner_iter->second;
                }
            }
        }
    }

    bool Config::GetKey(const std::string & line_text, std::string & key)
    {
        int equal_mark_index = line_text.find('=');
        if (equal_mark_index > 0)
        {
            std::string key_include_space = line_text.substr(0, equal_mark_index);
            key = Trim(key_include_space);
            return true;
        }

        LOG4CPLUS_ERROR(Loggers::ConfigManager(), "The following config line is illegal :" << line_text << std::endl);
        return false;
    }

    std::string& Config::Trim(std::string& s)
    {
        std::string::iterator p = find_if(s.begin(), s.end(), std::not1(std::ptr_fun(isspace)));
        s.erase(s.begin(), p);

        std::string::reverse_iterator r_p = find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun(isspace)));
        s.erase(r_p.base(), s.end());

        return s;
    }
}