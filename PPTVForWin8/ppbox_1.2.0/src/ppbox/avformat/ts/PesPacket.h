// PesFormat.h

#ifndef _PPBOX_AVFORMAT_TS_PES_FORMAT_H_
#define _PPBOX_AVFORMAT_TS_PES_FORMAT_H_

#include "ppbox/avformat/ts/TsFormat.h"

namespace ppbox
{
    namespace avformat
    {

        struct Bits33
        {
            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint8_t flag : 4;
                    boost::uint8_t bits_32_30 : 3;
                    boost::uint8_t marker_bit1 : 1;
#else
                    boost::uint8_t marker_bit1 : 1;
                    boost::uint8_t bits_32_30 : 3;
                    boost::uint8_t flag : 4;
#endif
                };
                boost::uint8_t byte1;
            };

            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint16_t bits_29_15 : 15;
                    boost::uint8_t marker_bit2 : 1;
#else
                    boost::uint16_t marker_bit2 : 1;
                    boost::uint16_t bits_29_15 : 15;
#endif
                };
                boost::uint16_t byte23;
            };

            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint16_t bits_14_0 : 15;
                    boost::uint16_t marker_bit3 : 1;
#else
                    boost::uint16_t marker_bit3 : 1;
                    boost::uint16_t bits_14_0 : 15;
#endif
                };
                boost::uint16_t byte45;
            };

            Bits33()
                : byte1(0)
                , byte23(0)
                , byte45(0)
            {
            }

            Bits33(
                boost::uint8_t flag, 
                boost::uint64_t value)
                : byte1((flag << 4) | ((boost::uint8_t)(value >> 29) & 0x0e) | 1)
                , byte23(((boost::uint16_t)(value >> 14) & 0xfffe) | 1)
                , byte45(((boost::uint16_t)(value << 1) & 0xfffe) | 1)
            {
            }

            boost::uint64_t value() const
            {
                return (boost::uint64_t)bits_32_30 << 30 | (boost::uint64_t)bits_29_15 << 15 | bits_14_0;
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & byte1;
                ar & byte23;
                ar & byte45;
            }
        };

        struct PesPacket
        {
            boost::uint8_t packet_start_code_prefix1;
            boost::uint8_t packet_start_code_prefix2;
            boost::uint8_t packet_start_code_prefix3;
            boost::uint8_t stream_id;
            boost::uint16_t packet_length;

            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint8_t reserved : 2;
                    boost::uint8_t PES_scrambling_control : 2;
                    boost::uint8_t PES_priority : 1;
                    boost::uint8_t data_alignment_indicator : 1;
                    boost::uint8_t copyright : 1;
                    boost::uint8_t original_or_copy : 1;
#else
                    boost::uint8_t original_or_copy : 1;
                    boost::uint8_t copyright : 1;
                    boost::uint8_t data_alignment_indicator : 1;
                    boost::uint8_t PES_priority : 1;
                    boost::uint8_t PES_scrambling_control : 2;
                    boost::uint8_t reserved : 2;
#endif
                };
                boost::uint8_t byte1;
            };

            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint8_t PTS_DTS_flags : 2;
                    boost::uint8_t ESCR_flag : 1;
                    boost::uint8_t ES_rate_flag : 1;
                    boost::uint8_t DSM_trick_mode_flag : 1;
                    boost::uint8_t additional_copy_info_flag : 1;
                    boost::uint8_t PES_CRC_flag : 1;
                    boost::uint8_t PES_extension_flag : 1;
#else
                    boost::uint8_t PES_extension_flag : 1;
                    boost::uint8_t PES_CRC_flag : 1;
                    boost::uint8_t additional_copy_info_flag : 1;
                    boost::uint8_t DSM_trick_mode_flag : 1;
                    boost::uint8_t ES_rate_flag : 1;
                    boost::uint8_t ESCR_flag : 1;
                    boost::uint8_t PTS_DTS_flags : 2;
#endif
                };
                boost::uint8_t byte2;
            };

            boost::uint8_t header_data_length;

            Bits33 pts_bits;
            Bits33 dts_bits;


            PesPacket()
                : packet_start_code_prefix1(0)
                , packet_start_code_prefix2(0)
                , packet_start_code_prefix3(0)
                , stream_id(0)
                , packet_length(0)
                , byte1(0)
                , byte2(0)
            {
            }

            PesPacket(
                boost::uint8_t stream_id, 
                boost::uint32_t size, 
                boost::uint64_t cts);

            PesPacket(
                boost::uint8_t stream_id, 
                boost::uint32_t size, 
                boost::uint64_t cts, 
                boost::uint64_t dts);

            boost::uint32_t payload_length() const
            {
                return packet_length ? packet_length - 3 - header_data_length : 0;
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & packet_start_code_prefix1;
                ar & packet_start_code_prefix2;
                ar & packet_start_code_prefix3;
                ar & stream_id;
                ar & packet_length;
                ar & byte1;
                ar & byte2;
                ar & header_data_length;
                if (PTS_DTS_flags == 2) { // pts
                    ar & pts_bits;
                }
                if (PTS_DTS_flags == 3) { // pts & dts
                    ar & pts_bits;
                    ar & dts_bits;
                }
            }
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_TS_PES_FORMAT_H_
