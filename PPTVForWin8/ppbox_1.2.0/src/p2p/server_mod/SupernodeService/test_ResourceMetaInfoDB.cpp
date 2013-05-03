// #include "Common.h"
#include "LocalDiskCache/BigFileMode/ResourceMetaInfoDB.h"
#include <uuid/uuid.h>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace super_node;

#define LOG_ERROR  std::cout << "--->error: line[" << __LINE__ << "], "

namespace
{

typedef ResourceMetaInfoDB::TableItem TableItem;
typedef std::map<std::string, TableItem> MetaTable;

const std::string db_file = "resource-metainfo.db";

inline bool operator == (const TableItem& a, const TableItem& b)
{
    if (a.bitmap != b.bitmap)
        std::cout << "bitmap mismatch" << std::endl;

    if (a.resource_id != b.resource_id)
        std::cout << "resource_id mismatch" << std::endl;

    if (a.resource_size != b.resource_size)
        std::cout << "resource_size mismatch" << std::endl;

    if (a.resource_pos_index != b.resource_pos_index)
        std::cout << "resource_pos_index mismatch " << a.resource_pos_index << ", " << b.resource_pos_index << std::endl;

    return a.resource_id == b.resource_id && a.bitmap == b.bitmap && a.resource_size == b.resource_size && a.resource_pos_index == b.resource_pos_index;
}

inline bool operator != (const TableItem& a, const TableItem& b)
{
    return !(a == b);
}

std::string generate_uuid()
{
    uuid_t  uid;
    uuid_generate(uid);

    char buf[128] = { 0 };

    for (size_t i = 0; i < sizeof(uuid_t); ++i)
    {
        sprintf(buf + 2 * i, "%02x", uid[i]);
    }
    buf[0] = 'U';

    return std::string(buf);
}

struct Command : public ResourceMetaInfoDB::Operation
{
    std::string table;

    Command(Mode _mode, const std::string& _table, const std::string& _rid)
    {
        mode = _mode;
        table = _table;
        item.resource_id = _rid;

        if (mode == INSERT || mode == UPDATE)
        {
            item.resource_size = rand() % (20 << 20);
            item.resource_pos_index = (1LL << 35) * (rand() % (20 << 20));

            static std::vector<boost::dynamic_bitset<boost::uint8_t> >  rand_bitmaps;

            if (rand_bitmaps.empty())
            {
                for (size_t i = 0; i < 1024; ++i)
                {
                    boost::dynamic_bitset<boost::uint8_t> bitmap;
                    bitmap.resize(rand() % 256);

                    for (size_t j = 0; j < bitmap.size(); ++j)
                        bitmap[j] = (rand() & 1);

                    if (!bitmap.empty())
                        bitmap[bitmap.size() - 1] = true;  // the ResourceMetaInfoDB trim the end 0s

                    rand_bitmaps.push_back(bitmap);
                }
            }

            item.bitmap = rand_bitmaps[rand() % rand_bitmaps.size()];
        }
    }

    bool Execute(boost::shared_ptr<ResourceMetaInfoDB> db)
    {
        switch (mode)
        {
        case INSERT:
            return db->Insert(table, item);
        case UPDATE:
            return db->Update(table, item);
        case DELETE:
            return db->Delete(table, item.resource_id);

        default:
            assert(false);
        }

        return false;
    }
};

bool test_ResourceMetaInfoDB(size_t open_cnt, size_t table_cnt, size_t item_cnt, size_t cmd_cnt)
{
    std::map<std::string, boost::shared_ptr<MetaTable> >  table_map;
    typedef std::map<std::string, boost::shared_ptr<MetaTable> >::iterator table_iterator;
    typedef std::map<std::string, TableItem>::iterator table_item_iterator;

    srand(time(0));

    for (size_t pass = 0; pass < open_cnt; ++pass)
    {
        std::cout << "\n--------->pass: " << pass << std::endl << std::endl;

        boost::shared_ptr<ResourceMetaInfoDB> meta_db(new ResourceMetaInfoDB());

        std::cout << "------>open the DB" << std::endl;
        if (!meta_db->Start(db_file))
        {
            LOG_ERROR << " fail to open DB file: " << db_file << std::endl;
            return false;
        }
        std::cout << "--->ok\n";

#define ERROR_RETURN if (meta_db) meta_db->Stop(); return false

        std::cout << "------>load each table in and check it against the copy in the memory" << std::endl;

        for (table_iterator t_it = table_map.begin(); t_it != table_map.end(); ++t_it)
        {
            std::vector<TableItem> items;

            if (!meta_db->LoadTable(t_it->first, items))
            {
                LOG_ERROR << "fail to load table" << std::endl;
                ERROR_RETURN;
            }

            MetaTable& table = *(t_it->second);

            if (table.size() != items.size())
            {
                LOG_ERROR << "the data get from LoadTable(...) is wrong!" << std::endl;
                ERROR_RETURN;
            }

            for (std::vector<TableItem>::iterator p = items.begin(); p != items.end(); ++p)
            {
                table_item_iterator m_it = table.find(p->resource_id);

                if (m_it == table.end())
                {
                    LOG_ERROR << "the data get from LoadTable(...) is wrong!" << std::endl;
                    ERROR_RETURN;
                }

                if (m_it->second != *p)
                {
                    LOG_ERROR << "the data get from LoadTable(...) is wrong!" << std::endl;
                    ERROR_RETURN;
                }
            }
        }
        std::cout << "--->ok\n";

        std::cout << "++++++>create " << table_cnt << " new table, each with " << item_cnt << " items in it" << std::endl;
        for (size_t i = 0; i < table_cnt; ++i)
        {
            std::string table_name = generate_uuid();
            table_map[table_name] = boost::shared_ptr<MetaTable>(new MetaTable);

            std::cout << "------>create table: " << table_name << std::endl;
            if (!meta_db->CreateTable(table_name))
            {
                LOG_ERROR << "fail to create table" << std::endl;
                ERROR_RETURN;
            }
            std::cout << "--->ok\n";

            MetaTable& table = *(table_map[table_name]);

            std::cout << "------>insert " << item_cnt << " items" << std::endl;
            for (size_t j = 0; j < item_cnt; ++j)
            {
                std::string rid = generate_uuid();
                Command cmd(Command::INSERT, table_name, rid);

                if (!cmd.Execute(meta_db))
                {
                    LOG_ERROR << "fail to insert" << std::endl;
                    ERROR_RETURN;
                }

                table[rid] = cmd.item;

                if (j > 0 && j % 200 == 0)
                    std::cout << j << std::endl;
            }
            std::cout << "--->items inserted" << std::endl;
        }
        std::cout << "+++>all new table created\n";


        std::cout << "------>generate " << cmd_cnt << " commands for each table:  insert 10%; update 70%; delete 20%" << std::endl;
        std::vector<Command> commands;
        commands.reserve(table_map.size() * cmd_cnt);

        for (table_iterator t_it = table_map.begin(); t_it != table_map.end(); ++t_it)
        {
            MetaTable& table = *(t_it->second);

            if (!table.empty())
            {
                std::vector<std::string> rid_vec;
                rid_vec.reserve(table.size());

                for (table_item_iterator p = table.begin(); p != table.end(); ++p)
                    rid_vec.push_back(p->first);

                for (size_t cnt = cmd_cnt * 10 / 100; cnt; --cnt)
                    commands.push_back(Command(Command::INSERT, t_it->first, rid_vec[rand() % rid_vec.size()]));

                for (size_t cnt = cmd_cnt * 70 / 100; cnt; --cnt)
                    commands.push_back(Command(Command::UPDATE, t_it->first, rid_vec[rand() % rid_vec.size()]));

                for (size_t cnt = cmd_cnt * 20 / 100; cnt; --cnt)
                    commands.push_back(Command(Command::DELETE, t_it->first, rid_vec[rand() % rid_vec.size()]));
            }
        }

        std::cout << "------>randomize the order of the commands" << std::endl;
        std::random_shuffle(commands.begin(), commands.end());

        std::cout << "------>do the commands ......" << std::endl;
        for (std::vector<Command>::iterator p = commands.begin(); p != commands.end(); ++p)
        {
            table_iterator t_it = table_map.find(p->table);
            assert(t_it != table_map.end());

            MetaTable& table = *(t_it->second);

            switch (p->mode)
            {
            case Command::INSERT:
                if (table.find(p->item.resource_id) == table.end())
                {
                    if (!p->Execute(meta_db))
                    {
                        LOG_ERROR << "fail to insert" << std::endl;
                        ERROR_RETURN;
                    }

                    table[p->item.resource_id] = p->item;
                }
                else
                {
                    if (p->Execute(meta_db))
                    {
                        LOG_ERROR << "insert should fail, but pass" << std::endl;
                        ERROR_RETURN;
                    }
                }
                break;

            case Command::UPDATE:
                if (!p->Execute(meta_db))
                {
                    LOG_ERROR << "fail to update" << std::endl;
                    ERROR_RETURN;
                }
                if (table.find(p->item.resource_id) != table.end())
                    table[p->item.resource_id] = p->item;
                break;

            case Command::DELETE:
                if (!p->Execute(meta_db))
                {
                    LOG_ERROR << "fail to delete" << std::endl;
                    ERROR_RETURN;
                }
                table.erase(p->item.resource_id);
                break;
            }

            if (p != commands.begin() && (p - commands.begin()) % 500 == 0)
                std::cout << (p - commands.begin()) << std::endl;
        }
        std::cout << "--->all commands done" << std::endl;

        std::cout << "------>close the DB\n";
        meta_db->Stop();
        meta_db.reset();
    }

    return true;
}

}


void test_ResourceMetaInfoDB(int argc, char* argv[])
{
    if (argc != 5)
    {
        std::cout << "usage: " << argv[0] << " open-cnt  table-to-create-on-each-open  item-to-insert-in-new-table  cmd-cnt" << std::endl;
        return;
    }

    std::cout << "---------------->test the resource meta info DB\n";

    bool ret = test_ResourceMetaInfoDB(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));

    std::cout << "\n--->result: " << (ret ?  "pass" : "fail") << std::endl;
}

static void opera_callback(boost::mutex* pMutex, bool ok)
{
    if (!ok)
    {
        std::cout << "--->fail!" << std::endl;
        _exit(1);
    }
    pMutex->unlock();
}

void test_performance_ResourceMetaInfoDB(int argc, char* argv[])
{
    if (argc != 3)
    {
usage:
        std::cout << "usage: " << argv[0] << " cmd  cmd-cnt" << std::endl;
        return;
    }

    std::string cmd = argv[1];
    size_t cmd_cnt = atoi(argv[2]);

    static std::string TEST_TABLE_NAME = "test_table";

    if (cmd != "insert" && cmd != "update" && cmd != "delete")
        goto usage;

    boost::shared_ptr<ResourceMetaInfoDB> meta_db(new ResourceMetaInfoDB());

    std::cout << "------>open the DB" << std::endl;
    if (!meta_db->Start(db_file))
    {
        LOG_ERROR << " fail to open DB file: " << db_file << std::endl;
        return;
    }
    std::cout << "--->ok\n";
    
    std::cout << "------>load in the test table" << std::endl;
    std::vector<TableItem> items;
    std::vector<std::string> rid_vec;
    if (!meta_db->LoadTable(TEST_TABLE_NAME, items))
    {
        std::cout << "--->fail to load table; create it" << std::endl;
        if (!meta_db->CreateTable(TEST_TABLE_NAME))
        {
            LOG_ERROR << "fail to create table" << std::endl;
            return;
        }
    }

    rid_vec.reserve(items.size());
    for (std::vector<TableItem>::iterator p = items.begin(); p != items.end(); ++p)
    {
        rid_vec.push_back(p->resource_id);
    }

    std::cout << "--->origin table loaded in, items: " << items.size() << std::endl;

    typedef ResourceMetaInfoDB::Operation  Operation;
    boost::shared_ptr< std::vector<Operation> >  pOperas(new std::vector<Operation>());

    std::cout << "--->generate commands ..." << std::endl;

    if (cmd == "insert")
    {
        for (size_t i = 0; i < cmd_cnt; ++i)
        {
            pOperas->push_back(*(Operation*)(&Command(Command::INSERT, TEST_TABLE_NAME, generate_uuid())));
        }
    }
    else
    {
        std::random_shuffle(rid_vec.begin(), rid_vec.end());

        Command::Mode cmd_mode = Command::UPDATE;
        if (cmd == "delete")
            cmd_mode = Command::DELETE;

        size_t cmd_todo = std::min(rid_vec.size(), cmd_cnt);

        for (size_t i = 0; i < cmd_todo; ++i)
        {
            pOperas->push_back( *(Operation*)(&Command(cmd_mode, TEST_TABLE_NAME, rid_vec[i])) );
        }
    }

    std::cout << "--->do commands: " << pOperas->size() << std::endl;

    boost::mutex  mutex;

    mutex.lock();
    meta_db->PostOperations(TEST_TABLE_NAME, pOperas, boost::bind(opera_callback, &mutex, _1) );
    mutex.lock();
    mutex.unlock();

    meta_db->Stop();

    std::cout << "--->complete" << std::endl;
}
