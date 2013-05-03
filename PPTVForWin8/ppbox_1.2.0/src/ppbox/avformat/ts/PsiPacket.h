// PsiPacket.h

#ifndef _PPBOX_AVFORMAT_TS_PSI_PACKET_H_
#define _PPBOX_AVFORMAT_TS_PSI_PACKET_H_

#include "ppbox/avformat/ts/TsPacket.h"
#include "ppbox/avformat/ts/CrcBuffer.h"

#include <boost/asio/buffer.hpp>

namespace ppbox
{
    namespace avformat
    {

        struct PsiTable
        {
            boost::uint8_t table_id;

            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint16_t section_syntax_indicator : 1;
                    boost::uint16_t undef : 1;
                    boost::uint16_t reserved : 2;
                    boost::uint16_t section_length : 12;
#else
                    boost::uint16_t section_length : 12;
                    boost::uint16_t reserved : 2;
                    boost::uint16_t undef : 1;
                    boost::uint16_t section_syntax_indicator : 1;
#endif
                };
                boost::uint16_t byte34;
            };

            boost::uint16_t transport_stream_id;

            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint8_t reserved1 : 2;
                    boost::uint8_t version_number : 5;
                    boost::uint8_t current_next_indicator : 1;
#else
                    boost::uint8_t current_next_indicator : 1;
                    boost::uint8_t version_number : 5;
                    boost::uint8_t reserved1 : 2;
#endif
                };
                boost::uint8_t byte7;
            };

            boost::uint8_t section_number;
            boost::uint8_t last_section_number;

            PsiTable()
                : table_id(0)
                , byte34(0)
                , byte7(0)
                , section_number(0)
                , last_section_number(0)
            {
            }

            PsiTable(
                boost::uint8_t id, 
                boost::uint16_t body_length);

            boost::uint32_t section_size() const
            {
                return 3 + section_length;
            }

            boost::uint32_t body_size() const
            {
                return section_length - 5 - 4;
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & table_id;
                ar & byte34;
                ar & transport_stream_id;
                ar & byte7;
                ar & section_number;
                ar & last_section_number;
            }
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_TS_PSI_PACKET_H_
