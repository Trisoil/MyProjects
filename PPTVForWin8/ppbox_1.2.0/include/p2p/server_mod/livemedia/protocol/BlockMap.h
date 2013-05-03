#ifndef _PROTOCOL_BLOCK_MAP_H_
#define _PROTOCOL_BLOCK_MAP_H_

#include "storage/BlockMap.h"
#include "storage/PieceInfo.h"
#include "storage/RidInfo.h"
#include "storage/UrlInfo.h"

#include <util/serialization/stl/string.h>
#include <util/serialization/Array.h>

namespace protocol
{

    using storage::BlockMap;

    using storage::SubPieceInfo;

    using storage::RidInfo;

    using storage::UrlInfo;

};

namespace framework
{
    namespace string
    {

        template <typename Archive>
        void serialize(Archive & ar, Uuid & t)
        {
            util::serialization::split_free(ar, t);
        }

        template <typename Archive>
        void load(Archive & ar, Uuid & t)
        {
            UUID uuid;
            ar & uuid.Data1
                & uuid.Data2
                & uuid.Data3
                & framework::container::make_array(uuid.Data4, sizeof(uuid.Data4));
            if (ar)
                t.assign(uuid);
        };

        template <typename Archive>
        void save(Archive & ar, Guid const & t)
        {
            UUID const & uuid = t.data();
            ar & uuid.Data1
                & uuid.Data2
                & uuid.Data3
                & framework::container::make_array(uuid.Data4, sizeof(uuid.Data4));
        }
    }
}

namespace storage
{

    template <typename Archive>
    void serialize(Archive & ar, BlockMap & t)
    {
        util::serialization::split_free(ar, t);
    }

    template <typename Archive>
    void load(Archive & ar, BlockMap & t)
    {
        boost::uint32_t bitmap_size , byte_num;
        boost::uint8_t buf[32];
        ar & bitmap_size;
        byte_num = (bitmap_size+7)/8;
        ar & framework::container::make_array(buf, byte_num);
        t = storage::BlockMap(buf, byte_num, bitmap_size);
    };

    template <typename Archive>
    void save(Archive & ar, BlockMap const & t)
    {
        boost::uint32_t bitset_size ,byte_num;
        boost::uint8_t buf[32]; 
        boost::uint8_t * p = buf;
        bitset_size = t.GetCount();
        t.GetBytes(p);
        byte_num = p - buf;
        ar & bitset_size;
        ar & framework::container::make_array(buf, byte_num);
    }

    template <typename Archive>
    void serialize(Archive & ar, SubPieceInfo & t)
    {
        ar & t.block_index_ & t.subpiece_index_;
    }
/*
    template <typename Archive>
    void serialize(Archive & ar, RidInfo & t)
    {
    }
*/
    template <typename Archive>
    void serialize(Archive & ar, UrlInfo & t)
    {
        ar & t.type_ 
            & util::serialization::make_sized<boost::uint16_t>(t.url_)
            & util::serialization::make_sized<boost::uint16_t>(t.refer_url_) 
            & util::serialization::make_sized<boost::uint16_t>(t.identifier_);
    }

}

#endif // _PROTOCOL_BLOCK_MAP_H_
