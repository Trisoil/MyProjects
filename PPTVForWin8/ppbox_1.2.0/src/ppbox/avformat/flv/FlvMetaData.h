// FlvMetadata.h

#ifndef _PPBOX_AVFORMAT_FLV_FLV_META_DATA_H_
#define _PPBOX_AVFORMAT_FLV_FLV_META_DATA_H_

#include "ppbox/avformat/flv/FlvDataType.h"

namespace ppbox
{
    namespace avformat
    {

        struct FlvMetaData
        {
            bool hasaudio;
            bool hasvideo;
            std::string author;
            std::string copyright;
            std::string description;
            boost::uint32_t duration;
            boost::uint32_t datarate;
            boost::uint64_t livetime;
            boost::uint64_t timeshift;
            boost::uint32_t width;
            boost::uint32_t height;
            boost::uint32_t videodatarate;
            boost::uint32_t framerate;
            boost::uint32_t videocodecid;
            boost::uint32_t audiosamplerate;
            boost::uint32_t audiosamplesize;
            bool stereo;
            boost::uint32_t audiocodecid;
            boost::uint32_t filesize;
            std::vector<boost::uint8_t> BinaryData;

            FlvMetaData();

            void from_data(
                FlvDataValue const & data);

            void to_data(
                FlvDataValue & data);
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_FLV_FLV_META_DATA_H_
