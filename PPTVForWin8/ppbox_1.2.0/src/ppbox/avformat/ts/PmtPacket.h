// PmtPacket.h

#ifndef _PPBOX_AVFORMAT_TS_PMT_PACKET_H_
#define _PPBOX_AVFORMAT_TS_PMT_PACKET_H_

#include "ppbox/avformat/ts/PsiPacket.h"

namespace ppbox
{
    namespace avformat
    {

        struct PmtStream
        {
            boost::uint8_t stream_type;
            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint16_t reserved : 3;
                    boost::uint16_t elementary_pid : 13;
#else
                    boost::uint16_t elementary_pid : 13;
                    boost::uint16_t reserved : 3;
#endif
                };
                boost::uint16_t byte23;
            };

            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint16_t reserved1 : 4;
                    boost::uint16_t es_info_length : 12;
#else
                    boost::uint16_t es_info_length : 12;
                    boost::uint16_t reserved1 : 4;
#endif
                };
                boost::uint16_t byte45;
            };

            //for (i = 0; i < N; i++) {
            //    descriptor()
            //}
            std::vector<boost::uint8_t> descriptor;

            PmtStream()
                : stream_type(0)
                , byte23(0)
                , byte45(0)
            {
            }

            PmtStream(
                boost::uint8_t stream_type, 
                boost::uint16_t elementary_pid)
                : stream_type(stream_type)
                , byte23(0xe000 | (elementary_pid & 0x1fff))
                , byte45(0xf000)
            {
            }

            boost::uint32_t size() const
            {
                return 5 + es_info_length;
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & stream_type;
                ar & byte23;
                ar & byte45;
                if (es_info_length) {
                    descriptor.resize(es_info_length);
                    ar & framework::container::make_array(&descriptor.front(), descriptor.size());
                }
            }
        };

        struct PmtSection
            : PsiTable
        {
            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint16_t reserved : 3;
                    boost::uint16_t pcr_pid : 13;
#else
                    boost::uint16_t pcr_pid : 13;
                    boost::uint16_t reserved : 3;
#endif
                };
                boost::uint16_t byte12;
            };

            union {
                struct {
#ifdef   BOOST_BIG_ENDIAN
                    boost::uint16_t reserved1 : 4;
                    boost::uint16_t program_info_length : 12;
#else
                    boost::uint16_t program_info_length : 12;
                    boost::uint16_t reserved1 : 4;
#endif
                };
                boost::uint16_t byte34;
            };

            //for (i = 0; i < N; i++) {
            //    descriptor()
            //}
            std::vector<boost::uint8_t> descriptor;

            TsVector<PmtStream> streams;

            boost::uint32_t crc;

            PmtSection()
                : byte12(0)
                , byte34(0)
            {
            }

            PmtSection(
                boost::uint16_t program_number);

            void add_stream(
                boost::uint8_t type);

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                AutoCrc<Archive> ac(ar, crc);

                PsiTable::serialize(ar);
                ar & byte12;
                ar & byte34;
                if (program_info_length) {
                    descriptor.resize(program_info_length);
                    ar & framework::container::make_array(&descriptor.front(), descriptor.size());
                }
                streams.set_byte_size(body_size() - 4 - program_info_length);
                ar & streams;
            }
        };

        struct PmtPayload
        {
            boost::uint8_t pointer;
            TsVector<PmtSection> sections;

            PmtPayload(
                TsPacket & packet)
                : pointer(0)
                , packet_(packet)
            {
            }

            PmtPayload(
                TsPacket & packet, 
                boost::uint16_t program_number);

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & pointer;
                sections.set_byte_size_with_suffer(packet_.payload_size() - 1);
                ar & sections;
            }

        private:
            TsPacket & packet_;
        };

        struct PmtPacket
            : TsPacket
            , PmtPayload
        {
            std::vector<PmtStream> streams;

            // construct pmt with only one section (default id)
            PmtPacket();

            void complete();

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                TsPacket::serialize(ar);
                PmtPayload::serialize(ar);
            }
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_TS_PMT_PACKET_H_
