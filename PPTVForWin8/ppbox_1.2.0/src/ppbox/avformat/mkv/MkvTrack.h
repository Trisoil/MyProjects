// MkvTrack.h

#ifndef _PPBOX_AVFORMAT_MKV_MKV_TRACK_H_
#define _PPBOX_AVFORMAT_MKV_MKV_TRACK_H_

#include "ppbox/avformat/mkv/MkvContentEncoding.h"

namespace ppbox
{
    namespace avformat
    {

        struct MkvTrackType
        {
            enum Enum
            {
                VIDEO       = 0x01, 
                AUDIO       = 0x02, 
                SUBTITLE    = 0x11, 
            };
        };

        // Video: child of TrackEntry

        struct MkvVideoData
        {
            EBML_UInt PixelWidth;
            EBML_UInt PixelHeight;
            EBML_UInt PixelCropBottom;
            EBML_UInt PixelCropTop;
            EBML_UInt PixelCropLeft;
            EBML_UInt PixelCropRight;
            EBML_UInt DisplayWidth;
            EBML_UInt DisplayHeight;
            EBML_UInt DisplayUnit;

            MkvVideoData()
                : PixelWidth(0x30)
                , PixelHeight(0x3A)
                , PixelCropBottom(0x14AA)
                , PixelCropTop(0x14BB)
                , PixelCropLeft(0x14CC)
                , PixelCropRight(0x14DD)
                , DisplayWidth(0x14B0)
                , DisplayHeight(0x14BA)
                , DisplayUnit(0x14B2)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & PixelWidth
                    & PixelHeight
                    & PixelCropBottom
                    & PixelCropTop
                    & PixelCropLeft
                    & PixelCropRight
                    & DisplayWidth
                    & DisplayHeight
                    & DisplayUnit;
            }
        };

        typedef EBML_Element<0x60, MkvVideoData> MkvVideo;

        // Audio: child of TrackEntry

        struct MkvAudioData
        {
            EBML_Float SamplingFrequency;
            EBML_UInt OutputSamplingFrequency;
            EBML_UInt Channels;
            EBML_UInt BitDepth;

            MkvAudioData()
                : SamplingFrequency(0x35)
                , OutputSamplingFrequency(0x38B5)
                , Channels(0x1F)
                , BitDepth(0x2264)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SamplingFrequency
                    & OutputSamplingFrequency
                    & Channels
                    & BitDepth;
            }
        };

        typedef EBML_Element<0x61, MkvAudioData> MkvAudio;

        // TrackEntry: child of Tracks

        struct MkvTrackEntryData
        {
            EBML_UInt TrackNumber;
            EBML_Binary TrackUID;
            EBML_UInt TrackType;
            EBML_Bool FlagEnable;
            EBML_Bool FlagDefault;
            EBML_Bool FlagForced;
            EBML_Bool FlagLacing;
            EBML_UInt MinCache;
            EBML_UInt MaxCache;
            EBML_UInt DefaultDuration;
            EBML_Float TrackTimeCodeScale;
            EBML_String Name;
            EBML_String Language;
            EBML_String CodecID; 
            EBML_Binary CodecPrivate;
            EBML_String CodecName;
            EBML_UInt AttachmentLink;

            MkvVideo Video;
            MkvAudio Audio;
            MkvContentEncodings ContentEncodings;

            MkvTrackEntryData()
                : TrackNumber(0x57)
                , TrackUID(0x33C5)
                , TrackType(0x3)
                , FlagEnable(0x39)
                , FlagDefault(0x8)
                , FlagForced(0x15AA)
                , FlagLacing(0x1C)
                , MinCache(0x2DE7)
                , MaxCache(0x2DF8)
                , DefaultDuration(0x3E383)
                , TrackTimeCodeScale(0x3314F)
                , Name(0x136E)
                , Language(0x2B59C)
                , CodecID(0x6)
                , CodecPrivate(0x23A2)
                , CodecName(0x58688)
                , AttachmentLink(0x3446)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & TrackNumber
                    & TrackUID
                    & TrackType
                    & FlagEnable
                    & FlagDefault
                    & FlagForced
                    & FlagLacing
                    & MinCache
                    & MaxCache
                    & DefaultDuration
                    & TrackTimeCodeScale
                    & Name
                    & Language
                    & CodecID
                    & CodecPrivate
                    & CodecName
                    & AttachmentLink
                    & Video
                    & Audio
                    & ContentEncodings;
            }
        };

        typedef EBML_Element<0x2E, MkvTrackEntryData> MkvTrackEntry;

        // Tracks: child of Segment

        struct MkvTracksData
        {
            std::vector<MkvTrackEntry> Tracks;

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & Tracks;
            }
        };

        typedef EBML_Element<0x654AE6B, MkvTracksData> MkvTracks;

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_MKV_MKV_TRACK_H_
