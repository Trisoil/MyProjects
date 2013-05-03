#include <framework/process/Process.h>
#include <boost/thread/thread.hpp>
#include <framework/memory/MemoryPage.h>
#include <framework/timer/TickCounter.h>

#include <iostream>
#include <vector>
#include <fstream>


std::ofstream os;

struct  ProcessInfo
{
    ProcessInfo(const char* name)
        :pid(0)
        ,name(name){}
    int pid;
    std::string name;
};

std::vector<ProcessInfo> g_list;

void param_error()
{
    std::cout<<"No Process Name ,example ./exe a b  'a' and 'b'are procee name"<<std::endl;
}



void Get_Pid(ProcessInfo& pid)
{
    std::vector<framework::process::ProcessInfo> vec;
    boost::system::error_code ec;
    pid.pid = 0;

    ec = framework::process::enum_process(pid.name,vec);
    if (!ec)
    {
        if (vec.size() > 0)
        {
            pid.pid =  vec[0].pid;
        }
    }
}

void Print(ProcessInfo& pid)
{
    if (pid.pid < 1)
    {
        return;
    }

    boost::system::error_code ec;
    framework::process::ProcessStat stat;

    ec = framework::process::get_process_stat(
        pid.pid, 
        stat);

    if (ec)
    {
        std::cout<<"get_process_stat failed pid:"<<pid.pid<<std::endl;
        pid.pid = 0;
    }
    else
    {

        static size_t pagesize = 0;
        if (pagesize < 1)
        {
            pagesize = framework::memory::MemoryPage::page_size()/1024;
            os<<"Page Size(k):"<<pagesize<<std::endl;
            std::cout<<"Page Size(k):"<<pagesize<<std::endl;
        }
        os<<"Name:"<<pid.name
            <<" RssMem(k):"<<stat.rss*pagesize
            <<" VMem(k):"<<stat.vsize/1024<<std::endl;

        std::cout<<"Name:"<<pid.name
            <<" RssMem(k):"<<stat.rss*pagesize
            <<" VMem(k):"<<stat.vsize/1024<<std::endl;
    }

}

void handle_time()
{
    for (int ii = 0; ii < g_list.size(); ++ii)
    {
        Get_Pid(g_list[ii]);
        Print(g_list[ii]);
    }
}



int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        param_error();
        return 0;
    }

    boost::uint64_t ztime = framework::timer::TickCounter::tick_count();

    char szFile[256] = {0};
    sprintf(szFile,"%u.log",ztime);
    os.open(szFile);

    for (int ii = 1; ii < argc; ++ii )
    {
        os<<"Add Process:"<<argv[ii]<<std::endl;
        g_list.push_back(ProcessInfo(argv[ii]));
    }


    while (true)
    {
        handle_time();
        boost::this_thread::sleep(boost::posix_time::milliseconds(10000));
    }


	return 0;
}
