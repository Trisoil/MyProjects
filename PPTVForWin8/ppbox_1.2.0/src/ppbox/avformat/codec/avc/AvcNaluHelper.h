// AvcNaluHelper.h

#ifndef _PPBOX_AVFORMAT_CODEC_AVC_AVC_NALU_HELPER_H_
#define _PPBOX_AVFORMAT_CODEC_AVC_AVC_NALU_HELPER_H_

#include "ppbox/avformat/codec/avc/AvcNaluBuffer.h"

namespace ppbox
{
    namespace avformat
    {

        class AvcNaluHelper
        {
        public:
            typedef NaluBuffer::ConstBuffers buffers_t;

        public:
            AvcNaluHelper(
                boost::uint8_t nalu_length_size = 4);

        public:
            boost::uint8_t nalu_length_size() const
            {
                return nalu_length_size_;
            }

            void nalu_length_size(
                boost::uint8_t v)
            {
                nalu_length_size_ = v;
            }

            std::vector<NaluBuffer> & nalus()
            {
                return nalus_;
            }

            void nalus(
                std::vector<NaluBuffer> & nalus);

        public:
            bool from_stream(
                boost::uint32_t size, 
                buffers_t const & data);

            bool from_packet(
                boost::uint32_t size, 
                buffers_t const & data);

            bool from_stream(
                buffers_t const & data);

            bool from_packet(
                buffers_t const & data);

        public:
            bool to_stream(
                boost::uint32_t & size, 
                buffers_t & data);

            bool to_packet(
                boost::uint32_t & size, 
                buffers_t & data);

            bool to_stream(
                buffers_t & data);

            bool to_packet(
                buffers_t & data);

        public:
            // return 0 if not found
            boost::uint8_t get_frame_type_from_stream(
                std::vector<boost::uint8_t> const & data, 
                boost::uint32_t * offset);

        private:
            boost::uint8_t nalu_length_size_;
            union {
                boost::uint32_t nalu_length_[3];
                boost::uint8_t nalu_start_code_[4];
            };
            std::vector<NaluBuffer> nalus_;
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_CODEC_AVC_AVC_NALU_HELPER_H_
