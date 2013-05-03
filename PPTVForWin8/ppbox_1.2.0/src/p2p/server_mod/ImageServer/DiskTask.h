// DiskTask.h

#ifndef _DISKTASK_H_
#define _DISKTASK_H_

#include <Container/List.h>

struct DiskTask
    : Util::ListHook<DiskTask>::type
    , Util::ObjectMemoryPoolObjectNoThrow<DiskTask>
{
    enum TypeEnum
    {
        read_data, 
        write_data, 
        delete_res, 
        get_res, 
        stop, 
    };

    DiskTask() :type(stop) {};

    DiskTask(
        TypeEnum type, 
        ResId const & rid, 
        size_t offset, 
        size_t size, 
        SharePtr const & data, 
        disk_task_response_type const & r)
        : type(type)
        , rid(rid)
        , offset(offset)
        , size(size)
        , data(data)
        , r(r)
    {
    }

    // ReadNoCheck
    DiskTask(
        ResId const & rid, 
        size_t offset, 
        size_t size, 
        disk_task_response_type const & r)
        : type(read_data)
        , rid(rid)
        , offset(offset)
        , size(size)
        , priority((size_t)-1)
        , r(r)
    {
    }

    // Read
    DiskTask(
        ResId const & rid, 
        size_t offset, 
        size_t size, 
        size_t priority, 
        disk_task_response_type const & r)
        : type(read_data)
        , rid(rid)
        , offset(offset)
        , size(size)
        , priority(priority)
        , r(r)
    {
    }

    // Write
    DiskTask(
        ResId const & rid, 
        size_t offset, 
        size_t size, 
        SharePtr const & data, 
        disk_task_response_type const & r)
        : type(write_data)
        , rid(rid)
        , offset(offset)
        , size(size)
        , priority((size_t)-1)
        , data(data)
        , r(r)
    {
    }

    // Delete
    DiskTask(
        ResId const & rid, 
        disk_task_response_type const & r)
        : type(delete_res)
        , rid(rid)
        , offset(0)
        , size(0)
        , priority((size_t)-1)
        , r(r)
    {
    }

    // GetList
    DiskTask(
        std::map<ResId, size_t> & ress, 
        disk_task_response_type const & r)
        : type(get_res)
        , size(0)
        , priority((size_t)-1)
        , r(r)
    {
        offset = (size_t)&ress;
    }

    TypeEnum type;
    ResId rid;
    size_t offset;
    size_t size;
    size_t priority;
    SharePtr data;
    disk_task_response_type r;
};

#endif // #ifndef _DISKTASK_H_
