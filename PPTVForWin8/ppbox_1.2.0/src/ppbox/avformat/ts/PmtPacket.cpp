// PmtPacket.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/ts/PmtPacket.h"

namespace ppbox
{
    namespace avformat
    {

        PmtSection::PmtSection(
            boost::uint16_t program_number)
            : PsiTable(PsiTableId::program_map, 4)
            , byte12(0xe000)
            , byte34(0xf000)
        {
            transport_stream_id = program_number; // transport_stream_id is program_number for pmt
        }

        void PmtSection::add_stream(
            boost::uint8_t type)
        {
            boost::uint16_t pid = TsPid::stream_base + (boost::uint16_t)streams.size();
            if (streams.empty()) {
                pcr_pid = pid;
            }
            streams.push_back(PmtStream(type, pid));
            section_length += 5;
        }

        PmtPayload::PmtPayload(
            TsPacket & packet, 
            boost::uint16_t program_number)
            : pointer(0)
            , packet_(packet)
        {
            sections.push_back(PmtSection(program_number));
        }

        PmtPacket::PmtPacket()
            : TsPacket(TsPid::pmt, 0)
            , PmtPayload(*this, default_program_number)
        {
        }

        void PmtPacket::complete()
        {
            fill_payload2(1 + sections.front().section_size());
        }

    } // namespace avformat
} // namespace ppbox
