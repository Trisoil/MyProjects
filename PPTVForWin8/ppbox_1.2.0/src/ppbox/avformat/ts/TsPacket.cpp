// TsPacket.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/ts/TsPacket.h"

namespace ppbox
{
    namespace avformat
    {

        AdaptationField::AdaptationField(
            boost::uint64_t program_clock_reference)
            : adaptation_field_length(7)
            , flags(0x10) // PCR_flag:1
            , program_clock_reference_base(boost::uint32_t(program_clock_reference >> 1))
            , program_clock_reference_2(boost::uint16_t(program_clock_reference << 15 | 0x7e00))
        {
        }

        TsPacket::TsPacket(
            boost::uint16_t pid, 
            boost::uint8_t continuity_counter)
            : TsPacketHeader(pid, continuity_counter)
        {
        }

        TsPacket::TsPacket(
            bool start, 
            boost::uint16_t pid, 
            boost::uint8_t continuity_counter)
            : TsPacketHeader(start, pid, continuity_counter)
        {
        }

        TsPacket::TsPacket(
            boost::uint16_t pid, 
            boost::uint8_t continuity_counter, 
            boost::uint64_t pcr)
            : TsPacketHeader(true, pid, continuity_counter)
            , adaptation(pcr)
        {
            adaptat_field_control = 3;
        }

        void TsPacket::fill_payload(
            boost::uint32_t & size)
        {
            if (adaptat_field_control & 2) {
                boost::int32_t delta = PAYLOAD_SIZE - (1 + adaptation.adaptation_field_length + size);
                if (delta > 0) {
                    adaptation.adaptation_field_length += (boost::uint8_t)delta;
                    adaptation.stuffing_bytes.insert(adaptation.stuffing_bytes.end(), delta, 0xff);
                } else if (delta < 0) {
                    size += delta;
                }
            } else {
                boost::int32_t delta = PAYLOAD_SIZE - size;
                if (delta > 1) {
                    adaptat_field_control = 3;
                    --delta; // 
                    adaptation.adaptation_field_length = (boost::uint8_t)delta;
                    --delta; // 
                    adaptation.stuffing_bytes.insert(adaptation.stuffing_bytes.end(), delta, 0xff);
                } else if (delta == 1) {
                    adaptat_field_control = 3;
                    adaptation.adaptation_field_length = 0; // zero is valid
                } else if (delta < 0) {
                    size = PAYLOAD_SIZE;
                }
            }
            if (size == 0) {
                adaptat_field_control &= ~1;
            }
        }

    } // namespace avformat
} // namespace ppbox
