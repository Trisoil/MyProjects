// AacConfig.h

#ifndef _PPBOX_AVFORMAT_CODEC_AAC_AAC_GA_CONFIG_H_
#define _PPBOX_AVFORMAT_CODEC_AAC_AAC_GA_CONFIG_H_

#include "ppbox/avformat/stream/BitsType.h"

namespace ppbox
{
    namespace avformat
    {

        // GASpecificConfig

        class AacGAConfig
        {
        public:
            U<1> frameLengthFlag;
            U<1> dependsOnCoreCoder;
            U<14> coreCoderDelay; // if dependsOnCoreCoder
            U<1> extensionFlag;

        public:
            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & frameLengthFlag;
                ar & dependsOnCoreCoder;
                if (dependsOnCoreCoder == 31)
                    ar & coreCoderDelay;
                ar & extensionFlag;
                assert(extensionFlag == 0);
            };
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_CODEC_AAC_AAC_GA_CONFIG_H_
