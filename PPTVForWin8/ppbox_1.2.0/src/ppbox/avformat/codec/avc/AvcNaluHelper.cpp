// AvcNaluHelper.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/codec/avc/AvcNaluHelper.h"
#include "ppbox/avformat/codec/avc/AvcNalu.h"

#include <util/buffers/BuffersSize.h>
#include <util/buffers/BuffersCopy.h>

#include <framework/system/BytesOrder.h>

namespace ppbox
{
    namespace avformat
    {

        AvcNaluHelper::AvcNaluHelper(
            boost::uint8_t nalu_length_size)
            : nalu_length_size_(nalu_length_size)
        {
            nalu_start_code_[0] = 0;
            nalu_start_code_[1] = 0;
            nalu_start_code_[2] = 0;
            nalu_start_code_[3] = 1;
        }

        void AvcNaluHelper::nalus(
            std::vector<NaluBuffer> & nalus)
        {
            return nalus.swap(nalus_);
        }

        static void continue_find_nalu(
            SampleBuffers::FindIterator2 & iter, 
            SampleBuffers::FindIterator2 & iend, 
            SampleBuffers::BuffersPosition & pos)
        {
            SampleBuffers::FindIterator2 ipos = iter;
            ipos.skip_bytes(2);
            while (ipos != iend) {
                boost::uint8_t byte = ipos->dereference_byte();
                ipos.skip_bytes(1);
                if (byte == 0) {
                } else if (byte == 1) {
                    pos = *ipos;
                    break;
                } else {
                    if (++iter != iend) {
                        ipos = iter;
                        ipos.skip_bytes(2);
                    } else {
                        break;
                    }
                }
            }
        }

        bool AvcNaluHelper::from_stream(
            boost::uint32_t size, 
            buffers_t const & data)
        {
            nalus_.clear();
            SampleBuffers::FindIterator2 iter(data, boost::asio::buffer("\0\0", 2));
            SampleBuffers::FindIterator2 iend;
            SampleBuffers::BuffersPosition pos;
            continue_find_nalu(iter, iend, pos);
            while (iter != iend) {
                SampleBuffers::BuffersPosition cur_pos = pos;
                NaluHeader const nalu_header(cur_pos.dereference_byte());
                if (nalu_header.nal_unit_type == NaluHeader::IDR ||
                    nalu_header.nal_unit_type == NaluHeader::UNIDR) {
                        // 假定 IDR，UNIDR 是最后一个Nalu，不继续搜索，以提高效率
                        nalus_.push_back(NaluBuffer(size - cur_pos.skipped_bytes(), cur_pos, iter.end_position()));
                        break;
                }
                ++iter;
                continue_find_nalu(iter, iend, pos);
                nalus_.push_back(NaluBuffer(cur_pos, iter.position()));
            }
            return true;
        }

        bool AvcNaluHelper::from_packet(
            boost::uint32_t size, 
            buffers_t const & data)
        {
            nalus_.clear();
            SampleBuffers::BuffersPosition position(data.begin(), data.end());
            SampleBuffers::BuffersPosition end(data.end());
            while (!position.at_end()) {
                boost::uint32_t len = 0;
                util::buffers::buffers_copy(
                    boost::asio::buffer((char *)&len + 4 - nalu_length_size_, nalu_length_size_), 
                    util::buffers::Container<boost::asio::const_buffer, SampleBuffers::BufferIterator>(SampleBuffers::BufferIterator(position, end)));
                len = framework::system::BytesOrder::net_to_host_long(len);
                position.increment_bytes(end, nalu_length_size_);
                SampleBuffers::BuffersPosition pos = position;
                position.increment_bytes(end, len);
                nalus_.push_back(NaluBuffer(pos, position));
            }
            return true;
        }

        bool AvcNaluHelper::to_stream(
            boost::uint32_t & size, 
            buffers_t & data)
        {
            std::deque<boost::asio::const_buffer> datas;
            for (boost::uint32_t i = 0; i < nalus_.size(); ++i) {
                NaluBuffer const & nalu = nalus_[i];
                NaluHeader nalu_header(nalu.begin.dereference_byte());
                datas.push_back(boost::asio::buffer(nalu_start_code_));
                datas.insert(datas.end(), nalu.buffers_begin(), nalu.buffers_end());
                size += nalu.size + 4;
            }
            data.swap(datas);
            return true;
        }

        bool AvcNaluHelper::to_packet(
            boost::uint32_t & size, 
            buffers_t & data)
        {
            std::deque<boost::asio::const_buffer> datas;
            size_t n = 0;
            for (boost::uint32_t i = 0; i < nalus_.size(); ++i) {
                NaluBuffer const & nalu = nalus_[i];
                NaluHeader const nalu_header(nalu.begin.dereference_byte());
                if (nalu_header.nal_unit_type == NaluHeader::UNIDR 
                    || nalu_header.nal_unit_type == NaluHeader::IDR
                    || nalu_header.nal_unit_type == NaluHeader::SEI) {
                        assert(n < sizeof(nalu_length_) / sizeof(nalu_length_[0]));
                        size += nalu.size + nalu_length_size_;
                        nalu_length_[n] = framework::system::BytesOrder::host_to_big_endian_long(nalu.size);
                        datas.push_back(boost::asio::buffer((boost::uint8_t*)&(nalu_length_[n]) + 4 - nalu_length_size_, nalu_length_size_));
                        datas.insert(datas.end(), nalu.buffers_begin(), nalu.buffers_end());
                        ++n;
                }
            }
            data.swap(datas);
            return true;
        }

        boost::uint8_t AvcNaluHelper::get_frame_type_from_stream(
            std::vector<boost::uint8_t> const & data, 
            boost::uint32_t * offset)
        {
            buffers_t buffers;
            buffers.push_back(boost::asio::buffer(data));
            from_stream(data.size(), buffers);
            if (nalus_.size() > 0) {
                NaluHeader const nalu_header(nalus_.back().begin.dereference_byte());
                if (nalu_header.nal_unit_type == 1 
                    || nalu_header.nal_unit_type == 5) {
                        if (offset) {
                            *offset = nalus_.back().begin.skipped_bytes() - 4;
                        }
                        return (boost::uint8_t)nalu_header.nal_unit_type;
                }
            }
            return 0;
        }

    } // namespace avformat
} // namespace ppbox
