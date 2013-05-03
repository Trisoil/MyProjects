#ifndef SUPER_NODE_BIG_FILE_LOCAL_DISK_H
#define SUPER_NODE_BIG_FILE_LOCAL_DISK_H

#include "../LocalDisk.h"
#include "../../ServiceHealthMonitor/ServiceComponent.h"
#include "ResourceMetaInfoDB.h"

namespace super_node
{

class BigFileLocalDiskManager;
struct WriteTaskCallbackWraper;

class BigFileLocalDisk
        : public ILocalDisk
        , public IServiceComponentHealthPredicate
        , public boost::enable_shared_from_this<BigFileLocalDisk>
        , public count_object_allocate<BigFileLocalDisk>
{
public:
    virtual ~BigFileLocalDisk();

    virtual void Start();
    virtual void Stop();

    virtual void UpdateConfig();

    /**
     * mutex is used to protect the state info,
     * generally it's better to invoke the [callback] without the mutex held;
     * for [AsyncRead], it's done this way,
     * but for [AsyncWrite and AsyncDeleteResources], the [callback] is invoked with the mutex held;
     * the reason is: the user of BigFileLocalDisk usually maintain a table of ResourceIdentifiers in [this],
     * and [callback] is used to update this table;
     * if the [callback] is invoked without the mutex held, then the table maintained by the user may result in inconsistence
     */
    virtual bool AsyncRead(const ResourceIdentifier& resource_identifier, int block_index, int read_priority, ReadResourceBlockCallback callback);
    virtual bool AsyncWrite(const ResourceIdentifier& resource_identifier, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks, WriteResourceTaskCallback callback);
    virtual bool AsyncDeleteResources(const std::vector<ResourceIdentifier>& resources, DeleteResourcesTaskCallback callback);

    virtual bool AsyncLoadResources(LoadResourcesTaskCallback callback, bool delete_resource_on_error);

    virtual boost::filesystem::path GetDiskPath() const;

    virtual size_t GetFreeSpacePercentage() const;
    virtual boost::uint64_t GetDiskSpaceInBytes() const;

    virtual bool IsComponentHealthy() const;

    virtual void Sync();

public:
    const static size_t DEFAULT_RESOURCE_BLOCK_SIZE;
    const static size_t DEFAULT_RESOURCE_SUB_BLOCK_SIZE;

protected:
    friend class BigFileLocalDiskManager;
    /**
     * if file_size_bound == 0, then the current file size achieved by lseek64 is used; this is usefull for raw-disk file
     */
    BigFileLocalDisk(BigFileLocalDiskManager* pManager, const std::string& path, boost::uint64_t file_size_bound);

    /**
     * ResourceMetaInfo:
     * this struct is used to track the meta info of a resource;
     * and all the meta info of the resources are recorded in the table [resource_meta_info_table_];
     *
     * there are two copys of meta info: memory-copy-metainfo, DB-copy-metainfo;
     *
     * the memory-copy-metainfo is inited with the DB-copy-metainfo,
     * the DB-copy-metainfo is updated periodically with the memory-copy-metainfo;
     *
     * attention: 
     *      to ensure correct behavior when restart after crash,
     *      the DB-copy-metainfo must be a subset of the memory-copy-metainfo at any time;
     */
    struct ResourceMetaInfo
    {
        boost::dynamic_bitset<boost::uint8_t> bitmap;
        size_t resource_size;
        boost::uint64_t resource_pos_index;

        enum PendingDBOperation
        {
            DB_NULL, // the meta-info saved in DB is up to date
            DB_INSERT, // this resource is new, and not recorded in DB
            DB_UPDATE,
            DB_DELETE, // this resource is marked as deleted, but DB deletion is pending(not completed)
            DB_OP_FAILURE_HAS_OCCURED, // if some DB operation of this resource has failed, subsequent DB operations of this resource will be ignored
        } pending_db_op;

        size_t pending_write_callback_count;
        size_t pending_read_callback_count;

        ResourceMetaInfo(const boost::dynamic_bitset<boost::uint8_t>& _bitmap, size_t _resource_size, boost::uint64_t _resource_pos_index);
        ResourceMetaInfo();

        bool HasPendingDBOp() { return pending_db_op == DB_INSERT || pending_db_op == DB_UPDATE || pending_db_op == DB_DELETE; }
    };

    /**
     * sizeof block is BlockData::MaxBlockSize, which may not equal to resource_sub_block_size_
     */
    bool CheckReadRange(const ResourceMetaInfo& meta, size_t block_id, boost::uint64_t& offset, size_t& len);
    bool CheckWriteRange(ResourceMetaInfo& meta, size_t block_id, boost::shared_ptr<BlockData> block, boost::uint64_t& offset, bool set_bitmap);

    bool CheckMetaInfo(const ResourceMetaInfo& meta);

    void ReadCallback(const ResourceIdentifier& resource_identifier, size_t block_index, ReadResourceBlockCallback callback, void* buf, size_t len, ErrorCode err, size_t bytes_transfered);
    void WriteCallback(boost::shared_ptr<WriteTaskCallbackWraper> wraper, size_t block_id, boost::shared_ptr<BlockData> block,
        size_t bytes_to_write, ErrorCode err, size_t bytes_transfered);

    void DoDeleteResources(const std::vector<ResourceIdentifier>& resources, DeleteResourcesTaskCallback callback);
    void DoLoadResources(LoadResourcesTaskCallback callback, bool delete_resource_on_error);

    void EndCallback(bool is_locked);

    void StartDBRefresh(bool always = false);

    boost::shared_ptr< std::vector<ResourceMetaInfoDB::Operation> >  GetDBOperations();
    void DBOpCallback(boost::shared_ptr< std::vector<ResourceMetaInfoDB::Operation> >  pOperations, bool all_ok);

    bool ResetMetaInfo();

    void DoSync(bool stop);

    void ReportStatus();

protected:
    bool running_;

    BigFileLocalDiskManager* pManager_;
    boost::filesystem::path path_;
    std::string path_name_;
    boost::shared_ptr<boost::asio::deadline_timer> timer_;

    class Monitor;
    boost::shared_ptr<Monitor> monitor_;

    boost::condition all_done_condition_; // used by DoSync to wait until all callbacks and DB operations are finished
    size_t pending_callback_count_;

    // resource data info: read from resource data file header
    size_t resource_block_size_;
    size_t resource_sub_block_size_;
    std::string resource_meta_info_table_name_;

    // meta info table:
    boost::mutex  mutex_;
    bool meta_info_loaded_;
    std::map<ResourceIdentifier, ResourceMetaInfo> resource_meta_info_table_;
    typedef std::map<ResourceIdentifier, ResourceMetaInfo>::iterator resource_meta_info_iterator;

    // config|state used by DB update cycle
    ResourceIdentifier  last_resource_scanned_;
    size_t db_op_batch_size_;
    size_t db_op_scan_size_;
    size_t pending_db_op_count_;
    std::set<ResourceIdentifier> resource_to_delete_;
    bool db_op_running_;

    // free positions in the resource data file, can be allocated for new resources
    // free_indexs_ U [next_resource_index_, resource_count_bound_)
    std::vector<boost::uint64_t>  free_indexs_;
    boost::uint64_t next_resource_index_;
    boost::uint64_t resource_count_bound_;
};

}

#endif
