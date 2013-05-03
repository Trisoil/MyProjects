// OpenManager.h

#include <framework/timer/ClockTime.h>
using namespace framework::timer;

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>

class OpenManager
{
public:
    OpenManager(
        InputHandler & std_in, 
        bool async, 
        boost::uint32_t sec)
        : std_in_(std_in)
        , async_(async)
        , time_out_(sec)
        , ec_(0)
    {
        inst_ = this;
    }

    ~OpenManager()
    {
        inst_ = NULL;
    }

    PP_int32 open(
        char const * playlink)
    {
        boost::mutex::scoped_lock lock(mutex_);

        if (async_) {
            PPBOX_AsyncOpen(playlink, static_call_back);
        } else {
            boost::thread th(boost::bind(&OpenManager::open_thread, playlink));
        }

        Time expire = Time::now();
        expire += Duration::seconds(time_out_);
        bool finished = false;
        while (time_out_ == 0 || expire > Time::now()) {
            if ((finished = cond_.timed_wait(mutex_, boost::posix_time::milliseconds(100)))) {
                break;
            }
            std::vector<std::string> cmd_args;
            if (std_in_.get_one(cmd_args) && cmd_args[0] == "close") {
                break;
            }
        }
        if (!finished) {
            std::cout << "open: cancel" << std::endl;
            PPBOX_Close();
            cond_.wait(mutex_);
        }
        return ec_;
    }

private:
    static void open_thread(
        char const * playlink)
    {
        PP_int32 ec = PPBOX_Open(playlink);
        std::cout << "open: open_thread" << std::endl;
        inst_->call_back(ec);
    }

    static void static_call_back(
        PP_int32 ec)
    {
        std::cout << "open: static_call_back" << std::endl;
        inst_->call_back(ec);
    }

    void call_back(
        PP_int32 ec)
    {
        ec_ = ec;
        boost::mutex::scoped_lock lock(mutex_);
        cond_.notify_all();
    }

private:
    static OpenManager * inst_;

private:
    InputHandler & std_in_;
    bool async_;
    boost::uint32_t time_out_;
    PP_int32 ec_;
    boost::mutex mutex_;
    boost::condition cond_;
};

OpenManager * OpenManager::inst_ = NULL;
