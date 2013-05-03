// TsPacket.h

#ifndef _PPBOX_AVFORMAT_TS_TS_PACKET_H_
#define _PPBOX_AVFORMAT_TS_TS_PACKET_H_

#include "ppbox/avformat/ts/TsFormat.h"

namespace ppbox
{
    namespace avformat
    {

        struct TsPacketHeader
        {
            boost::uint8_t  sync_byte;
            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint16_t transport_error_indicator : 1;
                    boost::uint16_t payload_uint_start_indicator : 1;
                    boost::uint16_t transport_priority : 1;
                    boost::uint16_t pid : 13;
#else
                    boost::uint16_t pid : 13;
                    boost::uint16_t transport_priority : 1;
                    boost::uint16_t payload_uint_start_indicator : 1;
                    boost::uint16_t transport_error_indicator : 1;
#endif
                };
                boost::uint16_t two_bytes;
            };

            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint8_t  transport_scrambling_control : 2;
                    boost::uint8_t  adaptat_field_control : 2;
                    boost::uint8_t  continuity_counter : 4;
#else
                    boost::uint8_t  continuity_counter : 4;
                    boost::uint8_t  adaptat_field_control : 2;
                    boost::uint8_t  transport_scrambling_control : 2;
#endif
                };
                boost::uint8_t one_byte;
            };

            TsPacketHeader()
                : sync_byte(0)
                , two_bytes(0)
                , one_byte(0)
            {
            }

            TsPacketHeader(
                boost::uint16_t pid, 
                boost::uint8_t continuity_counter)
                : sync_byte(0x47)
                , two_bytes(0x6000 | (pid & 0x1fff)) 
                    // transport_error_indicator:0
                    // payload_uint_start_indicator:1
                    // transport_priority:1
                    // pid:pid
                , one_byte(0x10 | (continuity_counter & 0x0f)) 
                    // transport_scrambling_control:00
                    // adaptat_field_control:01
                    // continuity_counter:continuity_counter
            {
            }

            TsPacketHeader(
                bool start, 
                boost::uint16_t pid, 
                boost::uint8_t continuity_counter)
                : sync_byte(0x47)
                , two_bytes((start ? 0x4000 : 0x0000) | (pid & 0x1fff)) 
                    // transport_error_indicator:0
                    // payload_uint_start_indicator:start
                    // transport_priority:0
                    // pid:pid
                , one_byte(0x10 | (continuity_counter & 0x0f)) 
                    // transport_scrambling_control:00
                    // adaptat_field_control:01
                    // continuity_counter:continuity_counter
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & sync_byte;
                ar & two_bytes;
                ar & one_byte;
            }
        };

        struct AdaptationField
        {
            boost::uint8_t adaptation_field_length;
            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint8_t discontinuity_indicator : 1;
                    boost::uint8_t random_access_indicator : 1;
                    boost::uint8_t elementary_stream_priority_indicator : 1;
                    boost::uint8_t PCR_flag : 1;
                    boost::uint8_t OPCR_flag : 1;
                    boost::uint8_t splicing_point_flag : 1;
                    boost::uint8_t transport_private_data_flag : 1;
                    boost::uint8_t adaptation_field_extension_flag : 1;
#else 
                    boost::uint8_t adaptation_field_extension_flag : 1;
                    boost::uint8_t transport_private_data_flag : 1;
                    boost::uint8_t splicing_point_flag : 1;
                    boost::uint8_t OPCR_flag : 1;
                    boost::uint8_t PCR_flag : 1;
                    boost::uint8_t elementary_stream_priority_indicator : 1;
                    boost::uint8_t random_access_indicator : 1;
                    boost::uint8_t discontinuity_indicator : 1;
#endif
                };
                boost::uint8_t flags;
            };

            // if (PCR_flag == 1) bit length (48bit)
            boost::uint32_t program_clock_reference_base; // 33
            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint16_t program_clock_reference_base_last1bit : 1;
                    boost::uint16_t pcr_reserved : 6;
                    boost::uint16_t program_clock_reference_extension : 9;
#else 
                    boost::uint16_t program_clock_reference_extension : 9;
                    boost::uint16_t program_clock_reference_reserved : 6;
                    boost::uint16_t program_clock_reference_base_last1bit : 1;
#endif
                };
                boost::uint16_t program_clock_reference_2;
            };

            // 其他标志没有处理，都算在stuffing_bytes中
            std::vector<boost::uint8_t> stuffing_bytes;

            AdaptationField()
                : adaptation_field_length(0)
                , flags(0)
                , program_clock_reference_base(0)
                , program_clock_reference_2(0)
            {
            }

            AdaptationField(
                boost::uint64_t program_clock_reference);

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & adaptation_field_length;
                if (adaptation_field_length > 0) {
                    ar & flags;
                    boost::int32_t suffering_size = adaptation_field_length - 1;
                    if (1 == PCR_flag) {
                        ar & program_clock_reference_base;
                        ar & program_clock_reference_2;
                        suffering_size -= 6;
                    }
                    if (suffering_size > 0) {
                        util::serialization::serialize_collection(ar, stuffing_bytes, suffering_size);
                    }
                }
            }
        };

        struct TsPacket
            : TsPacketHeader
        {
            AdaptationField adaptation;

            TsPacket() {}

            TsPacket(
                boost::uint16_t pid, 
                boost::uint8_t continuity_counter);

            TsPacket(
                bool start, 
                boost::uint16_t pid, 
                boost::uint8_t continuity_counter);

            TsPacket(
                boost::uint16_t pid, 
                boost::uint8_t continuity_counter, 
                boost::uint64_t pcr);

            void fill_payload(
                boost::uint32_t & size);

            void fill_payload2(
                boost::uint32_t size)
            {
                fill_payload(size);
            }

            boost::uint8_t header_adaptation_size() const
            {
                return 4 + ((adaptat_field_control & 2) ? (adaptation.adaptation_field_length + 1) : 0);
            }

            boost::uint8_t payload_size() const
            {
                return PACKET_SIZE - header_adaptation_size();
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                TsPacketHeader::serialize(ar);
                if (adaptat_field_control & 2)
                    ar & adaptation;
            }

            static boost::uint32_t const TIME_SCALE = 90000;
            static boost::uint8_t const PACKET_SIZE = 188;
            static boost::uint8_t const PAYLOAD_SIZE = PACKET_SIZE - 4;

        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_TS_TS_PACKET_H_
