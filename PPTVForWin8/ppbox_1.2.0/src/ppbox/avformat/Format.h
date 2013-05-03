// Format.h

#ifndef _PPBOX_AVFORMAT_FORMAT_H_
#define _PPBOX_AVFORMAT_FORMAT_H_

#include "ppbox/avformat/codec/Codec.h"

#include <boost/detail/endian.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/intrusive_ptr.hpp>

namespace ppbox
{
    namespace avformat
    {

#ifdef BOOST_BIG_ENDIAN
#define MAKE_FOURC_TYPE(c1, c2, c3, c4) \
    ((((boost::uint32_t)c1) << 24) | \
    (((boost::uint32_t)c2) << 16) | \
    (((boost::uint32_t)c3) << 8) | \
    (((boost::uint32_t)c4)))
#else
#define MAKE_FOURC_TYPE(c1, c2, c3, c4) \
    ((((boost::uint32_t)c1)) | \
    (((boost::uint32_t)c2) << 8) | \
    (((boost::uint32_t)c3)) << 16 | \
    (((boost::uint32_t)c4) << 24))
#endif

        static const boost::uint32_t MEDIA_TYPE_VIDE = MAKE_FOURC_TYPE('V', 'I', 'D', 'E');
        static const boost::uint32_t MEDIA_TYPE_AUDI = MAKE_FOURC_TYPE('A', 'U', 'D', 'I');
        static const boost::uint32_t MEDIA_TYPE_NONE = 0;

        static const boost::uint32_t VIDEO_TYPE_AVC1 = MAKE_FOURC_TYPE('A', 'V', 'C', '1');
        static const boost::uint32_t VIDEO_TYPE_MP4V = MAKE_FOURC_TYPE('M', 'P', '4', 'V');
        static const boost::uint32_t VIDEO_TYPE_NONE = 0;

        static const boost::uint32_t AUDIO_TYPE_MP4A = MAKE_FOURC_TYPE('M', 'P', '4', 'A');
        static const boost::uint32_t AUDIO_TYPE_MP1A = MAKE_FOURC_TYPE('M', 'P', '1', 'A');
        static const boost::uint32_t AUDIO_TYPE_WMA2 = MAKE_FOURC_TYPE('W', 'M', 'A', '2');
        static const boost::uint32_t AUDIO_TYPE_NONE = 0;

        struct VideoInfo
        {
            boost::uint32_t width;
            boost::uint32_t height;
            boost::uint32_t frame_rate;
        };

        struct AudioInfo
        {
            boost::uint32_t channel_count;
            boost::uint32_t sample_size;
            boost::uint32_t sample_rate;
        };

        struct StreamInfoBase
        {
            StreamInfoBase()
                : type(MEDIA_TYPE_NONE)
                , sub_type(VIDEO_TYPE_NONE)
                , time_scale(0)
                , duration(0)
                , codec(NULL)
            {
            }

            union {
                boost::uint32_t type;
                char type_char[4];
            };
            union {
                boost::uint32_t sub_type;
                char sub_type_char[4];
            };
            boost::uint32_t index;
            boost::uint32_t time_scale;
            boost::uint64_t start_time;
            boost::uint64_t duration;
            boost::intrusive_ptr<Codec> codec;
        };

        struct StreamInfo
            : StreamInfoBase
        {
            enum FormatTypeEnum
            {
                none, 
                video_avc_packet = 1, 
                video_avc_byte_stream = 2, 
                audio_microsoft_wave = 3, 
                audio_iso_mp4 = 4, 
                audio_aac_adts = 5, 
            };

            StreamInfo()
                : format_type(none)
            {
            }

            boost::uint32_t format_type;             // 格式说明的类型
            union {
                VideoInfo video_format;
                AudioInfo audio_format;
            };
            std::vector<boost::uint8_t> format_data; // 格式说明的内容
        };

        struct FileBlock
        {
            FileBlock(boost::uint64_t o, boost::uint32_t s)
                : offset(o)
                , size(s)
            {
            }

            boost::uint64_t offset;
            boost::uint32_t size;
        };

        struct Sample
        {
            Sample()
            :flags(0)
            ,us_delta(boost::uint32_t(-1))
            {
            }
            enum FlagEnum
            {
                 sync = 1, 
                 discontinuity = 2,
            };

            boost::uint32_t itrack;
            boost::uint32_t flags;
            boost::uint64_t time;
            boost::uint64_t ustime;
            boost::uint64_t dts;
            boost::uint32_t us_delta; //pts-dts
            boost::uint32_t cts_delta;
            boost::uint32_t duration;
            boost::uint32_t size;
            StreamInfo const * stream_info;
            void * context; // 处理上下文
            std::vector<FileBlock> blocks;
            std::deque<boost::asio::const_buffer> data;
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_FORMAT_H_
