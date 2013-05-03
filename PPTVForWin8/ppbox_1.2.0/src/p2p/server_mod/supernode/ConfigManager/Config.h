//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_CONFIG_H
#define SUPER_NODE_CONFIG_H

namespace super_node
{
    class Config
    {
    public:
        // 如果有相同项，将以config_file1为准
        static bool MergeConfig(const std::string & config_file1, const std::string & config_file2,
            std::map<std::string, std::map<std::string, std::string> > & merged_config);
        static void WriteConfig(const std::string & config_file, const std::map<std::string, std::map<std::string, std::string> > & config);
        static bool ReadConfig(const std::string & config_file, std::map<std::string, std::map<std::string, std::string> >& config);

    private:
        static void MergeConfig(const std::map<std::string, std::map<std::string, std::string> > & config1,
            const std::map<std::string, std::map<std::string, std::string> > & config2,
            std::map<std::string, std::map<std::string, std::string> > & merged_config);
        static bool GetKey(const std::string & line_text, std::string & key);
        static std::string& Trim(std::string& s);
    };
}

#endif  // SUPER_NODE_CONFIG_H