//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _LIVE_SUBPIECEINFO_H_
#define _LIVE_SUBPIECEINFO_H_

#ifdef DUMP_OBJECT
#include "../../../peer/count_object_allocate.h"
#endif

namespace protocol
{
    struct LiveSubPieceInfo
#ifdef DUMP_OBJECT
        : public count_object_allocate<LiveSubPieceInfo>
#endif
    {
    private:
        boost::uint32_t block_id_;
        boost::uint16_t subpiece_index_;

    public:
        LiveSubPieceInfo(boost::uint32_t block_id, boost::uint16_t subpiece_index)
            : block_id_(block_id), subpiece_index_(subpiece_index)
        {
        }

        LiveSubPieceInfo()
            : block_id_(0), subpiece_index_(0)
        {
        }

        boost::uint32_t GetBlockId() const { return block_id_; }
        boost::uint16_t GetSubPieceIndex() const { return subpiece_index_; }

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & block_id_ & subpiece_index_;
        }

        static boost::uint32_t length()
        {
            return sizeof( boost::uint32_t ) + sizeof( boost::uint16_t );
        }

        friend std::ostream & operator << (
            std::ostream & os, const LiveSubPieceInfo & sp)
        {
            os << "(" << sp.block_id_ << "|sp:" << sp.subpiece_index_ << ")";
            return os;
        }

        bool operator < (
            const LiveSubPieceInfo& n) const
        {
            if(block_id_ != n.block_id_)
            {
                return block_id_ < n.block_id_;
            }

            return subpiece_index_ < n.subpiece_index_;
        }
    };
}

#endif //_LIVE_SUBPIECEINFO_H_