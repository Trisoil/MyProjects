// DiskStatistics.h

#ifndef _DISKSTATISTICS_H_
#define _DISKSTATISTICS_H_

#include <Container/List.h>

struct DiskStatistics
    : Util::ListHook<
        DiskStatistics, 
        PointerTraitsBinder::bind<
            DiskStatistics
        >::type
    >::type
    , Util::SharedMemoryPool::PoolObjectNoThrow
{
    DiskStatistics()
        : queue(0)
        , ability(0)
        , num_res(0)
        , busy(0)
        , num_res_paused(0)
        , total_space(0)
        , used_space(0)
    {
    }

    Statistics recv;
    Statistics drop;
    Statistics send;
    // stat_->errû��ͬ�����������ƺ��޹ؽ�Ҫ
    Statistics err;
    Statistics trans;
    size_t queue;
    size_t ability; // ÿ�������������
    size_t num_res;
    size_t busy; // ������æ������
    size_t num_res_paused;
    size_t total_space; // MB;

    PointerTraitsBinder::bind<char>::type::pointer path;

    size_t used_space; // MB;
};

struct DiskStatisticsHead
    : Util::List<DiskStatistics>
    , Util::SharedMemoryPool::PoolObjectNoThrow
{
    Statistics recv;
    Statistics drop;
    Statistics send;
    // stat_->errû��ͬ�����������ƺ��޹ؽ�Ҫ
    Statistics err;
    Statistics trans;
    size_t ability;
    size_t free_space;
};

#endif // #ifndef _DISKSTATISTICS_H_
