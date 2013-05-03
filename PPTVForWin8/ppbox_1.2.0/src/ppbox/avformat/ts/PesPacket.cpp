// PesPacket.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/ts/PesPacket.h"

namespace ppbox
{
    namespace avformat
    {

        PesPacket::PesPacket(
            boost::uint8_t stream_id, 
            boost::uint32_t size, 
            boost::uint64_t pts)
            : packet_start_code_prefix1(0)
            , packet_start_code_prefix2(0)
            , packet_start_code_prefix3(1)
            , stream_id(stream_id)
            , packet_length(0)
            , byte1(0x84)
                //reserved = 2;   // 10
                //PES_scrambling_control = 0; // 00
                //PES_priority = 0;
                //data_alignment_indicator = 1;
                //copyright = 0;
                //original_or_copy = 0;
            , byte2(0x80)
                //pts_dts_flags = 2;
                //ESCR_flag = 0;
                //ES_rate_flag = 0;
                //DSM_trick_mode_flag = 0;
                //additional_copy_info_flag = 0;
                //PES_CRC_flag = 0;
                //PES_extension_flag = 0;
            , header_data_length(5) // 该字段后面还有5个字节
            , pts_bits(0x02, pts) // 0x02: same as pts_dts_flags
        {
            boost::uint8_t const header_data_length2 = header_data_length + 3;
            packet_length = (size + header_data_length2) < 0xffff ? (boost::uint16_t)(size + header_data_length2) : 0;
        }

        PesPacket::PesPacket(
            boost::uint8_t stream_id, 
            boost::uint32_t size, 
            boost::uint64_t pts, 
            boost::uint64_t dts)
            : packet_start_code_prefix1(0)
            , packet_start_code_prefix2(0)
            , packet_start_code_prefix3(1)
            , stream_id(stream_id)
            , packet_length(0)
            , byte1(0x84)
                //reserved = 2;   // 10
                //PES_scrambling_control = 0; // 00
                //PES_priority = 0;
                //data_alignment_indicator = 1;
                //copyright = 0;
                //original_or_copy = 0;
            , byte2(0xC0)
                //PTS_DTS_flags = 3;
                //ESCR_flag = 0;
                //ES_rate_flag = 0;
                //DSM_trick_mode_flag = 0;
                //additional_copy_info_flag = 0;
                //PES_CRC_flag = 0;
                //PES_extension_flag = 0;
            , header_data_length(10) // 该字段后面还有10个字节
            , pts_bits(0x03, pts)
            , dts_bits(0x01, dts)
        {
            boost::uint8_t const header_data_length2 = header_data_length + 3;
            packet_length = (size + header_data_length2) < 0xffff ? (boost::uint16_t)(size + header_data_length2) : 0;
        }

    } // namespace avformat
} // namespace ppbox
