#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include "udp_index_cleaner/UdpCleanerModule.h"
#include "framework/io/stdfile.h"
#include "udp_index_cleaner/TokyoTyrantClient.h"
using namespace TokyoTyrant;

namespace udplog
{
    struct MergeFileNode
    {
        typedef boost::shared_ptr<MergeFileNode> p;
        typedef boost::shared_ptr<framework::io::StdFile> file_ptr;
        tstring filename_;
        file_ptr file_;
        bool is_ok_;

        static p Create(tstring filename)
        {
            return p(new MergeFileNode(filename));
        }

        string getline()
        {
            if (is_ok_ == false)
            {
                return "";
            }
            const size_t bsize = 1024;
            char buff[bsize];
            if(file_->ReadLine(buff, bsize) == false)
            {
                return "";
            }
            return buff;
        }

        ~MergeFileNode()
        {
            file_->Close();
        }

    private:
        MergeFileNode(tstring filename): filename_(filename), is_ok_(false)
        {
            file_ = file_ptr(new framework::io::StdFile);
            file_->Open(filename_.c_str(), _T("r"));
            is_ok_ = file_->IsOpen();
        }
    };


    class MergeManager
        :public boost::enable_shared_from_this<MergeManager>
        , public boost::noncopyable
    {
    public:
        typedef boost::shared_ptr<MergeManager> p;

        static p Inst(){return inst_;};

        void OnMerge();
        void OnWrite(MemRecord::p mem_record, int aclock);

        void Start();
        void Stop();

        void CheckLog();

    private:
        inline tstring GetMergeName(const tstring &file_record_name, int aclock)
        {
            if (aclock < 0 || aclock > 24) return _T("");

            tstring aclocks[] = {_T("_00"),_T("_01"),_T("_02"),_T("_03"),_T("_04"),_T("_05"),_T("_06")
                ,_T("_07"),_T("_08"),_T("_09"),_T("_10"),_T("_11"),_T("_12"),_T("_13"),_T("_14")
                ,_T("_15"),_T("_16"),_T("_17"),_T("_18"),_T("_19"),_T("_20"),_T("_21"),_T("_22")
                ,_T("_23"),_T("_24")};

            return file_record_name + aclocks[aclock];
        }

    public:
        ~MergeManager(void);

    private:
        MergeManager(void);

    private:
        static p inst_;

        bool is_running_;

        tstring filename;
        tstring filename_tmp;

        tstring file_record_name_;
        tstring file_record_temp_name_;
    };
}