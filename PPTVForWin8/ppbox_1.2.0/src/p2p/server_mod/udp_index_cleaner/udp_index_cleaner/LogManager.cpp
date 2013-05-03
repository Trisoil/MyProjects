#include "StdAfx.h"
#include "LogManager.h"

namespace udplog
{
    LogManager::p LogManager::inst_ = LogManager::p(new LogManager());

    LogManager::LogManager(void)
        :is_running_(false)
    {
        
    }

    LogManager::~LogManager(void)
    {
        if (is_running_)
        {
            Stop();
        }
    }


    void LogManager::Start()
    {
        if (is_running_) return;

        tstring moduleName = _T("index_cleaner_log"); 
        framework::mswin::Module module;
        framework::io::Path config_path;
        module.BuildLocalFilePath(config_path, moduleName.c_str());
        filename = config_path.GetString();
        filename_tmp = filename + _T("_tmp");


        file_.Open(filename.c_str(), _T("w"));
        if (file_.IsOpen() && !file_.IsFailed())
        {
            is_running_ = true;
        }
    }
    void LogManager::Stop()
    {
        if (!is_running_) return;

        file_.Close();
        is_running_ = false;
    }
    void LogManager::ChangeFile()
    {
        if (!is_running_) return;

        file_.Close();
        remove(w2b(filename_tmp).c_str());
        rename(w2b(filename).c_str(), w2b(filename_tmp).c_str());
        file_.Open(filename.c_str(), _T("w"));
    }

    void LogManager::OnVisit(string key, time_t vt)
    {
        if (!is_running_) return;

        stringstream ss;
        ss << key << " " << vt << endl;
        string line_tmp = ss.str();
        file_.Write(line_tmp.c_str(), line_tmp.length());
        //file_.Flush();
    }
}