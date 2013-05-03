// FlvFormat.h

#ifndef _PPBOX_AVFORMAT_FLV_FLV_FORMAT_H_
#define _PPBOX_AVFORMAT_FLV_FLV_FORMAT_H_

#include "ppbox/avformat/stream/FormatBuffer.h"

#include <util/archive/BigEndianBinaryIArchive.h>
#include <util/archive/BigEndianBinaryOArchive.h>

namespace ppbox
{
    namespace avformat
    {

        struct FlvFlagType
        {
            enum Enum {
                HASVIDEO = 1,
                HASAUDIO = 4,
            };
        };

        struct FlvTagType
        {
            enum Enum {
                AUDIO = 0x08,
                VIDEO = 0x09,
                DATA  = 0x12,
            };
        };

        struct FlvSoundType
        {
            enum Enum {
                FLV_MONO   = 0,
                FLV_STEREO = 1,
            };
        };

        struct SoundSizeType
        {
            enum Enum {
                _8BIT  = 0,
                _16BIT = 1,
            };
        };

        struct FlvSoundSampleRateType
        {
            enum Enum {
                _SPECIAL = 0, 
                _11025HZ = 1,
                _22050HZ = 2,
                _44100HZ = 3,
            };
        };

        struct FlvSoundCodec
        {
            enum Enum {
                PCM                  = 0,
                ADPCM                = 1,
                MP3                  = 2,
                PCM_LE               = 3,
                NELLYMOSER_16KHZ_MONO = 4,
                NELLYMOSER_8KHZ_MONO = 5,
                NELLYMOSER           = 6,
                AAC                  = 10,
                SPEEX                = 11,
            };
        };

        struct FlvVideoCodec
        {
            enum Enum {
                H263    = 2,
                SCREEN  = 3,
                VP6     = 4,
                VP6A    = 5,
                SCREEN2 = 6,
                H264    = 7,
            };
        };

        struct FlvFrameType
        {
            enum Enum {
                KEY        = 1,
                INTER      = 2,
                DISP_INTER = 3,
            };
        };

        typedef util::archive::BigEndianBinaryIArchive<boost::uint8_t> FlvIArchive;
        typedef util::archive::BigEndianBinaryOArchive<boost::uint8_t> FlvOArchive;

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_FLV_FLV_FORMAT_H_
