// MemoryPool.cpp

#include "tools/test/Common.h"

#ifdef _DEBUG

#include <framework/memory/BigFirstPool.h>
#include <framework/memory/PrivateMemory.h>
using namespace framework::memory;

#include <boost/thread/mutex.hpp>
using namespace boost;

static BigFirstPool & get_pool()
{
    static BigFirstPool pool(PrivateMemory(), 300 * 1024 * 1024);
    return pool;
}

static BigFirstPool & s_pool = get_pool();

static mutex & get_mutex()
{
    static mutex mtx;
    return mtx;
}

void * operator new (size_t size)
{
    mutex::scoped_lock lock(get_mutex());
    void * ptr = get_pool().alloc(size);
    if (ptr == NULL) {
        std::cout << "consumption: " << get_pool().consumption() << std::endl;
    }
    return ptr;
}

void operator delete (void * ptr)
{
    mutex::scoped_lock lock(get_mutex());
    get_pool().free(ptr);
}

void * operator new [](size_t size)
{
    mutex::scoped_lock lock(get_mutex());
    void * ptr = get_pool().alloc(size);
    if (ptr == NULL) {
        std::cout << "consumption: " << get_pool().consumption() << std::endl;
    }
    return ptr;
}

void operator delete [](void * ptr)
{
    mutex::scoped_lock lock(get_mutex());
    get_pool().free(ptr);
}

void pool_dump()
{
    BigFirstPool & p = get_pool();
    std::cout << "[MemoryPool]"
        << " consumption: " << p.consumption()
        << " peek: " << p.peek()
        << " num_block: " << p.num_block()
        << " num_object: " << p.num_object() << std::endl;
}

#else

void pool_dump()
{
}

#endif // _DEBUG
