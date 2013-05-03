#include "StdAfx.h"
#include "BlackList.h"
#include <fstream>

namespace udpindex
{
    BlackList::p BlackList::inst_ = BlackList::p(new BlackList());

    bool BlackList::Start(string file_name)
    {
        is_running_ = true;
#ifdef UNICODE
        tstring moduleName = b2w(file_name); 
#else
        tstring moduleName = file_name; 
#endif
        framework::mswin::Module module;
        framework::io::Path config_path;
        module.BuildLocalFilePath(config_path, moduleName.c_str());
#ifdef UNICODE
        file_path_ = w2b(config_path.GetString());
#else
        file_path_ = config_path.GetString();
#endif
        return Refresh();
    }

    bool BlackList::Stop()
    {
        is_running_ = false;
        is_effect_ = false;
        black_list_.clear();
        return true;
    }

    bool BlackList::IsAllow(string url)
    {
        if (is_running_ == false) return true;
        for(set<string>::iterator it = black_list_.begin(); it != black_list_.end(); ++it)
        {
            if (url.find(*it)!=string.npos)
            {
                return false;
            }
        }
        return true;
    }

    bool BlackList::Refresh()
    {
        if (file_path_.empty()) return false;
        ifstream inf(file_path_.c_str());
        char buff[1024];
        if (!inf || !inf.getline(buff, 1024)) return false;
        size_t line_count = 0;
        while(inf.getline(buff, 1024))
        {
            string line = buff;
            TrimLine(line);
            if (line.empty()) continue;
            if (!line_count)
            {
                is_effect_ = atoi(line.c_str());
            }
            else
            {                                                                                                                  
                black_list_.insert(line);
            }
            line_count++;
        }
        return true;
    }
}
