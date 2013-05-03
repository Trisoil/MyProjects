// MkvSegment.h

#ifndef _PPBOX_AVFORMAT_MKV_MKV_SEGMENT_H_
#define _PPBOX_AVFORMAT_MKV_MKV_SEGMENT_H_

#include "ppbox/avformat/mkv/MkvTrack.h"
#include "ppbox/avformat/mkv/MkvCluster.h"

namespace ppbox
{
    namespace avformat
    {

        typedef EBML_OArchive MkvOArchive;

        typedef EBML_IArchive MkvIArchive;

        // SegmentInfo: child of Segment

        struct MkvSegmentInfoData
        {
            EBML_Binary Segment_UID;
            EBML_String Segment_File_Name;
            EBML_Binary Prev_UID;
            EBML_String Prev_File_Name;
            EBML_Binary Next_UID;
            EBML_String Next_File_Name;
            EBML_UInt Time_Code_Scale;
            EBML_Float Duration;
            EBML_String Title;
            EBML_String Muxing_App;
            EBML_String Writing_App;
            EBML_UInt Date_UTC;

            MkvSegmentInfoData()
                : Segment_UID(0x33A4)
                , Segment_File_Name(0x3384)
                , Prev_UID(0x1CB923)
                , Prev_File_Name(0x1C83AB)
                , Next_UID(0x1EB923)
                , Next_File_Name(0x1E83BB)
                , Time_Code_Scale(0xAD7B1)
                , Duration(0x489)
                , Title(0x3BA9)
                , Muxing_App(0xD80)
                , Writing_App(0x1741)
                , Date_UTC(0x461)
            {
            }

            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & Segment_UID
                    & Segment_File_Name
                    & Prev_UID
                    & Prev_File_Name
                    & Next_UID
                    & Next_File_Name
                    & Time_Code_Scale
                    & Duration
                    & Title
                    & Muxing_App
                    & Writing_App
                    & Date_UTC;
            }
        };

        typedef EBML_Element<0x549A966, MkvSegmentInfoData> MkvSegmentInfo;

        // Segment: top level

        struct MkvSegmentData
        {
            MkvSegmentInfo SegmentInfo;
            MkvTracks Tracks;
            std::vector<MkvCluster> Clusters;

            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & SegmentInfo;
                ar & Tracks;
                ar & Clusters;
            }
        };

        typedef EBML_Element<0x8538067, MkvSegmentData> MkvSegment;

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_MKV_MKV_SEGMENT_H_
