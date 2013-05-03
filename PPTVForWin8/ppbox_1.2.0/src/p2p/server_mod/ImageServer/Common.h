// Common.h

#include <PFSCommon.h>
#include <Resource.h>
#include <PFSImageData.h>

#include "Statistics.h"

// 如果要使用AWE，定义该宏
#define USE_AWE

#ifndef _WIN32
#undef USE_AWE
#endif

#ifdef USE_AWE

#include <Memory/AWEMemoryPool.h>
typedef Util::AWEMemoryPool MemoryPool;
typedef MemoryPool::WeakPtr WeakPtr;
typedef MemoryPool::SharePtr SharePtr;

#else

#include <Memory/PageMemoryPool.h>
typedef Util::PageMemoryPool MemoryPool;
typedef MemoryPool::Ptr WeakPtr;
typedef MemoryPool::Ptr SharePtr;

#endif

#include <Generic/RawPointer.h>
#include <Memory/ObjectMemoryPool.h>
#include <Memory/SharedMemoryPool.h>
#include <Memory/PoolAllocator.h>

typedef boost::function<
    void (Util::ErrorCode, size_t)
> response_type;

typedef boost::function<
    void (Util::ErrorCode, WeakPtr const &)
> disk_task_response_type;

extern boost::asio::io_service global_io_service;

static char const * const SHARED_MEMORY_POOL_NAME = "ImageServer_SharedMemoryPool";

static const size_t SHID_CACHE = 1;
static const size_t SHID_DISK = 2;
static const size_t SHID_UDP_SERVER = 3;

struct PointerTraitsBinder
{
    template <typename _Ty>
    struct bind
    {
        typedef 
#ifdef MONITOR
            Util::SharedPointerTraits<_Ty>
#else
            Util::RawPointerTraits2<_Ty>
#endif
            type;
    };
};
