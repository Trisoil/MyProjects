// AvcConfig.h

#ifndef _PPBOX_AVFORMAT_CODEC_AVC_AVC_CONFIG_H_
#define _PPBOX_AVFORMAT_CODEC_AVC_AVC_CONFIG_H_

#include "ppbox/avformat/stream/BitsType.h"

#include <util/serialization/Collection.h>

namespace ppbox
{
    namespace avformat
    {

        // AVC Decoder Configuration Record

        struct AvcConfig
        {
        public:
            U<8> configurationVersion;
            U<8> AVCProfileIndication;
            U<8> profile_compatibility;
            U<8> AVCLevelIndication;
            U<6> reserved;
            U<2> lengthSizeMinusOne;
            U<3> reserved2;
            U<5> numOfSequenceParameterSets;
            // for (i=0; i< numOfSequenceParameterSets; i++)
                std::vector<U<16> > sequenceParameterSetLength;
                std::vector<std::vector<boost::uint8_t> > sequenceParameterSetNALUnit;
            U<8> numOfPictureParameterSets;
                // for (i=0; i< numOfPictureParameterSets; i++)
                std::vector<U<16> > pictureParameterSetLength;
                std::vector<std::vector<boost::uint8_t> > pictureParameterSetNALUnit;

        public:
            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & configurationVersion;
                ar & AVCProfileIndication;
                ar & profile_compatibility;
                ar & AVCLevelIndication;
                ar & reserved;
                ar & lengthSizeMinusOne;
                ar & reserved2;
                ar & numOfSequenceParameterSets;
                sequenceParameterSetLength.resize(numOfSequenceParameterSets);
                sequenceParameterSetNALUnit.resize(numOfSequenceParameterSets);
                for (size_t i = 0; i < sequenceParameterSetLength.size(); ++i) {
                    ar & sequenceParameterSetLength[i];
                    sequenceParameterSetNALUnit[i].resize(sequenceParameterSetLength[i]);
                    util::serialization::serialize_collection(ar, sequenceParameterSetNALUnit[i], sequenceParameterSetNALUnit[i].size());
                }
                ar & numOfPictureParameterSets;
                pictureParameterSetLength.resize(numOfPictureParameterSets);
                pictureParameterSetNALUnit.resize(numOfPictureParameterSets);
                for (size_t i = 0; i < pictureParameterSetLength.size(); ++i) {
                    ar & pictureParameterSetLength[i];
                    pictureParameterSetNALUnit[i].resize(pictureParameterSetLength[i]);
                    util::serialization::serialize_collection(ar, pictureParameterSetNALUnit[i], pictureParameterSetNALUnit[i].size());
                }
            }
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_CODEC_AVC_AVC_CONFIG_H_
