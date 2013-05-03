// TsFormat.h

#ifndef _PPBOX_AVFORMAT_TS_TS_FORMAT_H_
#define _PPBOX_AVFORMAT_TS_TS_FORMAT_H_

#include "ppbox/avformat/stream/FormatBuffer.h"

#include <util/archive/BigEndianBinaryIArchive.h>
#include <util/archive/BigEndianBinaryOArchive.h>

namespace ppbox
{
    namespace avformat
    {

        typedef util::archive::BigEndianBinaryIArchive<boost::uint8_t> TsIArchive;
        typedef util::archive::BigEndianBinaryOArchive<boost::uint8_t> TsOArchive;

        static boost::uint16_t const default_program_number = 1;

        struct TsPid
        {
            enum Enum
            {
                pat         = 0x00, 
                pmt         = 0x64, 
                stream_base = 0x65, 
            };
        };

        struct PsiTableId
        {
            enum Enum
            {
                program_association, 
                conditional_access, 
                program_map, 
                discription, 
                iso_14496_scene_discription, 
                iso_14496_object_discription, 

                forbidden = 0xff
            };
        };

        struct TsStreamId
        {
            enum Enum
            {
                program_stream_map  = 0xbc, 
                private_stream_1    = 0xbd, 
                padding_stream      = 0xbe, 
                private_stream_2    = 0xbf, 
                audio_base          = 0xc0, 
                video_base          = 0xe0, 
                ecm_stream          = 0xf0, 
                emm_stream          = 0xf1
            };
        };

        struct TsStreamType
        {
            enum Enum
            {
                iso_reserved        = 0x00, 
                iso_11172_video     = 0x01, // MPEG-1 video
                iso_13818_2_video   = 0x02, // MPEG-2 video
                iso_11172_audio     = 0x03, // MPEG-1 audio layer 1,2,3
                iso_13818_3_audio   = 0x04, // MPEG-2 audio layer 1,2,3

                iso_13818_7_audio   = 0x0f, // AAC Audio with ADTS
                iso_13818_video   = 0x1b, // AVC
            };
        };

        template <typename _Ty>
        struct TsVector
            : std::vector<_Ty>
        {
            TsVector()
                : has_suffer_(false)
                , size_(0)
            {
            }

            SERIALIZATION_SPLIT_MEMBER();

            void set_byte_size(
                boost::uint64_t size)
            {
                has_suffer_ = false;
                size_ = size;
            }

            void set_byte_size_with_suffer(
                boost::uint64_t size)
            {
                has_suffer_ = true;
                size_ = size;
            }

            template <typename Archive>
            void load(
                Archive & ar)
            {
                this->clear();
                boost::uint64_t byte_end = (boost::uint64_t)ar.tellg() + size_;
                while ((boost::uint64_t)ar.tellg() < byte_end) {
                    if (has_suffer_) {
                        std::basic_streambuf<boost::uint8_t> * buf = ar.rdbuf();
                        if (buf->sgetc() == 0xff) {
                            break;
                        }
                    }
                    _Ty t;
                    ar >> t;
                    if (ar) {
                        this->push_back(t);
                    } else {
                        break;
                    }
                }
                if (has_suffer_) {
                    while ((boost::uint64_t)ar.tellg() < byte_end) {
                        boost::uint8_t suffer(0xff);
                        ar >> suffer;
                    }
                }
                assert(!ar || (boost::uint64_t)ar.tellg() == byte_end);
            }

            template <typename Archive>
            void save(
                Archive & ar) const
            {
                boost::uint64_t byte_end = (boost::uint64_t)ar.tellp() + size_;
                typename TsVector::const_iterator iter = this->begin();
                typename TsVector::const_iterator iend = this->end();
                while ((boost::uint64_t)ar.tellp() < byte_end && iter != iend) {
                    ar << *iter;
                    if (ar) {
                        ++iter;
                    } else {
                        break;
                    }
                }
                if (has_suffer_) {
                    while ((boost::uint64_t)ar.tellp() < byte_end) {
                        boost::uint8_t suffer(0xff);
                        ar << suffer;
                    }
                }
                assert(!ar || ((boost::uint64_t)ar.tellp() == byte_end && iter == iend));
            }

        private:
            bool has_suffer_;
            boost::uint64_t size_;
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_TS_TS_FORMAT_H_
