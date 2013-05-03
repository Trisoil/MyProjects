//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _PROTOCOL_STRUCT_SERIALIZATION_H_
#define _PROTOCOL_STRUCT_SERIALIZATION_H_

#include "struct/BlockMap.h"
#include "struct/PieceInfo.h"
#include "struct/RidInfo.h"
#include "struct/UrlInfo.h"
#include "struct/SubPieceInfo.h"

#include "struct/LivePieceInfoEx.h"
#include "struct/LiveSubPieceInfo.h"

#include <util/serialization/stl/string.h>
#include <util/serialization/Array.h>
#include <util/serialization/Uuid.h>
#include <util/serialization/Collection.h>

namespace protocol
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
        if (bitmap_size > 32*8)
        {
            bitmap_size = 0;
        }
        byte_num = (bitmap_size + 7)/8;
        ar & framework::container::make_array(buf, byte_num);
        if (ar)
        {
            t = protocol::BlockMap(buf, byte_num, bitmap_size);
        }
    };

    template <typename Archive>
    void save(Archive & ar, BlockMap const & t)
    {
        boost::uint32_t bitset_size , byte_num;
        boost::uint8_t buf[32];
        boost::uint8_t * p = buf;
        bitset_size = t.GetCount();
        t.GetBytes(p);
        byte_num = (bitset_size + 7)/8;
        ar & bitset_size;
        ar & framework::container::make_array(buf, byte_num);
    }

    template <typename Archive>
    void serialize(Archive & ar, SubPieceInfo & t)
    {
        ar & t.block_index_ & t.subpiece_index_;
    }

    template <typename Archive>
    void serialize(Archive & ar, RidInfo & t)
    {
        ar & t.rid_ & t.file_length_;
        t.block_count_ = t.block_md5_s_.size();
        ar & t.block_count_;
        ar & t.block_size_;
        util::serialization::serialize_collection(ar, t.block_md5_s_, t.block_count_);
    }

    template <typename Archive>
    void serialize(Archive & ar, UrlInfo & t)
    {
        boost::uint16_t length = t.url_.length() + t.refer_url_.length() + 7;
        ar & length
            & t.type_
            & util::serialization::make_sized<boost::uint16_t>(t.url_)
            & util::serialization::make_sized<boost::uint16_t>(t.refer_url_);
            // & util::serialization::make_sized<boost::uint16_t>(t.identifier_);
    }

}

#endif  // _PROTOCOL_STRUCT_SERIALIZATION_H_
