#include "BigFileLocalDisk.h"
#include "ResourceMetaInfoDB.h"
#include "BigFileLocalDiskManager.h"
#include "BlockDataAllocator.h"
#include <set>
#include <uuid/uuid.h>
#include <framework/string/Md5.h>
#include <stdexcept>
#include <cstdlib>
#include <memory>

#include "../../ServiceHealthMonitor/ComponentStatusItem.h"
#include "../../ServiceHealthMonitor.h"
#include "../../Statistics/NumericStatistics.h"
#include "../../Loggers.h"
#include <framework/configure/Config.h>
#include "../../ConfigManager/ConfigManager.h"


namespace super_node
{

/**
 * file format is simple:
 * 16K header followed by an array of Resource Blocks, and each Resource Block is divided into sub-blocks;
 * the size of each resource block is resource_block_size_,
 * the size of each resource sub-block is resource_sub_block_size_;
 */

#define FILE_FORMAT_VERSION "resource data format 1.0"

struct BigFileHeader
{
    char version[32];
    char meta_info_table_name[32];  //  an uuid
    char resource_block_size[16];
    char resource_sub_block_size[16];
    char md5_check_sum[32];

    bool CheckMD5()
    {
        std::string md5 = CalMD5();
        return md5.size() == sizeof(md5_check_sum) && memcmp(md5.c_str(), md5_check_sum, sizeof(md5_check_sum)) == 0;
    }
    void SetMD5()
    {
        std::string md5 = CalMD5();
        assert(md5.size() == sizeof(md5_check_sum));
        memcpy(md5_check_sum, md5.c_str(), sizeof(md5_check_sum));
    }
    std::string CalMD5();
    void GenerateMetaInfoTableName();
};

std::string BigFileHeader::CalMD5()
{
    framework::string::Md5 md5;

    md5.update((unsigned char*)version, sizeof(version));
    md5.update((unsigned char*)meta_info_table_name, sizeof(meta_info_table_name));
    md5.update((unsigned char*)resource_block_size, sizeof(resource_block_size));
    md5.update((unsigned char*)resource_sub_block_size, sizeof(resource_sub_block_size));

    md5.final();

    return md5.to_string();
}

void BigFileHeader::GenerateMetaInfoTableName()
{
    uuid_t  uid;
    uuid_generate(uid);

    char buf[128] = { 0 };

    for (size_t i = 0; i < sizeof(uuid_t); ++i)
    {
        sprintf(buf + 2 * i, "%02x", uid[i]);
    }

    buf[0] = 'T'; // the first character can't be a digit

    memcpy(meta_info_table_name, buf, sizeof(meta_info_table_name));
}

const size_t BigFileLocalDisk::DEFAULT_RESOURCE_BLOCK_SIZE = 21 << 20; // 21 M
const size_t BigFileLocalDisk::DEFAULT_RESOURCE_SUB_BLOCK_SIZE = 16 << 10;

static int DEFAULT_WRITE_PRIORITY = 30;

static const size_t RESERVED_HEADER_SIZE = 16 << 10; // 16K

/**
 * raw disk IO must be aligned to the size of disk-sector and [pagesize], we use 16K here
 */
static const unsigned int  IO_ALIGN = 16 << 10;
static inline size_t up_align_io_size(size_t len) { return (len + IO_ALIGN - 1) & ~(IO_ALIGN - 1); }

class BigFileLocalDisk::Monitor
{
public:
    Monitor(boost::shared_ptr<BigFileLocalDisk> owner, const std::string& path);
    ~Monitor();
    void ReportStatus(size_t res_total, size_t res_free);

    std::string path_;
    framework::timer::TimeCounter time_counter_;

    boost::shared_ptr<ComponentStatusItem> ticks_since_last_report_;

    boost::shared_ptr<ComponentStatusItem> sti_read_submitted_;
    boost::shared_ptr<ComponentStatusItem> sti_read_finished_;
    boost::shared_ptr<ComponentStatusItem> sti_read_speed_;
    boost::shared_ptr<ComponentStatusItem> sti_read_dropped_;
    boost::shared_ptr<ComponentStatusItem> sti_read_failure_;

    boost::shared_ptr<ComponentStatusItem> sti_write_submitted_;
    boost::shared_ptr<ComponentStatusItem> sti_write_finished_;
    boost::shared_ptr<ComponentStatusItem> sti_write_speed_;
    boost::shared_ptr<ComponentStatusItem> sti_write_dropped_;
    boost::shared_ptr<ComponentStatusItem> sti_write_failure_;

    boost::shared_ptr<ComponentStatusItem> sti_average_pending_callback_count_;

    boost::shared_ptr<ComponentStatusItem> sti_res_total_;
    boost::shared_ptr<ComponentStatusItem> sti_res_free_;
    boost::shared_ptr<ComponentStatusItem> sti_res_free_percent_;

    NumericStatistics  read_submitted_;
    NumericStatistics  read_finished_;
    NumericStatistics  read_finished_in_bytes_;
    NumericStatistics  read_dropped_;
    NumericStatistics  read_failure_;

    NumericStatistics  write_submitted_;
    NumericStatistics  write_finished_;
    NumericStatistics  write_finished_in_bytes_;
    NumericStatistics  write_dropped_;
    NumericStatistics  write_failure_;
    NumericStatistics  average_pending_callback_count_;
};


BigFileLocalDisk::ResourceMetaInfo::ResourceMetaInfo(const boost::dynamic_bitset<boost::uint8_t>& _bitmap, size_t _resource_size, boost::uint64_t _resource_pos_index)
    : bitmap(_bitmap)
    , resource_size(_resource_size)
    , resource_pos_index(_resource_pos_index)
    , pending_db_op(DB_NULL)
    , pending_write_callback_count(0)
    , pending_read_callback_count(0)
{ }

BigFileLocalDisk::ResourceMetaInfo::ResourceMetaInfo()
    : resource_size(0)
    , resource_pos_index( (boost::uint64_t)(-1) )
    , pending_db_op(DB_INSERT)
    , pending_write_callback_count(0)
    , pending_read_callback_count(0)
{ }

BigFileLocalDisk::BigFileLocalDisk(BigFileLocalDiskManager* pManager, const std::string& path, boost::uint64_t file_size_bound)
    : running_(false)
    , pManager_(pManager)
    , path_(path)
    , pending_callback_count_(0)
    , resource_block_size_(DEFAULT_RESOURCE_BLOCK_SIZE)
    , resource_sub_block_size_(DEFAULT_RESOURCE_SUB_BLOCK_SIZE)
    , meta_info_loaded_(false)
    , db_op_batch_size_(1000)
    , db_op_scan_size_(50000)
    , pending_db_op_count_(0)
    , db_op_running_(false)
    , next_resource_index_(0)
    , resource_count_bound_(0)
{
    assert(pManager_);
    path_name_ = path_.string();

    BigFileHeader  header;

    int fd = open(path_name_.c_str(), O_RDONLY);

    if (fd < 0)
        goto reset;

    if (file_size_bound == 0)
    {
        off64_t size = lseek64(fd, 0, SEEK_END);
        if (size < 0)
            goto reset;

        file_size_bound = size;

        if (lseek64(fd, 0, SEEK_SET) < 0)
            goto reset;
    }

    if (file_size_bound < RESERVED_HEADER_SIZE)
        goto reset;

    if (read(fd, &header, sizeof(header)) != sizeof(header))
    {
        goto reset;
    }

    close(fd);
    fd = -1;

    if (!header.CheckMD5())
    {
        if (strncmp(header.version, FILE_FORMAT_VERSION, sizeof(header.version)) == 0)
        {
            // corrupted!
            LOG4CPLUS_WARN(Loggers::LocalDisk(), path_name_ << " - File header corrupted");
        }

        goto reset;
    }

    header.version[sizeof(header.version) - 1] = 0;
    if (strcmp(header.version, FILE_FORMAT_VERSION) != 0)
    {
        // version mismatch
        LOG4CPLUS_WARN(Loggers::LocalDisk(), path_name_ << " - Version mismatch");
        goto reset;
    }

    if (header.meta_info_table_name[sizeof(header.meta_info_table_name) - 1])
        resource_meta_info_table_name_.assign(header.meta_info_table_name, sizeof(header.meta_info_table_name));
    else
        resource_meta_info_table_name_.assign(header.meta_info_table_name);

    header.resource_block_size[sizeof(header.resource_block_size) - 1] = 0;
    header.resource_sub_block_size[sizeof(header.resource_sub_block_size) - 1] = 0;

    resource_block_size_ = atoi(header.resource_block_size);
    resource_sub_block_size_ = atoi(header.resource_sub_block_size);

    if (resource_block_size_ < IO_ALIGN || resource_block_size_ > (1 << 30) ||
        resource_sub_block_size_ < IO_ALIGN || resource_sub_block_size_ % IO_ALIGN || resource_sub_block_size_ > resource_block_size_ ||
        resource_block_size_ % resource_sub_block_size_)
    {
        assert(false);
        goto reset;
    }

    if (BlockData::MaxBlockSize % resource_sub_block_size_)
    {
        assert(false);
        throw std::runtime_error("BlockData Buf size is not multiple of resource-sub-block-size");
    }

    resource_count_bound_ = (file_size_bound - RESERVED_HEADER_SIZE) / resource_block_size_;

    return;

reset:
    if (fd >= 0)
        close(fd);

    if (file_size_bound < RESERVED_HEADER_SIZE || !ResetMetaInfo())
        throw std::runtime_error(std::string("can't setup big-file-local-disk: ") + path_name_);

    resource_count_bound_ = (file_size_bound - RESERVED_HEADER_SIZE) / resource_block_size_;
}

BigFileLocalDisk::~BigFileLocalDisk()
{
    DoSync(true);
}

void BigFileLocalDisk::Start()
{
    UpdateConfig();

    boost::mutex::scoped_lock  guard(mutex_);

    if (!monitor_)
        monitor_.reset(new Monitor(shared_from_this(), path_name_));

    if (!timer_)
        timer_ = pManager_->CreateTimer();
    timer_->expires_from_now(boost::posix_time::seconds(5));
    timer_->async_wait(bind(&BigFileLocalDisk::ReportStatus, shared_from_this()));

    if (meta_info_loaded_)
        running_ = true;
}

void BigFileLocalDisk::Stop()
{
    DoSync(true);
}

void BigFileLocalDisk::UpdateConfig()
{
    framework::configure::Config conf(ConfigManager::AllConfigFileName);

    boost::mutex::scoped_lock  guard(mutex_);

    conf.register_module("LocalDiskCache")
        << CONFIG_PARAM_NAME_RDONLY("db_op_batch_size", db_op_batch_size_)
        << CONFIG_PARAM_NAME_RDONLY("db_op_scan_size", db_op_scan_size_);

    if (!db_op_scan_size_)
        db_op_scan_size_ = 50000;

    if (!db_op_batch_size_ || db_op_batch_size_ > db_op_scan_size_)
        db_op_batch_size_ = db_op_scan_size_;
}

void BigFileLocalDisk::Sync()
{
    DoSync(false);
}

boost::filesystem::path BigFileLocalDisk::GetDiskPath() const
{
    return path_;
}

size_t BigFileLocalDisk::GetFreeSpacePercentage() const
{
    return (resource_count_bound_ - next_resource_index_ + free_indexs_.size()) * 100 / (resource_count_bound_ + 1);
}

boost::uint64_t BigFileLocalDisk::GetDiskSpaceInBytes() const
{
    return (resource_count_bound_ - next_resource_index_ + free_indexs_.size()) * resource_block_size_;
}

void BigFileLocalDisk::ReportStatus()
{
    boost::mutex::scoped_lock  guard(mutex_);

    if (monitor_)
        monitor_->ReportStatus((size_t)resource_count_bound_, (size_t)(resource_count_bound_ - next_resource_index_ + free_indexs_.size()) );

    if (timer_)
    {
        timer_->expires_from_now(boost::posix_time::seconds(5));
        timer_->async_wait(bind(&BigFileLocalDisk::ReportStatus, shared_from_this()));
    }
}

bool BigFileLocalDisk::AsyncRead(const ResourceIdentifier& resource_identifier, int block_index, int read_priority,
        ReadResourceBlockCallback callback
    )
{
    std::auto_ptr<boost::mutex::scoped_lock>  guard(new boost::mutex::scoped_lock(mutex_));

    ErrorCode err = ErrorCodes::UnknownError;
    resource_meta_info_iterator it = resource_meta_info_table_.find(resource_identifier);

    if (!running_)
        err = ErrorCodes::ServiceStopped;
    else if (it == resource_meta_info_table_.end() || it->second.pending_db_op == ResourceMetaInfo::DB_DELETE)
    {
        // in the case of pending_db_op == DB_DELETE,
        // actually we can change pending_db_op to DB_NULL, and read this resource, but doing this will cause confusion to the user;
        // so it's better to avoid it;
        err = ErrorCodes::ResourceNotFound;
    }
    else
    {
        boost::uint64_t offset;
        size_t len;

        if (!CheckReadRange(it->second, block_index, offset, len))
            err = ErrorCodes::ResourceNotFound;
        else
        {
            assert( (offset & (IO_ALIGN - 1)) == 0 ); // must align to IO_ALIGN
            void* buf = BlockDataAllocator::Instance().Allocate();

            if (!buf)
                err = ErrorCodes::OutOfMemory;
            else
            {
                it->second.pending_read_callback_count++;
                ++pending_callback_count_;

                monitor_->average_pending_callback_count_.Add(pending_callback_count_);
                monitor_->read_submitted_.Add(1);

                // pManager_->AsyncRead(...) may invoke callbacks, so release the mutex
                guard.reset();

                pManager_->AsyncRead(path_name_, offset, buf, up_align_io_size(len), read_priority,
                        boost::bind(&BigFileLocalDisk::ReadCallback, shared_from_this(), resource_identifier, block_index, callback, buf, len, _1, _2)
                    );

                return true;
            }
        }
    }

    guard.reset();
    callback(err, boost::shared_ptr<BlockData>() );

    return false;
}


/**
 * WriteTaskCallbackWraper is used to group multiple block-write callbacks into one resource-write callback
 */
struct WriteTaskCallbackWraper
{
    ErrorCode  err_;
    ResourceIdentifier  resource_identifier_;
    WriteResourceTaskCallback  callback_;

    boost::mutex  mutex_;
    size_t pending_callback_count_;

    WriteTaskCallbackWraper(const ResourceIdentifier& resource_identifier, WriteResourceTaskCallback callback, size_t pending_count)
        : err_(ErrorCodes::Success)
        , resource_identifier_(resource_identifier)
        , callback_(callback)
        , pending_callback_count_(pending_count)
    { } 
    ~WriteTaskCallbackWraper()
    {
        assert(pending_callback_count_ == 0);
    }
    void do_callback(ErrorCode err)
    {
        bool pending = true;
        {
            boost::mutex::scoped_lock  guard(mutex_);

            if (err != ErrorCodes::Success)
                err_ = ErrorCodes::DiskWriteFailure;

            assert(pending_callback_count_);
            pending = --pending_callback_count_ > 0;
        }

        if (!pending)
            callback_(err_, resource_identifier_);
    }
};

struct ValidBlockToWrite
{
    size_t  id;
    boost::shared_ptr<BlockData>  block;
    boost::uint64_t  offset;

    ValidBlockToWrite(size_t _id, boost::shared_ptr<BlockData> _block, boost::uint64_t _offset)
        : id(_id)
        , block(_block)
        , offset(_offset)
    { }
};

bool BigFileLocalDisk::AsyncWrite(const ResourceIdentifier& resource_identifier, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks, WriteResourceTaskCallback callback)
{
    std::auto_ptr<boost::mutex::scoped_lock>  guard(new boost::mutex::scoped_lock(mutex_));

    resource_meta_info_iterator r_it = resource_meta_info_table_.find(resource_identifier);

    if (!running_)
    {
        callback(ErrorCodes::ServiceStopped, resource_identifier);
        return false;
    }

    bool pre_has_pending_dbop = false;

    if (r_it == resource_meta_info_table_.end())
    {
        ResourceMetaInfo  meta;

        // allocate a position for the new resource
        if (!free_indexs_.empty())
        {
            meta.resource_pos_index = free_indexs_.back();
            free_indexs_.pop_back();
        }
        else if (next_resource_index_ < resource_count_bound_)
        {
            meta.resource_pos_index = next_resource_index_++;
        }
        else
        {
            callback(ErrorCodes::DiskWriteFailure, resource_identifier);
            return false;
        }

        resource_meta_info_table_.insert( std::make_pair(resource_identifier, meta) );
        r_it = resource_meta_info_table_.find(resource_identifier);
    }
    else
    {
        pre_has_pending_dbop = r_it->second.HasPendingDBOp();
    }

    if (blocks.empty())
    {
        if (!pre_has_pending_dbop && r_it->second.HasPendingDBOp())
        {
            ++pending_db_op_count_;
            StartDBRefresh();
        }

        callback(ErrorCodes::Success, resource_identifier);

        return true;
    }

    boost::shared_ptr<WriteTaskCallbackWraper>  callback_wraper( new WriteTaskCallbackWraper(resource_identifier, callback, blocks.size()) );
    std::vector<ValidBlockToWrite>  valid_blocks;
    valid_blocks.reserve(blocks.size());
    size_t invalid_block_cnt = 0;

    for (std::map<size_t, boost::shared_ptr<BlockData> >::const_iterator b_it = blocks.begin(); b_it != blocks.end(); ++b_it)
    {
        if (!b_it->second || b_it->second->Size() == 0)
        {
            callback_wraper->do_callback(ErrorCodes::Success);
            continue;
        }

        boost::uint64_t offset;
        bool ok = CheckWriteRange(r_it->second, b_it->first, b_it->second, offset, false);

        if (ok)
        {
            r_it->second.pending_write_callback_count++;
            ++pending_callback_count_;

            valid_blocks.push_back(ValidBlockToWrite(b_it->first, b_it->second, offset));

            monitor_->average_pending_callback_count_.Add(pending_callback_count_);
            monitor_->write_submitted_.Add(1);
        }
        else
        {
            ++invalid_block_cnt;

            LOG4CPLUS_ERROR(Loggers::LocalDisk(), path_name_ << " - Invalid write request: "
                    << resource_identifier.GetResourceName() << "|" << b_it->first << "|" << b_it->second->Size()
                );
        }
    }

    if (r_it->second.pending_db_op == ResourceMetaInfo::DB_NULL || r_it->second.pending_db_op == ResourceMetaInfo::DB_DELETE)
    {
        // can't change pending_db_op to DB_INSERT if its current value is DB_DELETE,
        // for the deletion may not be in progress; and if its value is changed to DB_INSERT in this case, the deletion is canceled, and
        // the subsequent DB openration(INSERT) will fail;
        //
        // the method used here is change its value to DB_UPDATE;
        // if the DB deletion is not in progress, the deletion is canceled and DB_UPDATE is what we want,
        // otherwise the callback of DB operation will detect this and change pending_db_op from DB_UPDATE to DB_INSERT;

        // if the current value of pending_db_op is DB_NULL, it's possible that no update is really needed;
        // but the probability is very low and change it to DB_UPDATE cause no harm

        if (r_it->second.pending_db_op == ResourceMetaInfo::DB_DELETE)
            resource_to_delete_.erase(r_it->first);

        r_it->second.pending_db_op = ResourceMetaInfo::DB_UPDATE;
    }

    if (!pre_has_pending_dbop && r_it->second.HasPendingDBOp())
    {
        ++pending_db_op_count_;
        StartDBRefresh();
    }

    bool ret = true;

    if (invalid_block_cnt)
    {
        while (invalid_block_cnt--)
        {
            callback_wraper->do_callback(ErrorCodes::DiskWriteFailure);
        }

        ret = false;
    }

    // pManager_->AsyncWrite(...) may invoke callbacks, so release the mutex
    guard.reset();

    for (std::vector<ValidBlockToWrite>::iterator it = valid_blocks.begin(); it != valid_blocks.end(); ++it)
    {
        pManager_->AsyncWrite(path_name_, it->offset, it->block->Buf(), up_align_io_size(it->block->Size()), DEFAULT_WRITE_PRIORITY,
                        boost::bind(&BigFileLocalDisk::WriteCallback, shared_from_this(), callback_wraper, it->id, it->block, up_align_io_size(it->block->Size()), _1, _2)
                );
    }

    return ret;
}

void BigFileLocalDisk::ReadCallback(const ResourceIdentifier& resource_identifier, size_t block_index, ReadResourceBlockCallback callback, void* buf, size_t len, ErrorCode err, size_t bytes_transfered)
{
    {
        boost::mutex::scoped_lock  guard(mutex_);

        resource_meta_info_iterator it = resource_meta_info_table_.find(resource_identifier);
        assert(it != resource_meta_info_table_.end());

        it->second.pending_read_callback_count--;

        // update monitor 
        if (err == ErrorCodes::Success)
        {
            monitor_->read_finished_.Add(1);
            monitor_->read_finished_in_bytes_.Add(bytes_transfered);
        }
        else if (ErrorCodes::ServiceBusy)
            monitor_->read_dropped_.Add(1);
        else
            monitor_->read_failure_.Add(1);
    }

    if (err == ErrorCodes::Success && bytes_transfered == up_align_io_size(len))
    {
        boost::shared_ptr<BlockData> block(new BlockData((char*)buf, len)); // ownership of buf is given to block
        callback(ErrorCodes::Success, block);

        LOG4CPLUS_INFO(Loggers::LocalDisk(), "Block read completed: " << resource_identifier.GetResourceName() << "|" << block_index);
    }
    else
    {
        BlockDataAllocator::Instance().Free(buf);

        if (err == ErrorCodes::ServiceBusy)
        {
            callback(ErrorCodes::ServiceBusy, boost::shared_ptr<BlockData>() );
            LOG4CPLUS_INFO(Loggers::LocalDisk(), "Block read cancelled: " << resource_identifier.GetResourceName() << "|" << block_index);
        }
        else
        {
            callback(ErrorCodes::DiskReadFailure, boost::shared_ptr<BlockData>() );
            LOG4CPLUS_ERROR(Loggers::LocalDisk(), "Block read failed: " << resource_identifier.GetResourceName() << "|" << block_index);
        }
    }

    EndCallback(false);
}

void BigFileLocalDisk::WriteCallback(boost::shared_ptr<WriteTaskCallbackWraper> wraper, size_t block_id, boost::shared_ptr<BlockData> block,
        size_t bytes_to_write, ErrorCode err, size_t bytes_transfered)
{
    boost::mutex::scoped_lock  guard(mutex_);

    resource_meta_info_iterator it = resource_meta_info_table_.find(wraper->resource_identifier_);
    assert(it != resource_meta_info_table_.end());

    if (err == ErrorCodes::Success && bytes_to_write == bytes_transfered)
    {
        boost::uint64_t offset;
        // update meta info bitmap
        CheckWriteRange(it->second, block_id, block, offset, true);

        monitor_->write_finished_.Add(1);
        monitor_->write_finished_in_bytes_.Add(bytes_transfered);

        LOG4CPLUS_INFO(Loggers::LocalDisk(), "Block write completed: " << wraper->resource_identifier_.GetResourceName() << "|" << block_id);
    }
    else
    {
        if (err == ErrorCodes::ServiceBusy)
        {
            monitor_->write_dropped_.Add(1);
            LOG4CPLUS_INFO(Loggers::LocalDisk(), "Block write cancelled: " << wraper->resource_identifier_.GetResourceName() << "|" << block_id);
        }
        else {
            monitor_->write_failure_.Add(1);
            err = ErrorCodes::DiskWriteFailure;
            LOG4CPLUS_ERROR(Loggers::LocalDisk(), "Block write failed: " << wraper->resource_identifier_.GetResourceName() << "|" << block_id);
        }
    }

    if (--(it->second.pending_write_callback_count) == 0)
        StartDBRefresh();

    wraper->do_callback(err);

    EndCallback(true);
}

bool BigFileLocalDisk::AsyncDeleteResources(const std::vector<ResourceIdentifier>& resources, DeleteResourcesTaskCallback callback)
{
    boost::mutex::scoped_lock  guard(mutex_);

    if (running_)
    {
        // let the thread pool of pManager_ do it
        ++pending_callback_count_;
        pManager_->Post( boost::bind(&BigFileLocalDisk::DoDeleteResources, shared_from_this(), resources, callback) );
    }
    else
    {
        callback(ErrorCodes::ServiceStopped, std::vector<ResourceIdentifier>());
    }

    return true;
}

void BigFileLocalDisk::DoDeleteResources(const std::vector<ResourceIdentifier>& resources, DeleteResourcesTaskCallback callback)
{
    boost::mutex::scoped_lock  guard(mutex_);

    std::vector<ResourceIdentifier> deleted_reses;

    if (meta_info_loaded_)
    {
        for (std::vector<ResourceIdentifier>::const_iterator it = resources.begin(); it != resources.end(); ++it)
        {
            resource_meta_info_iterator m_it = resource_meta_info_table_.find(*it);

            if (m_it != resource_meta_info_table_.end())
            {
                if (m_it->second.pending_read_callback_count == 0 && m_it->second.pending_write_callback_count == 0)
                {
                    if (m_it->second.pending_db_op == ResourceMetaInfo::DB_INSERT)
                    {
                        LOG4CPLUS_INFO(Loggers::LocalDisk(), "Resource deleted: " << m_it->first.GetResourceName());

                        // not recorded in DB, erase it from memory directly
                        free_indexs_.push_back(m_it->second.resource_pos_index);
                        resource_meta_info_table_.erase(m_it);

                        deleted_reses.push_back(*it);

                        --pending_db_op_count_;
                    }
                    else if (m_it->second.pending_db_op == ResourceMetaInfo::DB_NULL || m_it->second.pending_db_op == ResourceMetaInfo::DB_UPDATE)
                    {
                        if (m_it->second.pending_db_op == ResourceMetaInfo::DB_NULL)
                            ++pending_db_op_count_;
                        resource_to_delete_.insert(m_it->first);

                        m_it->second.pending_db_op = ResourceMetaInfo::DB_DELETE;
                        deleted_reses.push_back(*it);
                        // the [m_it] can't be erased now, the erasion can only be done after the DB operation is done
                    }
                }
            }
        }
    }

    callback(ErrorCodes::Success, deleted_reses);

    EndCallback(true);
}

bool BigFileLocalDisk::AsyncLoadResources(LoadResourcesTaskCallback callback, bool delete_resource_on_error)
{
    boost::mutex::scoped_lock  guard(mutex_);

    ++pending_callback_count_;
    pManager_->Post( boost::bind(&BigFileLocalDisk::DoLoadResources, shared_from_this(), callback, delete_resource_on_error) );

    return true;
}

void BigFileLocalDisk::DoLoadResources(LoadResourcesTaskCallback callback, bool delete_resource_on_error)
{
    boost::mutex::scoped_lock  guard(mutex_);

    if (!meta_info_loaded_)
    {
        std::vector<ResourceMetaInfoDB::TableItem> table;

        bool ok = pManager_->GetResourceMetaInfoDB()->LoadTable(resource_meta_info_table_name_, table);

        if (ok)
        {
            std::set<boost::uint64_t>  pos_index_set;
            LOG4CPLUS_INFO(Loggers::LocalDisk(), path_name_ <<  " - meta info table loaded, the size is: " << table.size());

            for (std::vector<ResourceMetaInfoDB::TableItem>::const_iterator it = table.begin(); it != table.end(); ++it)
            {
                resource_meta_info_table_[it->resource_id] = ResourceMetaInfo(it->bitmap, it->resource_size, it->resource_pos_index);

                if (!CheckMetaInfo(resource_meta_info_table_[it->resource_id]))
                {
                    LOG4CPLUS_ERROR(Loggers::LocalDisk(), path_name_ << " - meta data loaded in is invalid " <<
                            path_name_ << "|" << resource_meta_info_table_name_
                        );

                    ok = false;
                    break;
                }

                pos_index_set.insert(it->resource_pos_index);
            }

            if (ok)
            {
                next_resource_index_ = pos_index_set.empty() ?  0 : *pos_index_set.rbegin() + 1;
                free_indexs_.clear();

                for (boost::uint64_t idx = 0; idx < next_resource_index_; ++idx)
                {
                    if (pos_index_set.find(idx) == pos_index_set.end())
                    {
                        free_indexs_.push_back(idx);
                    }
                }
            }
        }
        else
        {
            LOG4CPLUS_ERROR(Loggers::LocalDisk(), path_name_ << " - DB op failure: can't load in meta into table");
        }

        meta_info_loaded_ = ok || ResetMetaInfo();

        if (meta_info_loaded_)
            running_ = true;
    }

    std::vector<boost::shared_ptr<DiskResource> >  disk_resource_vec;

    for (resource_meta_info_iterator it = resource_meta_info_table_.begin(); it != resource_meta_info_table_.end(); ++it)
    {
        BlocksMap blocks_map;
        boost::shared_ptr<DiskResource> disk_resource(new DiskResource(it->first, false, blocks_map));

        disk_resource->SetSize(it->second.resource_size);

        size_t block_cnt = (it->second.bitmap.size() * resource_sub_block_size_ + BlockData::MaxBlockSize - 1) / BlockData::MaxBlockSize;

        for (size_t b = 0; b < block_cnt; ++b)
        {
            size_t i = (BlockData::MaxBlockSize * b) / resource_sub_block_size_;
            size_t j = (BlockData::MaxBlockSize * (b + 1)) / resource_sub_block_size_;

            if (it->second.resource_size)
                j = std::min(j, it->second.bitmap.size());
            else if (j > it->second.bitmap.size())
                break;

            if (i >= j)
                break;

            while (i < j && it->second.bitmap[i])
                ++i;

            if (i == j)
                disk_resource->AddBlock(b);
        }

        disk_resource_vec.push_back(disk_resource);
    }

    callback(meta_info_loaded_ ?  ErrorCodes::Success : ErrorCodes::DiskReadWriteFailure, disk_resource_vec);

    EndCallback(true);
}

void BigFileLocalDisk::StartDBRefresh(bool always)
{
    if (!db_op_running_ && pending_db_op_count_)
    {
        if (always || !resource_to_delete_.empty() || pending_db_op_count_ >= db_op_batch_size_)
        {
            boost::shared_ptr< std::vector<ResourceMetaInfoDB::Operation> >  pOperas = GetDBOperations();

            if (pOperas)
            {
                pManager_->GetResourceMetaInfoDB()->PostOperations(resource_meta_info_table_name_, pOperas,
                            boost::bind(&BigFileLocalDisk::DBOpCallback, shared_from_this(), pOperas, _1)
                    );

                db_op_running_ = true;
            }
        }
    }
}

boost::shared_ptr< std::vector<ResourceMetaInfoDB::Operation> >  BigFileLocalDisk::GetDBOperations()
{
    boost::shared_ptr< std::vector<ResourceMetaInfoDB::Operation> >  pOperations( new  std::vector<ResourceMetaInfoDB::Operation>() );
    std::vector<ResourceMetaInfoDB::Operation>& opera_vec = *pOperations;
    opera_vec.reserve(db_op_batch_size_);

    if (!resource_meta_info_table_.empty())
    {
        // handle DELETE in higher priority
        // the reason is: the user want to release space for new resources, and the space can only be released after the DB DELETE operation
        // has completed;
        // so if we don't handle DELETE in time, the user may try to delete more resources;
        for (std::set<ResourceIdentifier>::const_iterator p = resource_to_delete_.begin(); p != resource_to_delete_.end() && opera_vec.size() < db_op_batch_size_; ++p)
        {
            opera_vec.push_back(ResourceMetaInfoDB::Operation(p->GetResourceName()));
        }

        if (opera_vec.size() < db_op_batch_size_)
        {
            resource_meta_info_iterator p = resource_meta_info_table_.upper_bound(last_resource_scanned_);

            if (p == resource_meta_info_table_.end())
                p = resource_meta_info_table_.begin();

            size_t scan_cnt = std::min(resource_meta_info_table_.size(), db_op_scan_size_);

            while (scan_cnt-- && opera_vec.size() < db_op_batch_size_)
            {
                if (p->second.pending_write_callback_count == 0)
                {
                    if (p->second.pending_db_op == ResourceMetaInfo::DB_INSERT || p->second.pending_db_op == ResourceMetaInfo::DB_UPDATE)
                    {
                        opera_vec.push_back(
                                ResourceMetaInfoDB::Operation(
                                    p->second.pending_db_op == ResourceMetaInfo::DB_INSERT,
                                    p->first.GetResourceName(),
                                    p->second.bitmap,
                                    p->second.resource_size,
                                    p->second.resource_pos_index
                                )
                        );

                        p->second.pending_db_op = ResourceMetaInfo::DB_NULL;
                        --pending_db_op_count_;
                    }
                }

                if (++p == resource_meta_info_table_.end())
                    p = resource_meta_info_table_.begin();
            }

            last_resource_scanned_ = p->first;
        }
    }

    if (opera_vec.empty())
        pOperations.reset();

    return pOperations;
}

void BigFileLocalDisk::DBOpCallback(boost::shared_ptr< std::vector<ResourceMetaInfoDB::Operation> >  pOperations, bool all_ok)
{
    boost::mutex::scoped_lock  guard(mutex_);

    assert(db_op_running_);

    assert(pOperations);
    std::vector<ResourceMetaInfoDB::Operation>&  opera_vec = *pOperations;

    LOG4CPLUS_INFO(Loggers::LocalDisk(), path_name_ << 
            " - DB-OP callback(pre-batch-size: " << opera_vec.size() << ", pending: " << pending_db_op_count_ << ")"
        );

    for (std::vector<ResourceMetaInfoDB::Operation>::iterator p = opera_vec.begin(); p != opera_vec.end(); ++p)
    {
        resource_meta_info_iterator m_it = resource_meta_info_table_.find(ResourceIdentifier(p->item.resource_id));
        assert(m_it != resource_meta_info_table_.end());
        assert(m_it->second.pending_db_op != ResourceMetaInfo::DB_OP_FAILURE_HAS_OCCURED);

        if (p->result)
        {
            if (p->mode == ResourceMetaInfoDB::Operation::DELETE)
            {
                if (m_it->second.pending_db_op == ResourceMetaInfo::DB_UPDATE)
                {
                    m_it->second.pending_db_op = ResourceMetaInfo::DB_INSERT; // see the comment in AsyncWrite(...)
                }
                else if (m_it->second.pending_db_op == ResourceMetaInfo::DB_DELETE)
                {
                    LOG4CPLUS_INFO(Loggers::LocalDisk(), "Resource deleted: " << m_it->first.GetResourceName());

                    resource_to_delete_.erase(m_it->first);
                    --pending_db_op_count_;

                    free_indexs_.push_back(m_it->second.resource_pos_index);
                    resource_meta_info_table_.erase(m_it);
                }
                else
                {
                    assert(false);
                    LOG4CPLUS_ERROR(Loggers::LocalDisk(), "DB op callback: bug detected in db-refresh management");
                }
            }
        }
        else
        {
            // DB opera failed
            if (m_it->second.HasPendingDBOp())
                --pending_db_op_count_;

            if (m_it->second.pending_db_op == ResourceMetaInfo::DB_DELETE)
                resource_to_delete_.erase(m_it->first);

            m_it->second.pending_db_op = ResourceMetaInfo::DB_OP_FAILURE_HAS_OCCURED;
            assert(false);

            LOG4CPLUS_ERROR(Loggers::LocalDisk(), path_name_ << " - DB operation related to this file failed("
                    << (p->mode == ResourceMetaInfoDB::Operation::INSERT
                        ? "insert"
                        : (p->mode == ResourceMetaInfoDB::Operation::UPDATE ?  "update" : "delete")
                       )
                    << ")"
                );
        }
    }

    db_op_running_ = false;
    StartDBRefresh();

    if (!db_op_running_) // we don't do StartDBRefresh() looply here, DoSync(...) do it; so what we need to do here is notify DoSync(...);
        all_done_condition_.notify_all();
}

void BigFileLocalDisk::DoSync(bool stop)
{
    boost::mutex::scoped_lock  guard(mutex_);

    while (pending_callback_count_ || pending_db_op_count_ || db_op_running_)
    {
        if (pending_callback_count_ == 0 && !db_op_running_)
        {
            // the [all_done_condition_.wait(guard)] at the end of the loop is notified by EndCallback(...) & DBOpCallback(...),
            // there is no more EndCallback(...) nor DBOpCallback(...) at this point;
            //
            // so we must start db-refresh, otherwise none will notify us later.
            do {
                StartDBRefresh(true);
            } while (!db_op_running_);
        }

        all_done_condition_.wait(guard);
    }

    if (stop)
    {
        running_ = false;

        if (timer_)
        {
            boost::system::error_code cancel_error;
            timer_->cancel(cancel_error);
            timer_.reset();
        }

        if (monitor_)
            monitor_.reset();
    }
}

void BigFileLocalDisk::EndCallback(bool is_locked)
{
    std::auto_ptr<boost::mutex::scoped_lock>  guard(is_locked ?  0 : new boost::mutex::scoped_lock(mutex_));

    assert(pending_callback_count_);

    if (--pending_callback_count_ == 0)
        all_done_condition_.notify_all();

    monitor_->average_pending_callback_count_.Add(pending_callback_count_);
}

bool BigFileLocalDisk::CheckMetaInfo(const ResourceMetaInfo& meta)
{
    if (meta.resource_pos_index >= resource_count_bound_)
        return false;

    if (!meta.bitmap.empty() && !meta.bitmap.test(meta.bitmap.size() - 1))
        return false;

    if (meta.resource_size > resource_block_size_)
        return false;

    if (meta.resource_size && (meta.resource_size + resource_sub_block_size_ - 1) / resource_sub_block_size_ < meta.bitmap.size())
        return false;

    return true;
}

bool BigFileLocalDisk::CheckReadRange(const ResourceMetaInfo& meta, size_t block_id, boost::uint64_t& offset, size_t& len)
{
    size_t i = (block_id * BlockData::MaxBlockSize) / resource_sub_block_size_;
    size_t j = ((block_id + 1) * BlockData::MaxBlockSize) / resource_sub_block_size_;

    if (meta.resource_size)
        j = std::min(j, meta.bitmap.size());
    else if (j > meta.bitmap.size())
        return false;

    if (i >= j)
        return false;

    while (i < j && meta.bitmap[i])
        ++i;

    if (i == j)
    {
        offset = RESERVED_HEADER_SIZE + meta.resource_pos_index * resource_block_size_ + block_id * BlockData::MaxBlockSize;
        len = (meta.resource_size && meta.resource_size < (block_id + 1) * BlockData::MaxBlockSize) ?  meta.resource_size % BlockData::MaxBlockSize : BlockData::MaxBlockSize;

        return true;
    }

    return false;
}

bool BigFileLocalDisk::CheckWriteRange(ResourceMetaInfo& meta, size_t block_id, boost::shared_ptr<BlockData> block, boost::uint64_t& offset, bool set_bitmap)
{
    size_t i = (BlockData::MaxBlockSize * block_id) / resource_sub_block_size_;
    size_t j = (BlockData::MaxBlockSize * block_id + block->Size() + resource_sub_block_size_ - 1) / resource_sub_block_size_;

    if (j > resource_block_size_ / resource_sub_block_size_)
        return false;

    if (meta.resource_size)
    {
        if (block->Size() == BlockData::MaxBlockSize && j > meta.resource_size / resource_sub_block_size_)
            return false;

        if (block->Size() < BlockData::MaxBlockSize && (meta.resource_size != block_id * BlockData::MaxBlockSize + block->Size()) )
            return false;
    }
    else
    {
        // if the [block] is incomplete, it must be the last block of the resource and the size of the resource can be determined
        if (block->Size() < BlockData::MaxBlockSize)
        {
            if (j <= meta.bitmap.size())
                return false;

            meta.resource_size = block_id * BlockData::MaxBlockSize + block->Size();
        }
    }

    offset = RESERVED_HEADER_SIZE + meta.resource_pos_index * resource_block_size_ + block_id * BlockData::MaxBlockSize;

    if (set_bitmap)
    {
        if (meta.bitmap.size() < j)
            meta.bitmap.resize(j);

        while (i < j)
        {
            meta.bitmap.set(i++);
        }
    }

    return true;
}

bool BigFileLocalDisk::ResetMetaInfo()
{
    resource_meta_info_table_.clear();

    if (meta_info_loaded_)
    {
        // can't do this in midway, this is an [init or reset] operation, and only can be done at the start
        assert(false);
        return false;
    }

    if (BlockData::MaxBlockSize % DEFAULT_RESOURCE_SUB_BLOCK_SIZE)
    {
        assert(false);
        return false;
    }

    BigFileHeader header;
    memset(&header, 0, sizeof(header));

    strcpy(header.version, FILE_FORMAT_VERSION);
    header.GenerateMetaInfoTableName();

    if (header.meta_info_table_name[sizeof(header.meta_info_table_name) - 1])
        resource_meta_info_table_name_.assign(header.meta_info_table_name, sizeof(header.meta_info_table_name));
    else
        resource_meta_info_table_name_.assign(header.meta_info_table_name);

    if (!pManager_->GetResourceMetaInfoDB()->CreateTable(resource_meta_info_table_name_))
    {
        LOG4CPLUS_ERROR(Loggers::LocalDisk(), path_name_ << " - DB op failure: can't create resource meta info table");
        return false;
    }

    resource_block_size_ = DEFAULT_RESOURCE_BLOCK_SIZE;
    resource_sub_block_size_ = DEFAULT_RESOURCE_SUB_BLOCK_SIZE;

    sprintf(header.resource_block_size, "%d", resource_block_size_);
    sprintf(header.resource_sub_block_size, "%d", resource_sub_block_size_);

    header.SetMD5();

    int fd = open(path_name_.c_str(), O_WRONLY | O_CREAT | O_SYNC, 00666);

    if (fd < 0)
        return false;

    static char padding[RESERVED_HEADER_SIZE - sizeof(header)];

    bool ok = write(fd, &header, sizeof(header)) == sizeof(header) && write(fd, padding, sizeof(padding)) == sizeof(padding);

    close(fd);

    LOG4CPLUS_INFO(Loggers::LocalDisk(), path_name_ << ": meta info is reseted, the meta info table name is: " << resource_meta_info_table_name_);

    return ok;
}

bool BigFileLocalDisk::IsComponentHealthy() const
{
#if 0
    if (monitor_)
    {
        const size_t MaxConsecutiveDiskWriteFailuresAllowed = 10;
        const size_t MaxConsecutiveDiskReadFailuresAllowed = 20;

        if (monitor_->disk_read_consecutive_failures_ && 
            monitor_->disk_read_consecutive_failures_->GetStatus() > MaxConsecutiveDiskReadFailuresAllowed)
        {
            OperationsLogging::Log(OperationsLogEvents::WarningTooManyConsecutiveDiskReadFailures, path_name_, Warning);
            return false;
        }
        
        if (monitor_->disk_write_consecutive_failures_ &&
            monitor_->disk_write_consecutive_failures_->GetStatus() > MaxConsecutiveDiskWriteFailuresAllowed)
        {
            OperationsLogging::Log(OperationsLogEvents::WarningTooManyConsecutiveDiskWriteFailures, path_name_, Warning);
            return false;
        }
    }
#endif

    return true;
}

}


namespace super_node
{

BigFileLocalDisk::Monitor::Monitor(boost::shared_ptr<BigFileLocalDisk> owner, const std::string& path)
    : path_(path)
{
    boost::shared_ptr<ServiceComponent> service_component = ServiceHealthMonitor::Instance()->RegisterComponent(ServiceComponents::LocalDiskPrefix + path);
    service_component->SetHealthPredicate(owner);

    ticks_since_last_report_ = service_component->RegisterStatusItem("Ticks Since Last Report", "");

    sti_read_submitted_ = service_component->RegisterStatusItem("Disk Read - Submitted", "");
    sti_read_finished_ = service_component->RegisterStatusItem("Disk Read - Finished", "");
    sti_read_dropped_ = service_component->RegisterStatusItem("Disk Read - Dropped", "");
    sti_read_failure_ = service_component->RegisterStatusItem("Disk Read - Failure", "");
    sti_read_speed_ = service_component->RegisterStatusItem("Disk Read - Average Speed", "MB/s");

    sti_write_submitted_ = service_component->RegisterStatusItem("Disk Write - Submitted", "");
    sti_write_finished_ = service_component->RegisterStatusItem("Disk Write - Finished", "");
    sti_write_dropped_ = service_component->RegisterStatusItem("Disk Write - Dropped", "");
    sti_write_failure_ = service_component->RegisterStatusItem("Disk Write - Failure", "");
    sti_write_speed_ = service_component->RegisterStatusItem("Disk Write - Average Speed", "MB/s");

    sti_average_pending_callback_count_= service_component->RegisterStatusItem("Average Pending Callback Count", "");

    sti_res_total_ = service_component->RegisterStatusItem("Resource Block - Total", "K");
    sti_res_free_ = service_component->RegisterStatusItem("Resource Block - Free", "K");
    sti_res_free_percent_ = service_component->RegisterStatusItem("Resource Block - Free Percent", "%");
}

void BigFileLocalDisk::Monitor::ReportStatus(size_t res_total, size_t res_free)
{
    size_t ms_elapse = time_counter_.elapse();
    size_t s_elapse = (ms_elapse + 500) / 1000;
    time_counter_.reset();

    if (s_elapse == 0)
        s_elapse = 1;

    ticks_since_last_report_->SetStatus(ms_elapse);

    sti_read_submitted_->SetStatus(read_submitted_.Sum());
    sti_read_finished_->SetStatus(read_finished_.Sum());
    sti_read_speed_->SetStatus(read_finished_in_bytes_.Sum() / (1 << 20) / s_elapse);
    sti_read_dropped_->SetStatus(read_dropped_.Sum());
    sti_read_failure_->SetStatus(read_failure_.Sum());
                                                                                                                     
    sti_write_submitted_->SetStatus(write_submitted_.Sum());
    sti_write_finished_->SetStatus(write_finished_.Sum());
    sti_write_speed_->SetStatus(write_finished_in_bytes_.Sum() / (1 << 20) / s_elapse);
    sti_write_dropped_->SetStatus(write_dropped_.Sum());
    sti_write_failure_->SetStatus(write_failure_.Sum());

    sti_average_pending_callback_count_->SetStatus(average_pending_callback_count_.Average());

    sti_res_total_->SetStatus(res_total / 1000);
    sti_res_free_->SetStatus(res_free / 1000);
    sti_res_free_percent_->SetStatus( (size_t)(res_free * 100ULL / res_total) );

    read_submitted_.Reset();
    read_finished_.Reset();
    read_finished_in_bytes_.Reset();
    read_dropped_.Reset();
    read_failure_.Reset();

    write_submitted_.Reset();
    write_finished_.Reset();
    write_finished_in_bytes_.Reset();
    write_dropped_.Reset();
    write_failure_.Reset();
    average_pending_callback_count_.Reset();
}

BigFileLocalDisk::Monitor::~Monitor()
{
    ServiceHealthMonitor::Instance()->UnregisterComponent(ServiceComponents::LocalDiskPrefix + path_);
}


}
