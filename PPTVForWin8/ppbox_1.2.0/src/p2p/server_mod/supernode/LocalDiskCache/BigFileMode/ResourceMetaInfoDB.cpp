#include "ResourceMetaInfoDB.h"
#include "../../AsioServiceRunner.h"
#include "sqlite3.h"


namespace super_node
{

ResourceMetaInfoDB::ResourceMetaInfoDB()
    : pDB_(0)
{ }

ResourceMetaInfoDB::~ResourceMetaInfoDB()
{
    assert(!db_runner_);
    Stop();
}

static void SetResult(boost::mutex* pMutex, volatile bool* pRet, bool ok)
{
    *pRet = ok;
    pMutex->unlock();
}

bool ResourceMetaInfoDB::PostAndWait(boost::function< void (Callback) >  handler)
{
    assert(db_runner_);
    boost::mutex mutex;
    volatile bool ok = false;

    mutex.lock();
    Callback callback = boost::bind(SetResult, &mutex, &ok, _1);
    io_service_->post( boost::bind(handler, callback) );
    mutex.lock(); // will block until the handler posted above is handled
    mutex.unlock();

    return ok;
}

bool ResourceMetaInfoDB::Start(const std::string& db_file)
{
    Stop();

    assert(!db_runner_ && !io_service_ && !pDB_);

    db_runner_.reset( new AsioServiceRunner(std::string("DB-Runner: ") + db_file) );
    io_service_ = db_runner_->Start();

    PostAndWait( boost::bind(&ResourceMetaInfoDB::OpenDB, shared_from_this(), db_file, _1) );

    if (pDB_)
        return true;

    io_service_.reset();
    db_runner_->Stop();
    db_runner_.reset();

    return false;
}

void ResourceMetaInfoDB::Stop()
{
    if (db_runner_)
    {
        assert(io_service_ && pDB_);

        PostAndWait( boost::bind(&ResourceMetaInfoDB::CloseDB, shared_from_this(), _1) );
        io_service_.reset();

        db_runner_->Stop();
        db_runner_.reset();

        assert(!pDB_);
    }
}

bool ResourceMetaInfoDB::LoadTable(const std::string& table, std::vector<TableItem>& items)
{
    return PostAndWait( boost::bind(&ResourceMetaInfoDB::DoLoadTable, shared_from_this(), table, &items, _1) );
}

bool ResourceMetaInfoDB::CreateTable(const std::string& table)
{
    return PostAndWait( boost::bind(&ResourceMetaInfoDB::DoCreateTable, shared_from_this(), table, _1) );
}

bool ResourceMetaInfoDB::Update(const std::string& table, const TableItem& item)
{
    return PostAndWait( boost::bind(&ResourceMetaInfoDB::DoUpdate, shared_from_this(), table, item, _1) );
}

bool ResourceMetaInfoDB::Insert(const std::string& table, const TableItem& item)
{
    return PostAndWait( boost::bind(&ResourceMetaInfoDB::DoInsert, shared_from_this(), table, item, _1) );
}

bool ResourceMetaInfoDB::Delete(const std::string& table, const std::string& resource_id)
{
    return PostAndWait( boost::bind(&ResourceMetaInfoDB::DoDelete, shared_from_this(), table, resource_id, _1) );
}

void ResourceMetaInfoDB::PostUpdate(const std::string& table, const TableItem& item, Callback callback)
{
    assert(db_runner_);
    io_service_->post( boost::bind(&ResourceMetaInfoDB::DoUpdate, shared_from_this(), table, item, callback) );
}

void ResourceMetaInfoDB::PostInsert(const std::string& table, const TableItem& item, Callback callback)
{
    assert(db_runner_);
    io_service_->post( boost::bind(&ResourceMetaInfoDB::DoInsert, shared_from_this(), table, item, callback) );
}

void ResourceMetaInfoDB::PostDelete(const std::string& table, const std::string& resource_id, Callback callback)
{
    assert(db_runner_);
    io_service_->post( boost::bind(&ResourceMetaInfoDB::DoDelete, shared_from_this(), table, resource_id, callback) );
}


void ResourceMetaInfoDB::PostOperations(const std::string& table, boost::shared_ptr< std::vector<Operation> >  pOperations, Callback callback)
{
    assert(db_runner_);
    io_service_->post( boost::bind(&ResourceMetaInfoDB::DoOperations, shared_from_this(), table, pOperations, callback) );
}

void ResourceMetaInfoDB::OpenDB(const std::string& db_file, Callback callback)
{
    assert(!pDB_);

    sqlite3* pDB = 0; // non volatile sqlite3* is needed in sqlite3_open(...)

    if (sqlite3_open(db_file.c_str(), &pDB) != SQLITE_OK && pDB)
    {
        sqlite3_close(pDB);
        pDB = 0;
    }
    pDB_ = pDB;

    if (pDB_)
    {
        if (sqlite3_exec(pDB_, "pragma synchronous = full", 0, 0, 0) != SQLITE_OK)
        {
            assert(false);
            sqlite3_close(pDB_);
            pDB_ = 0;
        }
    }

    callback(!!pDB_);
}

void ResourceMetaInfoDB::CloseDB(Callback callback)
{
    assert(pDB_);

    sqlite3_close(pDB_);
    pDB_ = 0;

    callback(true);
}

static void set_bitmap_from_blob(boost::dynamic_bitset<boost::uint8_t>& bitmap, const void* __pBlob, size_t byte_cnt)
{
    bitmap.clear();
    bitmap.resize(byte_cnt * 8);

    const unsigned char* pBlob = (const unsigned char*)__pBlob;
    size_t i = 0;

    for (size_t b = 0; b < byte_cnt; ++b)
    {
        for (size_t j = 0; j < 8; ++j, ++i)
        {
            if (pBlob[b] & (1 << j))
                bitmap.set(i);
        }
    }

    while (bitmap.size() && !bitmap.test(bitmap.size() - 1))
        bitmap.resize(bitmap.size() - 1);
}

void ResourceMetaInfoDB::DoLoadTable(const std::string& table, std::vector<TableItem>* pItems, Callback callback)
{
    assert(pDB_);

    sqlite3_stmt* pStmt_select;
    std::string sql = std::string("select * from ") + table;
    std::vector<TableItem>& items = *pItems;

    if (sqlite3_prepare(pDB_, sql.c_str(), -1, &pStmt_select, 0) != SQLITE_OK)
        goto error;

    for ( ;; )
    {
        switch (sqlite3_step(pStmt_select))
        {
        case SQLITE_DONE:
            goto finish;

        case SQLITE_ROW:
            if (sqlite3_column_count(pStmt_select) != 4)
            {
                assert(false);
                goto error;
            }
            items.push_back(TableItem());
            items.back().resource_id.assign((const char*)sqlite3_column_blob(pStmt_select, 0), sqlite3_column_bytes(pStmt_select, 0));

            set_bitmap_from_blob(items.back().bitmap, sqlite3_column_blob(pStmt_select, 1), sqlite3_column_bytes(pStmt_select, 1));
            items.back().resource_size = sqlite3_column_int(pStmt_select, 2);
            items.back().resource_pos_index = sqlite3_column_int64(pStmt_select, 3);

            break;

        default: // unexpected error occurs
            goto error;
        }
    }

finish:
    sqlite3_finalize(pStmt_select);
    callback(true);

    return;

error:
    items.clear();

    if (pStmt_select)
        sqlite3_finalize(pStmt_select);

    callback(false);
}

void ResourceMetaInfoDB::DoCreateTable(const std::string& table, Callback callback)
{
    assert(pDB_);

    std::string sql = std::string("create table if not exists ") + table +  "(resource_id blob unique, bitmap blob, resource_size integer, resource_pos_index integer)";

    int err = sqlite3_exec(pDB_, sql.c_str(), 0, 0, 0);

    bool ok = (err == SQLITE_OK);

    callback(ok);
}


static int bind_bitmap_blob(sqlite3_stmt* pStmt, int n, const boost::dynamic_bitset<boost::uint8_t>& bitmap)
{
    size_t bit_cnt = bitmap.size(), byte_cnt = (bit_cnt + 7) / 8;
    if (!byte_cnt)
        byte_cnt = 1;

    unsigned char *pBlob = (unsigned char*)malloc(byte_cnt);

    if (!pBlob)
        return SQLITE_NOMEM;

    memset(pBlob, 0, byte_cnt);

    for (size_t b = 0, j = 0, i = 0; i < bit_cnt; ++i)
    {
        if (bitmap[i])
            pBlob[b] |= 1 << j;

        if (++j == 8)
        {
            j = 0;
            ++b;
        }
    }

    return sqlite3_bind_blob(pStmt, n, pBlob, byte_cnt, free);
}

void ResourceMetaInfoDB::DoUpdate(const std::string& table, const TableItem& item, Callback callback)
{
    assert(pDB_);

    char sql[1024];
    snprintf(sql, sizeof(sql), "update %s set bitmap = ?1, resource_size = %lu, resource_pos_index = %llu where resource_id = ?2",
            table.c_str(), item.resource_size, item.resource_pos_index);

    sqlite3_stmt* pStmt = 0; 

    bool ok = sqlite3_prepare(pDB_, sql, -1, &pStmt, 0) == SQLITE_OK
            && bind_bitmap_blob(pStmt, 1, item.bitmap) == SQLITE_OK
            && sqlite3_bind_blob(pStmt, 2, (unsigned char*)item.resource_id.c_str(), item.resource_id.size(), SQLITE_STATIC) == SQLITE_OK
            && sqlite3_step(pStmt) == SQLITE_DONE;

    if (pStmt)
        sqlite3_finalize(pStmt);

    callback(ok);
}

void ResourceMetaInfoDB::DoInsert(const std::string& table, const TableItem& item, Callback callback)
{
    assert(pDB_);

    char sql[1024];
    snprintf(sql, sizeof(sql),  "insert into %s values(?1, ?2, %lu, %llu)", table.c_str(), item.resource_size, item.resource_pos_index);

    sqlite3_stmt* pStmt = 0; 

    bool ok = sqlite3_prepare(pDB_, sql, -1, &pStmt, 0) == SQLITE_OK
            && sqlite3_bind_blob(pStmt, 1, (unsigned char*)item.resource_id.c_str(), item.resource_id.size(), SQLITE_STATIC) == SQLITE_OK
            && bind_bitmap_blob(pStmt, 2, item.bitmap) == SQLITE_OK
            && sqlite3_step(pStmt) == SQLITE_DONE;

    if (pStmt)
        sqlite3_finalize(pStmt);

    callback(ok);
}

void ResourceMetaInfoDB::DoDelete(const std::string& table, const std::string& resource_id, Callback callback)
{
    assert(pDB_);

    char sql[1024];
    snprintf(sql, sizeof(sql),  "delete from %s where resource_id = ?1", table.c_str());

    sqlite3_stmt* pStmt = 0; 

    bool ok = sqlite3_prepare(pDB_, sql, -1, &pStmt, 0) == SQLITE_OK
            && sqlite3_bind_blob(pStmt, 1, (unsigned char*)resource_id.c_str(), resource_id.size(), SQLITE_STATIC) == SQLITE_OK
            && sqlite3_step(pStmt) == SQLITE_DONE;

    if (pStmt)
        sqlite3_finalize(pStmt);

    callback(ok);
}

static void do_opera_callback(volatile bool* pRet, bool ok)
{
    *pRet = ok;
}

void ResourceMetaInfoDB::DoOperations(const std::string& table, boost::shared_ptr< std::vector<Operation> >  pOperations, Callback callback)
{
    bool all_ok = true;

    assert(pOperations);
    std::vector<Operation>&  opera_vec = *pOperations;

    volatile bool ok;
    Callback  one_callback = boost::bind(do_opera_callback, &ok, _1);

    if (sqlite3_exec(pDB_, "begin transaction", 0, 0, 0) != SQLITE_OK)
        goto transaction_fail;

    for (std::vector<Operation>::iterator p = opera_vec.begin(); p != opera_vec.end(); ++p)
    {
        ok = false;

        switch (p->mode)
        {
        case Operation::INSERT:
            DoInsert(table, p->item, one_callback);
            break;
        case Operation::UPDATE:
            DoUpdate(table, p->item, one_callback);
            break;
        case Operation::DELETE:
            DoDelete(table, p->item.resource_id, one_callback);
            break;
        default:
            assert(false);
        }

        assert(ok);
        if (!ok)
            all_ok = false;

        p->result = ok;
    }

    if (sqlite3_exec(pDB_, "commit transaction", 0, 0, 0) != SQLITE_OK)
        goto transaction_fail;

    callback(all_ok);
    return;

transaction_fail:
    assert(false);

    for (std::vector<Operation>::iterator p = opera_vec.begin(); p != opera_vec.end(); ++p)
        p->result = false;

    callback(false);
}

}
