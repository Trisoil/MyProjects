// AacAdts.h

#ifndef _PPBOX_AVFORMAT_CODEC_AAC_AAC_ADTS_H_
#define _PPBOX_AVFORMAT_CODEC_AAC_AAC_ADTS_H_

#include "ppbox/avformat/stream/BitsType.h"

namespace ppbox
{
    namespace avformat
    {

        /*
        Letter  Length (bits)  Description  
        A       12      syncword 0xFFF, all bits must be 1  
        B       1       MPEG Version: 0 for MPEG-4, 1 for MPEG-2  
        C       2       Layer: always 0  
        D       1       protection absent, Warning, set to 1 if there is no CRC and 0 if there is CRC  
        E       2       profile, the MPEG-4 Audio Object Type minus 1  
        F       4       MPEG-4 Sampling Frequency Index (15 is forbidden)  
        G       1       private stream, set to 0 when encoding, ignore when decoding  
        H       3       MPEG-4 Channel Configuration (in the case of 0, the channel configuration is sent via an inband PCE)  
        I       1       originality, set to 0 when encoding, ignore when decoding  
        J       1       home, set to 0 when encoding, ignore when decoding  
        K       1       copyrighted stream, set to 0 when encoding, ignore when decoding  
        L       1       copyright start, set to 0 when encoding, ignore when decoding  
        M       13      frame length, this value must include 7 or 9 bytes of header length: FrameLength = (ProtectionAbsent == 1 ? 7 : 9) + size(AACFrame)  
        O       11      Buffer fullness  
        P       2       Number of AAC frames (RDBs) in ADTS frame minus 1, for maximum compatibility always use 1 AAC frame per ADTS frame  
        Q       16      CRC if protection absent is 0  
         */

        struct AacAdts
        {
            U<12> syncword;
            U<1> version; // 0: MPEG-4 part 3, 1: MPEG-2 part 7
            U<2> layer;
            U<1> protection_absent; // crc
            U<2> profile;
            U<4> sampling_frequency_index;
            U<1> private_bit;
            U<3> channel_configuration;
            U<1> original;
            U<1> home;
            U<1> copyright_identification_bit;
            U<1> copyright_identification_start;
            U<13> frame_length;
            U<11> buffer_fullness;
            U<2> no_raw_data_blocks_in_frame;
            //U<16> crc_check;

            AacAdts()
                : syncword(0x0fff)
                , protection_absent(1)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & syncword
                    & version
                    & layer
                    & protection_absent
                    & profile
                    & sampling_frequency_index
                    & private_bit
                    & channel_configuration
                    & original
                    & home
                    & copyright_identification_bit
                    & copyright_identification_start
                    & frame_length
                    & buffer_fullness
                    & no_raw_data_blocks_in_frame
                    ;

                if (syncword != 0xfff) {
                    ar.fail();
                }
            }
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_CODEC_AAC_AAC_ADTS_H_
