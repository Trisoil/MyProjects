// AacConfig.h

#ifndef _PPBOX_AVFORMAT_CODEC_AAC_AAC_CONFIG_H_
#define _PPBOX_AVFORMAT_CODEC_AAC_AAC_CONFIG_H_

#include "ppbox/avformat/stream/BitsType.h"
#include "ppbox/avformat/codec/aac/AacGAConfig.h"

namespace ppbox
{
    namespace avformat
    {

        // AudioSpecificConfig

        struct AacConfig
        {
        public:
            U<5> audioObjectType;
            U<6> audioObjectTypeExt; // if audioObjectType == 31 audioObjectType = 32 + audioObjectTypeExt
            U<4> samplingFrequencyIndex;
            U<24> samplingFrequency; // if samplingFrequencyIndex == 15
            U<4> channelConfiguration;

            //sbrPresentFlag = -1;
            //if ( audioObjectType == 5 ) {
            //    extensionAudioObjectType = audioObjectType;
            //    sbrPresentFlag = 1;
            //    extensionSamplingFrequencyIndex; 4 uimsbf
            //        if ( extensionSamplingFrequencyIndex == 0xf )
            //            extensionSamplingFrequency; 24 uimsbf
            //            audioObjectType = GetAudioObjectType();
            //}
            //else {
            //    extensionAudioObjectType = 0;
            //}

            //if ( audioObjectType == 5 ) {
                U<4> extensionSamplingFrequencyIndex;
                U<24> extensionSamplingFrequency; // if extensionSamplingFrequencyIndex = 0xf
                U<5> extensionAudioObjectType;
                U<6> extensionAudioObjectTypeExt; // if extensionAudioObjectType == 31 extensionAudioObjectTypeExt = 32 + extensionAudioObjectType
            //}

            AacGAConfig ga_config;

            U<11> syncExtensionType;
            U<5> syncExtensionAudioObjectType;
            U<6> syncExtensionAudioObjectTypeExt;
            U<1> syncSbrPresentFlag;
            U<4> syncExtensionSamplingFrequencyIndex;
            U<24> syncExtensionSamplingFrequency;

            U<11> syncExtensionType2;
            U<1> syncPsPresentFlag;

        public:
            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & audioObjectType;
                if (audioObjectType == 31)
                    ar & audioObjectTypeExt;
                ar & samplingFrequencyIndex;
                if (samplingFrequencyIndex == 0xf)
                    ar & samplingFrequency;
                ar & channelConfiguration;
                if (audioObjectType == 5 || audioObjectType == 29) {
                    ar & extensionSamplingFrequencyIndex;
                    if (extensionSamplingFrequencyIndex == 0xf) {
                        ar & extensionSamplingFrequency;
                    }
                    ar & extensionAudioObjectType;
                    if (extensionAudioObjectType == 31)
                        ar & extensionAudioObjectTypeExt;
                }
                ar & ga_config;

                typename Archive::is_loading load;
                if (ar && audioObjectType != 5 && (load || syncExtensionType)) {
                    ar & syncExtensionType;
                    if (syncExtensionType == 0x2b7) {
                        ar & syncExtensionAudioObjectType;
                        if (syncExtensionAudioObjectType == 31) {
                            ar & syncExtensionAudioObjectTypeExt;
                        }
                        if (syncExtensionAudioObjectType == 5) {
                            ar & syncSbrPresentFlag;
                            if (syncSbrPresentFlag) {
                                ar & syncExtensionSamplingFrequencyIndex;
                                if (syncExtensionSamplingFrequencyIndex == 0xf)
                                    ar & syncExtensionSamplingFrequency;
                                if (ar) {
                                    ar & syncExtensionType2;
                                    if (syncExtensionType2 == 0x548) {
                                        ar & syncPsPresentFlag;
                                    } else if (syncExtensionType2 == 0) {
                                        ar.clear();
                                    }
                                } // 
                            }
                        }
                    } else if (syncExtensionType == 0) {
                        ar.clear();
                    }
                }
            };
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_CODEC_AAC_AAC_CONFIG_H_
