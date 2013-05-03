#ifndef SUPER_NODE_RESOURCE_META_INFO_DB_H
#define SUPER_NODE_RESOURCE_META_INFO_DB_H

#include "../../Common.h"

struct sqlite3;
struct sqlite3_stmt;

namespace super_node
{

class AsioServiceRunner;

/**
 * run sqlite3 in the background thread only: early version of sqlite3 don't support multi-threads, and newer version has limitations,
 * so it's better to avoid multi-threads
 */
class ResourceMetaInfoDB : public boost::enable_shared_from_this<ResourceMetaInfoDB>
{
public:
    ResourceMetaInfoDB();
    ~ResourceMetaInfoDB();

    struct TableItem
    {
        std::string resource_id;
        boost::dynamic_bitset<boost::uint8_t> bitmap;
        size_t resource_size;
        boost::uint64_t resource_pos_index;
    };

    bool CreateTable(const std::string& table);

    bool LoadTable(const std::string& table, std::vector<TableItem>& items);

    bool Update(const std::string& table, const TableItem& item);
    bool Insert(const std::string& table, const TableItem& item);
    bool Delete(const std::string& table, const std::string& resource_id);

    bool Start(const std::string& db_file);
    void Stop();

    typedef boost::function<void(bool)> Callback;

    // all the callbacks are invoked in the background thread

    void PostUpdate(const std::string& table, const TableItem& item, Callback callback);
    void PostInsert(const std::string& table, const TableItem& item, Callback callback);
    void PostDelete(const std::string& table, const std::string& resource_id, Callback callback);

    struct Operation
    {
        enum Mode { INSERT, UPDATE, DELETE }  mode;
        TableItem  item;
        bool result;

        Operation() {}
        Operation(const std::string& resource_id) { mode = DELETE; item.resource_id = resource_id; }

        Operation(bool do_insert, const std::string& resource_id, const boost::dynamic_bitset<boost::uint8_t>& bitmap, size_t resource_size, boost::uint64_t resource_pos_index)
        {
            mode = do_insert ?  INSERT : UPDATE;
            item.resource_id = resource_id;
            item.bitmap = bitmap;
            item.resource_size = resource_size;
            item.resource_pos_index = resource_pos_index;
        }
    };

    void PostOperations(const std::string& table, boost::shared_ptr< std::vector<Operation> >  pOperations, Callback callback);

protected:
    // db routines
    void DoLoadTable(const std::string& table, std::vector<TableItem>* pItems, Callback callback);

    void DoCreateTable(const std::string& table, Callback callback);

    void DoUpdate(const std::string& table, const TableItem& item, Callback callback);
    void DoInsert(const std::string& table, const TableItem& item, Callback callback);
    void DoDelete(const std::string& table, const std::string& resource_id, Callback callback);

    void DoOperations(const std::string& table, boost::shared_ptr< std::vector<Operation> >  pOperations, Callback callback);

    void OpenDB(const std::string& db_name, Callback callback);
    void CloseDB(Callback callback);

    bool PostAndWait(boost::function< void (Callback) >  handler);

protected:
    boost::shared_ptr<AsioServiceRunner>  db_runner_;
    boost::shared_ptr<boost::asio::io_service>  io_service_;
    sqlite3* volatile pDB_;
};

}

#endif
