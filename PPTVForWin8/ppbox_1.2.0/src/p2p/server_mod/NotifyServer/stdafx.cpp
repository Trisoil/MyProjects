// stdafx.cpp : 只包括标准包含文件的源文件
// p2pvideo.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include "framework/timer/AsioTimerManager.h"

boost::asio::io_service & global_io_svc()
{
    static boost::asio::io_service * io_svc = new boost::asio::io_service();
    return *io_svc;
}

framework::timer::TimerQueue & global_second_timer()
{
    static framework::timer::AsioTimerManager timer_manager(global_io_svc(), boost::posix_time::seconds(1));
    return timer_manager;
}
// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
