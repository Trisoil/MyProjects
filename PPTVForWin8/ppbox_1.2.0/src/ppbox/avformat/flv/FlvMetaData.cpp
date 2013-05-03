// FlvMetadata.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/flv/FlvMetaData.h"

namespace ppbox
{
    namespace avformat
    {

        FlvMetaData::FlvMetaData()
            : hasaudio(true)
            , hasvideo(true)
            , duration(0)
            , datarate(0)
            , livetime(0)
            , timeshift(0)
            , width(0)
            , height(0)
            , videodatarate(0)
            , framerate(0)
            , videocodecid(0)
            , audiosamplerate(0)
            , audiosamplesize(0)
            , stereo(false)
            , audiocodecid(0)
            , filesize(0)
        {
        }

        void FlvMetaData::from_data(
            FlvDataValue const & data)
        {
            std::vector<FlvDataObjectProperty> const & variables = 
                (data.Type == FlvDataType::MIXEDARRAY) ? data.ECMAArray.Variables : data.Object.ObjectProperties;
            for (size_t i = 0; i < variables.size(); ++i) {
                FlvDataObjectProperty const & property = variables[i];
                if (property.PropertyName.StringData == "datarate") {
                    datarate = (boost::uint32_t)property.PropertyData.Double;
                }
                if (property.PropertyName.StringData == "width") {
                    width = (boost::uint32_t)property.PropertyData.Double;
                }
                if (property.PropertyName.StringData == "height") {
                    height = (boost::uint32_t)property.PropertyData.Double;
                }
                if (property.PropertyName.StringData == "framerate") {
                    framerate = (boost::uint32_t)property.PropertyData.Double;
                }
                if (property.PropertyName.StringData == "audiosamplerate") {
                    audiosamplerate = (boost::uint32_t)property.PropertyData.Double;
                }
                if (property.PropertyName.StringData == "audiosamplesize") {
                    audiosamplesize = (boost::uint32_t)property.PropertyData.Double;
                }
            }
        }

        void FlvMetaData::to_data(
            FlvDataValue & data)
        {
            if (data.Type != FlvDataType::MIXEDARRAY) {
                data.Type = FlvDataType::OBJECT;
            }
            std::vector<FlvDataObjectProperty> & variables = 
                (data.Type == FlvDataType::OBJECT) ? data.Object.ObjectProperties : data.ECMAArray.Variables;
            if (hasvideo) {
                variables.push_back(FlvDataObjectProperty("hasVideo", 1));
            }
            if (hasaudio) {
                variables.push_back(FlvDataObjectProperty("hasAudio", 1));
            }
            if (datarate > 0) {
                variables.push_back(FlvDataObjectProperty("datarate", datarate));
            }
            if (videocodecid > 0) {
                variables.push_back(FlvDataObjectProperty("videocodecid", videocodecid));
            }
            if (width > 0) {
                variables.push_back(FlvDataObjectProperty("width", width));
            }
            if (height > 0) {
                variables.push_back(FlvDataObjectProperty("height", height));
            }
            if (framerate > 0) {
                variables.push_back(FlvDataObjectProperty("framerate", framerate));
            }
            if (audiocodecid > 0) {
                variables.push_back(FlvDataObjectProperty("audiocodecid", audiocodecid));
            }
            if (audiosamplerate > 0) {
                variables.push_back(FlvDataObjectProperty("audiosamplerate", audiosamplerate));
            }
            if (audiosamplesize > 0) {
                variables.push_back(FlvDataObjectProperty("audiosamplesize", audiosamplesize));
            }
            if (duration > 0) {
                variables.push_back(FlvDataObjectProperty("duration", duration));
            }
        }

    } // namespace avformat
} // namespace ppbox
