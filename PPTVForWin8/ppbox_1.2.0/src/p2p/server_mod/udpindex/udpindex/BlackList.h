#pragma once
//#include <boost/noncopyable.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/shared_ptr.hpp>
#include <set>
#include <string>
using namespace std;

namespace udpindex
{
    class BlackList
        : public boost::noncopyable
        , public boost::enable_shared_from_this<BlackList>
    {
    public:
        typedef boost::shared_ptr<BlackList> p;
        static BlackList &Inst(){return *inst_;}

        bool Start(string file_name);

        bool Stop();

        bool IsAllow(string url);

        bool Refresh();

        bool IsEffect(){return is_effect_;}

    public:
        ~BlackList(void){is_running_ = false;}
    private:
        BlackList(void):is_running_(false),is_effect_(false){}

        void TrimLine(string &line)
        {
            size_t ps = line.find('#');
            if (ps != string::npos)
            {
                line = line.substr(0, ps);
            }
            boost::trim(line);
        }

    private:
        static p inst_;

        bool is_effect_;

        bool is_running_;
        string file_path_;

        set<string> black_list_;
    };
}
