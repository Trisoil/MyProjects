#include "StdAfx.h"
#include "MergeManager.h"
#include "udp_index_cleaner/MergeThread.h"
#include <io.h>

namespace udplog
{
    MergeManager::p MergeManager::inst_ = MergeManager::p(new MergeManager());

    MergeManager::MergeManager(void)
    :is_running_(false)
    {
    }

    MergeManager::~MergeManager(void)
    {
    }

    void MergeManager::CheckLog()
    {
        tstring moduleName = _T("index_cleaner"); 
        framework::mswin::Module module;
        framework::io::Path config_path;
        module.BuildLocalFilePath(config_path, moduleName.c_str());
        filename = config_path.GetString()+_T("_log");
        filename_tmp = filename + _T("_tmp");

        file_record_name_ = config_path.GetString()+_T("_record");
        file_record_temp_name_ = file_record_name_+_T("_tmp");

        if (!access(w2b(filename_tmp).c_str(),0))
        {
            MemRecord::p mem_record = MemRecord::Create();

            framework::io::StdFile file_tmp_1;
            file_tmp_1.Open(filename_tmp.c_str(), _T("r"));
            if (file_tmp_1.IsOpen())
            {
                const size_t bsize = 1024;
                char buff[bsize];
                while (file_tmp_1.ReadLine(buff,bsize))
                {
                    string line_tmp = buff;
                    size_t ps = line_tmp.find(" ");
                    if (ps != string::npos)
                    {
                        string key = line_tmp.substr(0,ps);
                        time_t vt = atoi(boost::trim_copy(line_tmp.substr(ps)).c_str()); 

                        if (vt != 0)
                        {
                            mem_record->record_[key] = vt;
                        }
                    }
                }
            }
            file_tmp_1.Close();

            remove(w2b(filename_tmp).c_str());
            MergeThread::IOS().post(
                boost::bind(&MergeManager::OnWrite, MergeManager::Inst(), mem_record, 24)
                );

        }
        MergeThread::IOS().post(
            boost::bind(&MergeManager::OnMerge, MergeManager::Inst())
            );
    }

    void MergeManager::Start()
    {
        if (is_running_) return;

        is_running_ = true;

        CheckLog();
    }

    void MergeManager::Stop()
    {
        if (!is_running_)return;
    }

    void MergeManager::OnWrite(MemRecord::p mem_record, int aclock)
    {
        if (!is_running_)return;
        tstring fname = GetMergeName(file_record_name_, aclock);

        {
            framework::io::StdFile outfile;
            outfile.Open(fname.c_str(), _T("w"));
            for (MemRecord::rcd::iterator it = mem_record->record_.begin(); 
                it != mem_record->record_.end(); ++it)
            {
                stringstream ss;
                ss << it->first << " " << it->second << endl;
                string str_tmp = ss.str();
                outfile.Write(str_tmp.c_str(), str_tmp.length());            
            }
            outfile.Close();
        }
        remove(w2b(filename_tmp).c_str());
    }

    void MergeManager::OnMerge()
    {
        if (!is_running_)return;

        string ttip = Config::Inst().GetValue("cleaner.ttip");
        short ttport = Config::Inst().GetInteger("cleaner.ttport");
        if (ttip.empty() || ttport == 0)
        {
            CONSOLE_OUTPUT("Failed to Get the IP and Port of Tokyo Tyrant");
            return;
        }
        {
            TokyoTyrantClient::p ttclient_ = TokyoTyrantClient::Create(ttip, ttport);

            if (ttclient_->IsGoodConnection() == false)
            {
                CONSOLE_OUTPUT(ttclient_->GetLastErr());
            }

            framework::io::StdFile file_record_tmp;
            file_record_tmp.Open(file_record_temp_name_.c_str(), _T("w"));

            multimap<string, MergeFileNode::p> mergemap;

            MergeFileNode::p fn0 = MergeFileNode::Create(file_record_name_);
            string str_0 = fn0->getline();
            if (str_0.empty() == false)
            {
                mergemap.insert(make_pair(str_0, fn0));
            }

            for (int i = 0; i < 25; i++)
            {
                MergeFileNode::p fni = MergeFileNode::Create(GetMergeName(file_record_name_, i));
                string str_i = fni->getline();
                if (str_i.empty() == false)
                {
                    mergemap.insert(make_pair(str_i, fni));
                }
            }

            while(mergemap.empty() == false)
            {
                string line = mergemap.begin()->first;
                MergeFileNode::p fnt = mergemap.begin()->second;
                mergemap.erase(mergemap.begin());

                string linet = fnt->getline();
                if (linet.empty() == false)
                {
                    mergemap.insert(make_pair(linet, fnt));
                }
                if (mergemap.empty())
                {
                    size_t ps0 = line.find(" ");
                    if (ps0 != string::npos)
                    {
                        string key0 = line.substr(0, ps0);
                        size_t val0 = atoi(boost::trim_copy(line.substr(ps0)).c_str()); 
                        if (val0 < time(NULL) - Config::Inst().GetInteger("cleaner.keep_days",30)*24*3600)
                        {
                            if (ttclient_->IsGoodConnection() == false)
                            {
                                ttclient_ = TokyoTyrantClient::Create(ttip, ttport);
                            }
                            if(ttclient_->Del(key0)==false)
                            {
                                CONSOLE_OUTPUT(ttclient_->GetLastErr());
                            }
                        }
                        else
                        {
                            file_record_tmp.Write(line.c_str(), line.length());
                        }
                    }
                    break;
                }
                string key0, key;
                size_t ps0 = line.find(" ");
                size_t ps = mergemap.begin()->first.find(" ");
                if (ps0 != string::npos)
                {
                    key0 = line.substr(0, ps0);
                    if (ps != string::npos)
                    {
                        key = mergemap.begin()->first.substr(0, ps);
                        if (key0 == key)
                        {
                            continue;
                        }
                    }
                    size_t val0 = atoi(boost::trim_copy(line.substr(ps0)).c_str()); 
                    if (val0 < time(NULL) - Config::Inst().GetInteger("cleaner.keep_days",30)*24*3600)
                    {
                        if (ttclient_->IsGoodConnection() == false)
                        {
                            ttclient_ = TokyoTyrantClient::Create(ttip, ttport);
                        }
                        if(ttclient_->Del(key0)==false)
                        {
                            CONSOLE_OUTPUT(ttclient_->GetLastErr());
                        }
                    }
                    else
                    {
                        file_record_tmp.Write(line.c_str(), line.length());
                    }
                }
            }

            file_record_tmp.Close();
        }

        for (int i = 0; i < 25; i++)
        {
            remove(w2b(GetMergeName(file_record_name_, i)).c_str());
        }

        remove(w2b(file_record_name_).c_str());
        remove(w2b(filename_tmp).c_str());
        rename(w2b(file_record_temp_name_).c_str(), w2b(file_record_name_).c_str());
    }
}