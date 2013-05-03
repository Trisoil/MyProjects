// PsiFormat.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/ts/PsiPacket.h"

namespace ppbox
{
    namespace avformat
    {

        PsiTable::PsiTable(
            boost::uint8_t id, 
            boost::uint16_t body_length)
            : table_id(id)
            , byte34(0xb000 | ((5 + body_length + 4) & 0x1fff)) // 5: behind fields, 4: crc
            , transport_stream_id(0)
            , byte7(0xC1)
            , section_number(0)
            , last_section_number(0)
        {
        }

    } // namespace avformat
} // namespace ppbox
