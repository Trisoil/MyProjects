// Cache.h

#ifndef _CACHE_H_
#define _CACHE_H_

#include <Container/Ordered.h>
#include <Container/List.h>
#include <boost/multi_index/member.hpp>

struct BlockStat
{
    BlockStat()
        : expire(0)
        , cost(0)
    {
    }

    Statistics visit; // 访问次数
    Statistics hit;   // Cache命中次数
    Statistics load;  // 加载次数
    Statistics loaded;  // 成功加载次数
    Statistics answer;  // 成功提供数据次数
    Statistics refused;  // 拒绝提供数据次数
    time_t expire; // 生命期
    size_t cost; // 根据历史推断的价值
};

struct ResCache;
struct DelayRead;

struct BlockCache
    : Util::OrderedHook<
        BlockCache, 
        PointerTraitsBinder::bind<
            BlockCache
        >::type
    >::type
    , Util::SharedMemoryPool::PoolObjectNoThrow
{
    BlockCache()
        : status(free)
        , delay_read_list_(NULL)
    {
    }

    ~BlockCache()
    {
    }

    void ReleaseData()
    {
        if (data) {
            MemoryPool::free(data);
            data.release();
            status = free;
        }
    }

    enum StatusEnum {
        free, 
        initing, 
        ready
    };

    StatusEnum status;
    WeakPtr data;
    union {
        DelayRead * delay_read_list_; // initing 状态有效
        ResCache * myres; // ready 状态有效
    };
    BlockStat stat;

    static size_t min_keep_expire; // 最小的保留截止时间，生命期在此之前的数据块没有连续性价值
};

struct BlockCmp1
{
    bool operator () (
        BlockCache const & l, 
        BlockCache const & r) const
    {
        return l.stat.expire < r.stat.expire;
    }
};

struct BlockCmp2
{
    bool operator () (
        BlockCache const & l, 
        BlockCache const & r) const
    {
        return l.stat.cost < r.stat.cost;
    }
};

struct ResStat
{
    ResStat()
        : cost(0)
        , load_priority(0)
        , num_cache(0)
        , num_cache2(0)
    {
    }

    Statistics visit; // 访问次数
    Statistics hit;   // Cache命中次数（总数）
    Statistics hit2;   // Cache命中次数（自然命中，还有一部分是连续访问命中）
    Statistics load;  // 加载次数
    Statistics loaded;  // 成功加载次数
    Statistics answer;  // 成功提供数据次数
    Statistics refused;  // 拒绝提供数据次数
    union {
        size_t cost;  // 淘汰的代价
        size_t block_size; // 块大小（Cache::all_stat）
    };
    size_t load_priority; // 加载积分
    size_t num_cache; // 缓存块数（总数）
    size_t num_cache2; // 缓存块数（已经超过停尸时间）
};

struct ResCache
    : Util::OrderedHook<
        ResCache, 
        PointerTraitsBinder::bind<
            ResCache
        >::type
    >::type
    , Util::SharedMemoryPool::PoolObjectNoThrow
{
    ResCache(
        size_t size, 
        size_t block_size)
        : status(normal)
    {
        num_block = (size - 1) / block_size + 1;
        blocks = new BlockCache[num_block];
    }

    ResCache()
        : status(normal)
        , num_block(0)
        , blocks(NULL)
    {
    }

    ~ResCache()
    {
        if (blocks)
            delete [] (BlockCache *)blocks;
        blocks = NULL;
        num_block = 0;
    }

    bool operator < (
        ResCache const & r) const
    {
        return stat.cost < r.stat.cost;
    }

    enum StatusEnum {
        normal, 
        paused, 
        deleting
    };

    typedef Util::Ordered<
        BlockCache, 
        Util::identity<BlockCache>, 
        BlockCmp2, 
        Util::ordered_non_unique_tag
    > BlockOrdered;

    StatusEnum status;
    size_t num_block;
    rebind<BlockCache>::type::pointer blocks;
    ResStat stat;
    // 数据块排序，历史访问量由小到大
    BlockOrdered block_orders;
};

struct Cache
    : Util::SharedMemoryPool::PoolObjectNoThrow
{
    typedef ResIdT<
        PointerTraitsBinder::bind<char>::type
    > ResId;

    struct ResMapNode
        : Util::OrderedHook<
            ResMapNode, 
            PointerTraitsBinder::bind<
                ResMapNode
            >::type
        >::type
        , Util::SharedMemoryPool::PoolObjectNoThrow
    {
        ResMapNode()
        {
        }

        ResMapNode(
            ResId const & rid, 
            ResCache * res)
            : first(rid)
            , second(res)
        {
        }

        ResId first;
        PointerTraitsBinder::bind<ResCache>::type::pointer second;
    };

    typedef Util::Ordered<
        ResMapNode, 
        boost::multi_index::member<
            ResMapNode const, 
            ResId const, 
            &ResMapNode::first
        >, 
        std::less<ResId>
    > ResMap;

    //typedef std::pair<
    //    ResId const, 
    //    PointerTraitsBinder::bind<ResCache>::type::pointer
    //> ResMapNode;

    //typedef std::map<
    //    ResId, 
    //    PointerTraitsBinder::bind<ResCache>::type::pointer, 
    //    std::less<ResId>, 
    //    Util::PoolAllocator<
    //        ResMapNode, 
    //        Util::SharedMemoryPool, 
    //        PointerTraitsBinder::bind<ResMapNode>::type
    //    >
    //> ResMap;

    typedef Util::Ordered<
        ResCache, 
        Util::identity<ResCache>, 
        std::less<ResCache>, 
        Util::ordered_non_unique_tag
    > ResOrdered;

    enum BlockQueueEnum
    {
        block_queue_1, 
        block_queue_2, 
    };

    BlockCache::pointer ReleaseBlock(
        BlockQueueEnum queue, 
        BlockCache::pointer blk)
    {
        assert(blk->status == BlockCache::ready);
        blk->ReleaseData();
        all_stat.num_cache--;
        blk->myres->stat.num_cache--;
        if (queue == block_queue_1) {
            blk->myres = NULL;
            blk = block_orders1.erase(blk);
        } else {
            all_stat.num_cache2--;
            blk->myres->stat.num_cache2--;
            ResCache * res = blk->myres;
            blk->myres = NULL;
            blk = res->block_orders.erase(blk);
        }
        return blk;
    }

    typedef Util::Ordered<
        BlockCache, 
        Util::identity<BlockCache>, 
        BlockCmp1, 
        Util::ordered_non_unique_tag
    > BlockOrdered1;

    typedef ResCache::BlockOrdered BlockOrdered2;

    // Id查找表
    ResMap res_map_;
    // 资源排序，历史访问量由小到大
    ResOrdered res_orders;
    // 数据块排序，寿命由小到大
    BlockOrdered1 block_orders1;
    // 数据块排序，历史访问量由小到大
    BlockOrdered2 reserved; // block_orders2;
    size_t num_res;
    size_t num_paused;
    size_t num_block;
    ResStat all_stat;
};

#endif // #ifndef _CACHE_H_
