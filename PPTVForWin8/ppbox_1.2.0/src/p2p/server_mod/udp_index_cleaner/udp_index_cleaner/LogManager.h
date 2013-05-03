#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include "udp_index_cleaner/UdpCleanerModule.h"
#include "framework/io/stdfile.h"

namespace udplog
{
    class LogManager:
        public boost::enable_shared_from_this<LogManager>
        , public boost::noncopyable
    {

    public:
        typedef boost::shared_ptr<LogManager> p;

        static p Inst(){return inst_;};

        void OnVisit(string key, time_t vt);

        void Start();
        void Stop();
        void ChangeFile();

    public:
        ~LogManager(void);

    private:
        LogManager(void);

    private:
        static p inst_;

        framework::io::StdFile file_;

        bool is_running_;

        tstring filename;
        tstring filename_tmp;
    };


}