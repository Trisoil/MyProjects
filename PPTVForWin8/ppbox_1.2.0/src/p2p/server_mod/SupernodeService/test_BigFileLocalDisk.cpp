#include "Common.h"
#include "AsioServiceRunner.h"
#include "LocalDiskCache/BigFileMode/BigFileLocalDisk.h"
#include "LocalDiskCache/BigFileMode/BigFileLocalDiskManager.h"
#include "BlockDataAllocator.h"
#include <uuid/uuid.h>
#include <algorithm>


using namespace super_node;

#define ERROR_EXIT(__msg__)  do {\
        std::cout << "--->error: line[" << __LINE__ << "], big-file-disk[" << path_name_ << "], " << (__msg__) << std::endl; std::cout.flush(); _exit(1); \
    } while (0);

namespace
{

std::string generate_uuid()
{
    uuid_t  uid;
    uuid_generate(uid);

    char buf[128] = { 0 };

    for (size_t i = 0; i < sizeof(uuid_t); ++i)
    {
        sprintf(buf + 2 * i, "%02x", uid[i]);
    }

    return std::string(buf);
}

const size_t DATA_UNIT_SIZE = 512;

void set_up_block_data(const std::string& uuid, size_t block_id, BlockData& block)
{
    assert(uuid.size() == 32);

    char * buf = (char*)block.Buf();
    size_t buf_size = BlockData::MaxBlockSize;

    memset(buf, 0, buf_size);

    size_t sub_id = 0;

    while (buf_size >= DATA_UNIT_SIZE)
    {
        sprintf(buf, "%s, %u, %u", uuid.c_str(), block_id, sub_id++);

        buf += DATA_UNIT_SIZE;
        buf_size -= DATA_UNIT_SIZE;
    }
}

bool check_block_data(const std::string& uuid, size_t block_id, BlockData& block)
{
    assert(uuid.size() == 32);

    char* buf = (char*)block.Buf();
    size_t buf_size = block.Size();

    size_t sub_id = 0;

    while (buf_size)
    {
        char expected_data[DATA_UNIT_SIZE] = { 0 };
        sprintf(expected_data, "%s, %u, %u", uuid.c_str(), block_id, sub_id++);

        size_t len = std::min(DATA_UNIT_SIZE, buf_size);

        if (memcmp(buf, expected_data, len) != 0)
            return false;

        buf += len;
        buf_size -= len;
    }

    return true;
}



class ThreadPool : public boost::enable_shared_from_this<ThreadPool>
{
public:
    ThreadPool(size_t thread_cnt)
        : pending_count_(0)
    {
        assert(thread_cnt);
        runner_.reset(new AsioServiceRunner("threadpool"));
        io_service_ = runner_->Start(thread_cnt);
    }
    ~ThreadPool()
    {
        Wait();
        runner_->Stop();
    }

    void Post(boost::function<void()> job)
    {
        {
            boost::mutex::scoped_lock guard(mutex_);
            ++pending_count_;
        }
        io_service_->post(boost::bind(&ThreadPool::DoJob, shared_from_this(), job));
    }

    void Wait()
    {
        boost::mutex::scoped_lock  guard(mutex_);

        while (pending_count_)
            all_done_condition_.wait(guard);
    }

private:
    void DoJob(boost::function<void()> job)
    {
        job();

        boost::mutex::scoped_lock guard(mutex_);
        assert(pending_count_);

        if (--pending_count_ == 0)
            all_done_condition_.notify_all();
    }

private:
    boost::shared_ptr<AsioServiceRunner>  runner_;
    boost::shared_ptr<boost::asio::io_service>  io_service_;

    boost::mutex mutex_;
    boost::condition  all_done_condition_;
    size_t pending_count_;
};


class BigFileLocalDiskTester : public boost::enable_shared_from_this<BigFileLocalDiskTester>
{
public:
    BigFileLocalDiskTester(const std::string& path_name, size_t thread_cnt)
        : path_name_(path_name)
        , thread_pool_(thread_cnt)
        , read_cnt_(0)
        , normal_write_cnt_(0)
        , invalid_write_cnt_(0)
        , delete_cnt_(0)
    { }
    ~BigFileLocalDiskTester()
    {
        if (disk_)
            disk_->Stop();
    }

    void Start();

    bool AddReadCmd(const ResourceIdentifier& resource_identifier, int priority, bool try_lock_before_read);
    bool AddWriteCmd(const ResourceIdentifier& resource_identifier, size_t percent, bool allow_rewrite, bool try_lock_before_write, bool try_do_invalid_write);
    bool AddDeleteCmd(const std::vector<ResourceIdentifier>& resources);

    void FlushCmdBuffer(bool shuffle = true);

    void LoadResources(bool reset);
    void ReopenAndCheckResources();

    void CreateResource(size_t cnt);

    void GetResourceIdentifiers(std::vector<ResourceIdentifier>& res_ids);

    void Wait();

protected:
    void ReadCallback(bool locked_before_read, const ResourceIdentifier& resource_identifier, size_t block_id, ErrorCode err, boost::shared_ptr<BlockData> block);
    void WriteCallback(const ResourceIdentifier& resource_identifier, bool locked_before_write, bool expected_result, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks,
            ErrorCode err, const ResourceIdentifier&
    );
    void DeleteCallback(const std::vector<ResourceIdentifier>& resources_to_delete, const ErrorCode& err, const std::vector<ResourceIdentifier>& deleted_resources);

    void LoadCallback(bool reset, const ErrorCode& err, std::vector<boost::shared_ptr<DiskResource> >&  resources);

private:
    std::string path_name_;
    boost::shared_ptr<BigFileLocalDisk>  disk_;
    ThreadPool thread_pool_;

    std::vector< boost::function<bool()> > cmd_buffer_;

    struct ResourceCtrl
    {
        size_t  resource_size;
        boost::shared_ptr<DiskResource>  resource;

        // pending_io_count and pending_delete_count can't both > 0;
        // these are used to make sure a command's correct result can be dertermined despite the exact time when it's executed
        size_t pending_io_count;
        size_t pending_delete_count;

        bool resource_size_recorded_in_disk;

        ResourceCtrl(size_t _resource_size = 0) : resource_size(_resource_size), pending_io_count(0), pending_delete_count(0), resource_size_recorded_in_disk(false)
        { }
    };

    boost::mutex  mutex_;
    std::map<ResourceIdentifier,  ResourceCtrl>  resource_table_;
    typedef std::map<ResourceIdentifier,  ResourceCtrl>::iterator  resource_iterator;


    // statistics
    size_t read_cnt_;
    size_t normal_write_cnt_;
    size_t invalid_write_cnt_;
    size_t delete_cnt_;
    void ReportStatistics(bool force = false);
};


void BigFileLocalDiskTester::CreateResource(size_t cnt)
{
    while (cnt--)
    {
        ResourceIdentifier resource_identifier(generate_uuid());
        size_t resource_size;

        size_t seed =  rand() % 100;
        size_t size_bound = BigFileLocalDisk::DEFAULT_RESOURCE_BLOCK_SIZE;

        if (seed > 95)
            resource_size = size_bound;
        else if (seed > 50)
            resource_size = size_bound * 3 / 4 + size_bound / 4 * (rand() % 100 + 1) / 100;
        else if (seed > 20)
            resource_size = size_bound * (rand() % 100 + 1) / 100;
        else
            resource_size = BigFileLocalDisk::DEFAULT_RESOURCE_SUB_BLOCK_SIZE *
                ((BigFileLocalDisk::DEFAULT_RESOURCE_BLOCK_SIZE / BigFileLocalDisk::DEFAULT_RESOURCE_SUB_BLOCK_SIZE) * (rand() % 100 + 1) / 100);

        if (resource_size == 0)
            resource_size = rand() % size_bound + 1;

        assert(resource_size <= size_bound);

        boost::mutex::scoped_lock  guard(mutex_);
        assert(resource_table_.find(resource_identifier) == resource_table_.end());

        resource_table_[resource_identifier] = ResourceCtrl(resource_size);
    }
}

bool BigFileLocalDiskTester::AddReadCmd(const ResourceIdentifier& resource_identifier, int priority, bool try_lock_before_read)
{
    boost::mutex::scoped_lock guard(mutex_);

    resource_iterator it = resource_table_.find(resource_identifier);

    int block_index = 0;

    if (it != resource_table_.end())
    {
        int block_cnt = (it->second.resource_size + BlockData::MaxBlockSize - 1) / BlockData::MaxBlockSize;
        assert(block_cnt > 0);
        block_index = rand() % block_cnt;
    }

    bool locked_before_read = false;

    if (try_lock_before_read)
    {
        if (it != resource_table_.end() && it->second.pending_delete_count == 0 && it->second.resource && it->second.resource->HasBlock(block_index))
        {
            it->second.pending_io_count++;
            locked_before_read = true;
        }
    }

    ReadResourceBlockCallback callback = boost::bind(&BigFileLocalDiskTester::ReadCallback, shared_from_this(), locked_before_read, resource_identifier, block_index, _1, _2);

    cmd_buffer_.push_back( boost::bind(&BigFileLocalDisk::AsyncRead, disk_, resource_identifier, block_index, priority, callback));

    return true;
}


static boost::shared_ptr<BlockData> AllocBlockData(size_t size = BlockData::MaxBlockSize)
{
    char* buf = reinterpret_cast<char*>(BlockDataAllocator::Instance().Allocate());
    assert(buf && size <= BlockData::MaxBlockSize);

    return boost::shared_ptr<BlockData>(new BlockData(buf, size));
}

bool BigFileLocalDiskTester::AddWriteCmd(const ResourceIdentifier& resource_identifier, size_t percent, bool allow_rewrite, bool try_lock_before_write, bool try_do_invalid_write)
{
    if (!percent)
        percent = rand() % 100 + 1;

    boost::mutex::scoped_lock  guard(mutex_);
    resource_iterator  it = resource_table_.find(resource_identifier);

    if (it != resource_table_.end())
    {
        assert(it->second.resource_size);

        std::vector<size_t> block_ids;
        block_ids.resize((it->second.resource_size + BlockData::MaxBlockSize - 1) / BlockData::MaxBlockSize);

        for (size_t id = 0; id < block_ids.size(); ++id)
            block_ids[id] = id;

        std::random_shuffle(block_ids.begin(), block_ids.end());

        size_t block_cnt_towrite = std::max(block_ids.size() * percent / 100, 1ul);

        bool locked_before_write = try_lock_before_write && it->second.pending_delete_count == 0;

        if (locked_before_write)
            it->second.pending_io_count++;

        bool expected_result = true;
        std::map<size_t, boost::shared_ptr<BlockData> >  block_map;

        for (size_t i = 0; i < block_ids.size() && block_cnt_towrite; ++i)
        {
            size_t block_id = block_ids[i];

            if (allow_rewrite || !it->second.resource || !it->second.resource->HasBlock(block_id))
            {
                size_t block_size = (block_id == block_ids.size() - 1 ?  it->second.resource_size - block_id * BlockData::MaxBlockSize : BlockData::MaxBlockSize);

                if (expected_result && try_do_invalid_write)
                {
                    if (locked_before_write && it->second.resource_size_recorded_in_disk)
                    {
                        // do an invalid write
                        size_t invalid_size = rand() % BlockData::MaxBlockSize + 1;

                        if (block_size != invalid_size)
                        {
                            block_size = invalid_size;
                            expected_result = false;
                        }
                    }
                }

                block_map[block_id] = AllocBlockData(block_size);
                set_up_block_data(resource_identifier.GetResourceName(), block_id, *block_map[block_id]);
                block_cnt_towrite--;
            }
        }

        WriteResourceTaskCallback callback = boost::bind(&BigFileLocalDiskTester::WriteCallback, shared_from_this(),
                            resource_identifier, locked_before_write, expected_result, block_map, _1, _2);

        cmd_buffer_.push_back( boost::bind(&BigFileLocalDisk::AsyncWrite, disk_, resource_identifier, block_map, callback) );

        return true;
    }

    return false;
}

bool BigFileLocalDiskTester::AddDeleteCmd(const std::vector<ResourceIdentifier>& resources)
{
    std::vector<ResourceIdentifier> valid_resources;

    boost::mutex::scoped_lock  guard(mutex_);

    for (std::vector<ResourceIdentifier>::const_iterator p = resources.begin(); p != resources.end(); ++p)
    {
        resource_iterator it = resource_table_.find(*p);

        if (it != resource_table_.end() && it->second.pending_io_count == 0)
        {
            it->second.pending_delete_count++;
            valid_resources.push_back(*p);
        }
    }

    if (!valid_resources.empty())
    {
        DeleteResourcesTaskCallback callback = boost::bind(&BigFileLocalDiskTester::DeleteCallback, shared_from_this(), valid_resources, _1, _2);

        cmd_buffer_.push_back( boost::bind(&BigFileLocalDisk::AsyncDeleteResources, disk_, valid_resources, callback) );

        return true;
    }

    return false;
}

void BigFileLocalDiskTester::ReadCallback(bool locked_before_read, const ResourceIdentifier& resource_identifier, size_t block_id, ErrorCode err, boost::shared_ptr<BlockData> block)
{
    if (err == ErrorCodes::Success)
    {
        if (!block || !block->Size() || !check_block_data(resource_identifier.GetResourceName(), block_id, *block))
        {
            ERROR_EXIT("the data read in is invalid!");
        }
    }

    if (locked_before_read)  // the read should always succeed
    {
        boost::mutex::scoped_lock  guard(mutex_);

        resource_iterator  it = resource_table_.find(resource_identifier);
        assert(it != resource_table_.end());

        it->second.pending_io_count--;

        if (err != ErrorCodes::Success)
        {
            ERROR_EXIT("read failure");
        }

        // the data content has already been checked, now check it's size

        bool size_valid = block_id * BlockData::MaxBlockSize + block->Size() == it->second.resource_size ||
            ((block_id + 1) * BlockData::MaxBlockSize <= it->second.resource_size && block->Size() == BlockData::MaxBlockSize);

        if (!size_valid)
        {
            ERROR_EXIT("the size of the data read in is invalid");
        }
    }

    ++read_cnt_;
    ReportStatistics();
}

void BigFileLocalDiskTester::WriteCallback(
        const ResourceIdentifier& resource_identifier, bool locked_before_write, bool expected_result, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks,
        ErrorCode err, const ResourceIdentifier&)
{
    if (expected_result != (err == ErrorCodes::Success))
        ERROR_EXIT(expected_result ?  "write failure" : "should fail but succeed");

    if (err == ErrorCodes::Success && !blocks.empty())
    {
        boost::mutex::scoped_lock  guard(mutex_);

        resource_iterator  it = resource_table_.find(resource_identifier);
        assert(it != resource_table_.end());

        if (!it->second.resource)
        {
            BlocksMap bmap;
            it->second.resource.reset( new DiskResource(resource_identifier, false, bmap) );
        }

        for (std::map<size_t, boost::shared_ptr<BlockData> >::const_iterator p = blocks.begin(); p != blocks.end(); ++p)
        {
            it->second.resource->AddBlock(p->first);

            if (p->second->Size() != BlockData::MaxBlockSize)
                it->second.resource_size_recorded_in_disk = true;
        }

        if (locked_before_write)
        {
            assert(it->second.pending_io_count);
            it->second.pending_io_count--;
        }
    }

    if (expected_result)
        normal_write_cnt_++;
    else
        invalid_write_cnt_++;

    ReportStatistics();
}

void BigFileLocalDiskTester::DeleteCallback(const std::vector<ResourceIdentifier>& resources_to_delete, const ErrorCode& err, const std::vector<ResourceIdentifier>& deleted_resources)
{
    if (err != ErrorCodes::Success)
        ERROR_EXIT("delete failure");

    boost::mutex::scoped_lock  guard(mutex_);

    for (std::vector<ResourceIdentifier>::const_iterator p = resources_to_delete.begin(); p != resources_to_delete.end(); ++p)
    {
        resource_iterator it = resource_table_.find(*p);
        assert(it != resource_table_.end());

        assert(it->second.pending_io_count == 0 && it->second.pending_delete_count > 0);
        it->second.pending_delete_count--;
    }

    for (std::vector<ResourceIdentifier>::const_iterator p = deleted_resources.begin(); p != deleted_resources.end(); ++p)
    {
        resource_iterator it = resource_table_.find(*p);
        assert(it != resource_table_.end());

        it->second.resource.reset();
        it->second.resource_size_recorded_in_disk = false;
    }

    ++delete_cnt_;
    ReportStatistics();
}

void BigFileLocalDiskTester::ReportStatistics(bool force)
{
    size_t total_cnt = read_cnt_ + normal_write_cnt_ + invalid_write_cnt_ + delete_cnt_;

    if (force || (total_cnt % 100 == 0 && total_cnt))
        std::cout << path_name_ << " --->info: cmds finished(read: " << read_cnt_ << ", normal write: " << normal_write_cnt_ << ", invalid write: " << invalid_write_cnt_
                << ", delete: " << delete_cnt_
                << ")" << std::endl;
}

void BigFileLocalDiskTester::LoadResources(bool reset)
{
    FlushCmdBuffer();

    thread_pool_.Wait(); // all commands submitted to disk_

    disk_->Sync(); // disk_ finish all the IO operations

    disk_->AsyncLoadResources(boost::bind(&BigFileLocalDiskTester::LoadCallback, shared_from_this(), reset, _1, _2), false);
    disk_->Sync();
}

bool is_subset_of(const BlocksMap& a, const BlocksMap& b)
{
    if (a.size() == b.size())
        return a.is_subset_of(b);
    else if (a.size() < b.size())
    {
        BlocksMap c = a;
        c.resize(b.size());
        return c.is_subset_of(b);
    }
    else
    {
        BlocksMap c = b;
        c.resize(a.size());
        return a.is_subset_of(c);
    }
}

void BigFileLocalDiskTester::LoadCallback(bool reset, const ErrorCode& err, std::vector<boost::shared_ptr<DiskResource> >&  resources)
{
    if (err != ErrorCodes::Success)
        ERROR_EXIT("fail to load resources");

    if (reset)
    {
        resource_table_.clear();

        for (std::vector<boost::shared_ptr<DiskResource> >::const_iterator p = resources.begin(); p != resources.end(); ++p)
        {
            ResourceCtrl ctrl;

            ctrl.resource = *p;
            ctrl.resource_size_recorded_in_disk = (ctrl.resource->GetSize() != 0);

            ctrl.resource_size = (*p)->GetSize();

            if (!ctrl.resource_size)
            {
                const BlocksMap& bmap = ctrl.resource->GetBlocksBitmap();

                size_t j = bmap.size();

                while (j && !bmap[j - 1])
                    --j;

                if (j * BlockData::MaxBlockSize > BigFileLocalDisk::DEFAULT_RESOURCE_BLOCK_SIZE)
                {
                    ERROR_EXIT("load failure: invalid bitmap");
                }

                if (j * BlockData::MaxBlockSize == BigFileLocalDisk::DEFAULT_RESOURCE_BLOCK_SIZE)
                    ctrl.resource_size = BigFileLocalDisk::DEFAULT_RESOURCE_BLOCK_SIZE;
                else
                    ctrl.resource_size = j * BlockData::MaxBlockSize + rand() % (BigFileLocalDisk::DEFAULT_RESOURCE_BLOCK_SIZE - j * BlockData::MaxBlockSize + 1);
            }

            assert(ctrl.resource_size && ctrl.resource_size <= BigFileLocalDisk::DEFAULT_RESOURCE_BLOCK_SIZE);

            resource_table_[ctrl.resource->GetResourceIdentifier()] = ctrl;
        }
        std::cout << path_name_ << " --->info: resource loaded in: " << resource_table_.size() << std::endl;
    }
    else
    {
        /**
         * do check:
         * if disk_->AsyncWrite(...) failed, some data blocks may still have been writen in, but these are not recorded
         * in [this];
         * so [resource_table_] tracks only a subset of all the data in [disk_];
         */

        std::map<ResourceIdentifier, boost::shared_ptr<DiskResource> >  res_map;

        for (std::vector<boost::shared_ptr<DiskResource> >::const_iterator p = resources.begin(); p != resources.end(); ++p)
            res_map[(*p)->GetResourceIdentifier()] = *p;

        for (resource_iterator it = resource_table_.begin(); it != resource_table_.end(); ++it)
        {
            if (it->second.resource && it->second.resource->GetBlocksCount())
            {
                std::map<ResourceIdentifier, boost::shared_ptr<DiskResource> >::iterator p = res_map.find(it->first);

                if (p == res_map.end() ||
                    (p->second->GetSize() && p->second->GetSize() != it->second.resource_size) ||
                    !is_subset_of(it->second.resource->GetBlocksBitmap(), p->second->GetBlocksBitmap())
                   )
                {
                    ERROR_EXIT("data corruption detected: the data load from disk is incomplete!");
                }
            }
        }
    }
}

void BigFileLocalDiskTester::ReopenAndCheckResources()
{
    FlushCmdBuffer();

    std::cout << path_name_ << " --->info: wait all commands to finish ......" << std::endl;
    thread_pool_.Wait();

    disk_->Sync();
    ReportStatistics(true);
    std::cout << path_name_ << " --->info: all commands finished" << std::endl;

    disk_->Stop();
    disk_.reset(BigFileLocalDiskManager::Instance().CreateBigFileLocalDisk(path_name_, 128LL << 30)); // 128 G

    if (!disk_)
        ERROR_EXIT("fail to reopen big-file-disk");

    disk_->Start();

    std::cout << path_name_ << " --->info: reload and check" << std::endl;
    LoadResources(false);  // load in resources and check them against the memory copy
}

void BigFileLocalDiskTester::FlushCmdBuffer(bool shuffle)
{
    if (shuffle)
        std::random_shuffle(cmd_buffer_.begin(), cmd_buffer_.end());

    for (size_t i = 0; i < cmd_buffer_.size(); ++i)
        cmd_buffer_[i]();

    cmd_buffer_.clear();
}

void BigFileLocalDiskTester::GetResourceIdentifiers(std::vector<ResourceIdentifier>& res_ids)
{
    res_ids.clear();

    boost::mutex::scoped_lock  guard(mutex_);
    res_ids.reserve(resource_table_.size());

    for (resource_iterator it = resource_table_.begin(); it != resource_table_.end(); ++it)
    {
        res_ids.push_back(it->first);
    }
}

void BigFileLocalDiskTester::Start()
{
    disk_.reset(BigFileLocalDiskManager::Instance().CreateBigFileLocalDisk(path_name_, 128LL << 30)); // 128 G

    if (!disk_)
        ERROR_EXIT("fail to create big-file-disk");

    LoadResources(true);
}


void do_test(const std::string& path_name, size_t thread_cnt,
        size_t round_cnt, size_t create_per_round, size_t read_per_round, size_t write_per_round, size_t delete_per_round)
{
    boost::shared_ptr<BigFileLocalDiskTester>  pTester(new BigFileLocalDiskTester(path_name, thread_cnt));

    pTester->Start();

    while (round_cnt--)
    {
        pTester->CreateResource(create_per_round);

        std::vector<ResourceIdentifier> res_ids;
        pTester->GetResourceIdentifiers(res_ids);

        std::random_shuffle(res_ids.begin(), res_ids.end());
        size_t read_cmd_cnt = 0;
        for (size_t i = 0; i < read_per_round && i < res_ids.size(); ++i)
            if (pTester->AddReadCmd(res_ids[i], rand() % 50, rand() % 100 < 50))
                ++read_cmd_cnt;

        std::random_shuffle(res_ids.begin(), res_ids.end());
        size_t write_cmd_cnt = 0;
        for (size_t i = 0; i < res_ids.size() && i < write_per_round; ++i)
            if (pTester->AddWriteCmd(res_ids[i], rand() % 100 + 1, rand() % 100 < 20, rand() % 100 < 20, rand() % 100 < 90))
                ++write_cmd_cnt;

        std::random_shuffle(res_ids.begin(), res_ids.end());
        size_t delete_cmd_cnt = 0;
        for (size_t i = 0; i < delete_per_round; ++i)
        {
            size_t j = rand() % res_ids.size();
            size_t k = std::min(15ul, rand() % (res_ids.size() - j) + 1);

            std::vector<ResourceIdentifier> to_delete(res_ids.begin() + j, res_ids.begin() + j + k);

            if (pTester->AddDeleteCmd(to_delete))
                ++delete_cmd_cnt;
        }

        std::cout << path_name << " --->info: cmds added(read: " << read_cmd_cnt << ", write: " << write_cmd_cnt << ", delete: " << delete_cmd_cnt << ")" << std::endl;

        std::cout << path_name << " --->info: close & reopen this file, and check the content of it with the memory copy ..." << std::endl;
        pTester->ReopenAndCheckResources(); // will flush cmd buffer and wait
    }
}

}


static void test_BigFileLocalDisk(size_t disk_cnt, size_t thread_cnt,
        size_t round_cnt, size_t create_per_round, size_t read_per_round, size_t write_per_round, size_t delete_per_round)
{
    std::vector< boost::shared_ptr<boost::thread> >  threads;

    for (size_t d = 0; d < disk_cnt; ++d)
    {
        char path_name[64] = { 0 };
        sprintf(path_name, "big-file-disk[%d]", d);

        threads.push_back( boost::shared_ptr<boost::thread>(
                                new boost::thread(
                                    boost::bind(do_test, std::string(path_name), thread_cnt, round_cnt, create_per_round, read_per_round, write_per_round, delete_per_round)
                                )
                    )
            );
    }

    for (size_t i = 0; i < threads.size(); ++i)
        threads[i]->join();
}


void test_BigFileLocalDisk(int argc, char* argv[])
{
    size_t params[] ={ 1, 3, 10, 400, 800, 600, 80 };

    for (int i = 1; i < 8 && argv[i]; ++i)
    {
        int val = atoi(argv[i]);
        params[i - 1] = val;
    }


    std::cout << "---------------->test started with below params:" << std::endl;
    std::cout << ">disk_cnt: " << params[0] << std::endl;
    std::cout << ">thread_cnt: " << params[1] << std::endl;
    std::cout << ">round_cnt: " << params[2] << std::endl;
    std::cout << ">create_per_round: " << params[3] << std::endl;
    std::cout << ">read_per_round: " << params[4] << std::endl;
    std::cout << ">write_per_round: " << params[5] << std::endl;
    std::cout << ">delete_per_round: " << params[6] << std::endl;
    std::cout << std::endl;

    srand(time(0));

    if (!BigFileLocalDiskManager::Instance().Start("resource-metainfo.db", 2 * params[0]))
    {
        std::cout << "--->error: fail to start big-file-local-disk-manager" << std::endl;
        return;
    }

    BigFileLocalDiskManager::Instance().Config(1u << 30, 0, 1000);

    test_BigFileLocalDisk(params[0], params[1], params[2], params[3], params[4], params[5], params[6]);

    std::cout << "--->pass!" << std::endl;
}
