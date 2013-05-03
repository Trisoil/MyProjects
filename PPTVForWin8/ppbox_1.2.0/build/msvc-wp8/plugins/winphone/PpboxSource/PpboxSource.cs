using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.Generic;

using PpboxRT;

namespace PpboxSource
{
    public class PpboxSource
        : System.Windows.Media.MediaStreamSource
    {
        public PpboxSource(
            String url)
        {
            this.demuxer_ = new Demuxer();
            this.url_ = url;
            this.pending_ = false;
            this.mediaStreamTypes_ = new List<MediaStreamType>();
            this.mediaStreamDescriptions_ = new Dictionary<MediaStreamType, MediaStreamDescription>();
            this.mediaStreamSamples_ = new Dictionary<MediaStreamType, List<MediaStreamSample>>();
        }

        protected override void OpenMediaAsync()
        {
            demuxer_.close();
            demuxer_.async_open(url_,
                "format=raw&demux.Source.time_out=0&mux.RawMuxer.video_format=es&mux.RawMuxer.time_scale=10000000", // &mux.TimeScale.time_adjust_mode=2
                this.OpenMediaCallback);
        }

        protected override void GetDiagnosticAsync(
            MediaStreamSourceDiagnosticKind diagnosticKind)
        {
            throw new NotImplementedException();
        }

        protected override void GetSampleAsync(
            MediaStreamType mediaStreamType)
        {
            while (mediaStreamSamples_[mediaStreamType].Count == 0)
            {
                Sample sample;
                Error ec = demuxer_.get_sample(out sample);
                if (ec == Error.success)
                {
                    Dictionary<MediaSampleAttributeKeys, string> mediaSampleAttributes =
                        new Dictionary<MediaSampleAttributeKeys, string>();
                    if (sample.is_sync)
                    {
                        mediaSampleAttributes[MediaSampleAttributeKeys.KeyFrameFlag] = bool.TrueString;
                    }
                    MediaStreamType type = mediaStreamTypes_[(int)sample.index];
                    MediaStreamSample sample2 = new MediaStreamSample(
                        mediaStreamDescriptions_[type],
                        new System.IO.MemoryStream(sample.data),
                        0,
                        sample.data.Length,
                        (long)sample.time,
                        mediaSampleAttributes);
                    mediaStreamSamples_[type].Add(sample2);
                }
                else
                {
                    if (ec == Error.would_block)
                    {
                        if (pending_)
                        {
                            System.Threading.Thread.Sleep(100);
                            continue;
                        }
                        else
                        {
                            pending_ = true;
                            System.Threading.ThreadPool.QueueUserWorkItem(
                                GetSamplePending, mediaStreamType);
                        }
                    }
                    else if (ec == Error.stream_end)
                    {
                        ReportGetSampleCompleted(null);
                    }
                    else
                    {
                        ErrorOccurred(ec.ToString());
                    }
                    return;
                }
            }

            pending_ = false;

            MediaStreamSample sample3 = mediaStreamSamples_[mediaStreamType][0];
            mediaStreamSamples_[mediaStreamType].RemoveAt(0);
            ReportGetSampleCompleted(sample3);
        }

        protected override void SeekAsync(
            long seekToTime)
        {
            demuxer_.seek((ulong)seekToTime);
            ReportSeekCompleted(seekToTime);
        }

        protected override void SwitchMediaStreamAsync(
            MediaStreamDescription mediaStreamDescription)
        {
        }

        protected override void CloseMedia()
        {
            demuxer_.close();
        }

        private void OpenMediaCallback(
            Error ec)
        {
            if (ec != Error.success)
            {
                ErrorOccurred(ec.ToString());
                return;
            }

            Media media;
            demuxer_.get_media(out media);

            Dictionary<MediaSourceAttributesKeys, string> mediaSourceAttributes = 
                new Dictionary<MediaSourceAttributesKeys, string>();
            mediaSourceAttributes[MediaSourceAttributesKeys.Duration] =
                media.duration.ToString();
            mediaSourceAttributes[MediaSourceAttributesKeys.CanSeek] =
                (media.duration != ulong.MaxValue).ToString();

            List<MediaStreamDescription> mediaStreamDescriptions = 
                new List<MediaStreamDescription>();
            for (int i = 0; i < media.streams.Length; ++i)
            {
                Stream stream = media.streams[i];
                Dictionary<MediaStreamAttributeKeys, string> mediaStreamAttributes =
                    new Dictionary<MediaStreamAttributeKeys, string>();
                mediaStreamAttributes[MediaStreamAttributeKeys.CodecPrivateData] =
                    stream.codec_data.ToString();
                if (stream.type == StreamType.video)
                {
                    mediaStreamAttributes[MediaStreamAttributeKeys.VideoFourCC] =
                        FourCC[(int)stream.sub_type];
                    mediaStreamAttributes[MediaStreamAttributeKeys.Width] =
                        stream.video.width.ToString();
                    mediaStreamAttributes[MediaStreamAttributeKeys.Height] =
                        stream.video.height.ToString();
                    char[] CodecPrivateDataHex = new char[stream.codec_data.Length * 2];
                    int index = 0;
                    ToHexHelper(CodecPrivateDataHex, ref index, stream.codec_data); // ExtraData
                    //mediaStreamAttributes[MediaStreamAttributeKeys.CodecPrivateData] =
                    //    new String(CodecPrivateDataHex);
                    MediaStreamDescription videoStreamDescription =
                        new MediaStreamDescription(MediaStreamType.Video, mediaStreamAttributes);
                    mediaStreamDescriptions.Add(videoStreamDescription);
                    mediaStreamTypes_.Add(MediaStreamType.Video);
                    mediaStreamDescriptions_[MediaStreamType.Video] = videoStreamDescription;
                    mediaStreamSamples_[MediaStreamType.Video] = new List<MediaStreamSample>();
                    //ParseAvcConfig(videoStreamDescription, mediaStreamSamples_[MediaStreamType.Video], stream.codec_data);
                }
                else if (stream.type == StreamType.audio)
                {
                    char[] WaveFormatExHex = new char[9 * 4 + stream.codec_data.Length * 2];
                    int index = 0;
                    ToHexHelper(WaveFormatExHex, ref index, 2, 255); // FormatTag
                    ToHexHelper(WaveFormatExHex, ref index, 2, stream.audio.channel_count); // Channels
                    ToHexHelper(WaveFormatExHex, ref index, 4, stream.audio.sample_rate); // SamplesPerSec
                    ToHexHelper(WaveFormatExHex, ref index, 4, 0); // AverageBytesPerSecond
                    ToHexHelper(WaveFormatExHex, ref index, 2, 1); // BlockAlign
                    ToHexHelper(WaveFormatExHex, ref index, 2, stream.audio.sample_size); // BitsPerSample
                    ToHexHelper(WaveFormatExHex, ref index, 2, stream.codec_data.Length); // ExtraDataSize
                    ToHexHelper(WaveFormatExHex, ref index, stream.codec_data); // ExtraData
                    mediaStreamAttributes[MediaStreamAttributeKeys.CodecPrivateData] =
                        new String(WaveFormatExHex);
                    MediaStreamDescription audioStreamDescription = 
                        new MediaStreamDescription(MediaStreamType.Audio, mediaStreamAttributes);
                    mediaStreamDescriptions.Add(audioStreamDescription);
                    mediaStreamTypes_.Add(MediaStreamType.Audio);
                    mediaStreamDescriptions_[MediaStreamType.Audio] = audioStreamDescription;
                    mediaStreamSamples_[MediaStreamType.Audio] = new List<MediaStreamSample>();
                }
                else
                {
                    mediaStreamTypes_.Add(MediaStreamType.Script);
                }
            } // for

            ReportOpenMediaCompleted(mediaSourceAttributes, mediaStreamDescriptions);
        }

        private void GetSamplePending(
            object mediaStreamType)
        {
            GetSampleAsync((MediaStreamType)mediaStreamType);
        }

        private static void ToHexHelper(
            char[] chars, 
            ref int startIndex, 
            byte sizeOfField, 
            long fieldData)
        {
            for (int i = 0; i < sizeOfField; i++, fieldData >>= 8, startIndex += 2)
            {
                chars[startIndex] = DecToHexConv[((fieldData & 0xf0) >> 4)];
                chars[startIndex + 1] = DecToHexConv[fieldData & 0x0f];
            }
        }

        private static void ToHexHelper(
            char[] chars, 
            ref int startIndex, 
            byte[] bytesData)
        {
            for (int i = 0; i < bytesData.Length; i++, startIndex += 2)
            {
                chars[startIndex] = DecToHexConv[((bytesData[i] & 0xf0) >> 4)];
                chars[startIndex + 1] = DecToHexConv[bytesData[i] & 0x0f];
            }
        }

        private static void ParseAvcConfig(
            MediaStreamDescription stream, 
            List<MediaStreamSample> samples, 
            byte[] data)
        {
            System.IO.Stream ios = new System.IO.MemoryStream(data);
            ios.Seek(5, System.IO.SeekOrigin.Begin);
            int num_sps = ios.ReadByte() & 0x1f;
            for (int i = 0; i < num_sps; ++i)
            {
                int len_sps = (ios.ReadByte() << 8) | ios.ReadByte();
                byte[] sps = new byte[len_sps];
                ios.Read(sps, 0, len_sps);
                samples.Add(new MediaStreamSample(
                    stream, 
                    new System.IO.MemoryStream(sps), 
                    0, 
                    len_sps, 
                    0, 
                    new Dictionary<MediaSampleAttributeKeys, string>()));
            }
            int num_pps = ios.ReadByte();
            for (int i = 0; i < num_pps; ++i)
            {
                int len_pps = (ios.ReadByte() << 8) | ios.ReadByte();
                byte[] pps = new byte[len_pps];
                ios.Read(pps, 0, len_pps);
                samples.Add(new MediaStreamSample(
                    stream, 
                    new System.IO.MemoryStream(pps), 
                    0, 
                    len_pps, 
                    0, 
                    new Dictionary<MediaSampleAttributeKeys, string>()));
            }
        }

        private static readonly string[] FourCC = new string[]
        {
            "", 
            "H264",
        };
   
        private static readonly char[] DecToHexConv = new char[] 
        {
            '0',    /* 48 ascii */
            '1',    /* 49 */
            '2',    /* 50 */
            '3',    /* 51 */
            '4',    /* 52 */
            '5',    /* 53 */
            '6',    /* 54 */
            '7',    /* 55 */
            '8',    /* 56 */
            '9',    /* 57 */
            'A',    /* 65 ascii */
            'B',    /* 66 */
            'C',    /* 67 */
            'D',    /* 68 */
            'E',    /* 69 */
            'F'     /* 70 */
        };

        private Demuxer demuxer_;
        private String url_;
        private Boolean pending_;
        private List<MediaStreamType> mediaStreamTypes_;
        private Dictionary<MediaStreamType, MediaStreamDescription> mediaStreamDescriptions_;
        private Dictionary<MediaStreamType, List<MediaStreamSample>> mediaStreamSamples_;
    }
}
