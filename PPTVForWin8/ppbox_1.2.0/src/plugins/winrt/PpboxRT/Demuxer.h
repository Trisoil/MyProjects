#pragma once

using namespace Platform;
using namespace Windows::Foundation::Collections;

struct PPBOX_StreamInfoEx;
struct PPBOX_SampleEx2;

namespace PpboxRT
{

	public enum class Error: int
	{
		success = 0, 
		not_start, 
		already_start, 
		not_open, 
		already_open, 
		operation_canceled, 
		would_block, 
		stream_end, 
		logic_error, 
		network_error, 
		demux_error, 
		certify_error, 
		other_error = 1024, 
	};

	public enum class StreamType: int
    {
        video = 1, 
        audio = 2, 
    };

	public enum class StreamSubType: int
    {
        video_avc = 1, 
        audio_aac = 2, 
        audio_mp3 = 3, 
        audio_wma = 4,
    };

    public value struct VideoInfo
    {
        uint32 width;
		uint32 height;
        uint32 frame_rate;
    };

    public value struct AudioInfo
    {
        uint32 channel_count;
        uint32 sample_size;
        uint32 sample_rate;
    };

	class Access;

	public delegate void Callback(Error);

	public ref class Stream sealed
	{
	public:
		property StreamType type { StreamType get() { return type_; } }

		property StreamSubType sub_type { StreamSubType get() { return sub_type_; } }

		property VideoInfo video { VideoInfo get() { return video_; } }

		property AudioInfo audio { AudioInfo get() { return audio_; } }

		property Array<uint8> ^ codec_data { Array<uint8> ^ get() { return codec_data_; } }

	private:
		Stream(
			PPBOX_StreamInfoEx & stream);

		friend class Access;

	private:
		StreamType type_;
		StreamSubType sub_type_;

		VideoInfo video_;
		AudioInfo audio_;

		Array<uint8> ^ codec_data_;
	};

	public ref class Media sealed
	{
	public:
		property uint64 duration { uint64 get() { return duration_; }}

		property Array<Stream ^> ^ streams { Array<Stream ^> ^ get() { return streams_; }}

	private:
		Media(
			uint64 duration, 
			Array<Stream ^> ^ streams);

		friend class Access;

	private:
		uint64 duration_;
		Array<Stream ^> ^ streams_;
	};

	public ref class Sample sealed
	{
	public:
		property uint32 index { uint32 get() { return index_; } }

		property uint64 time { uint64 get() { return time_; } }

		property bool is_sync { bool get() { return (flag_ & sync) != 0; } }

		property bool is_discontinue { bool get() { return (flag_ & discontinue) != 0; } }

		property Array<uint8> ^ data { Array<uint8> ^ get() { return data_; } }

	private:
		Sample(
			PPBOX_SampleEx2 & sample);

		friend class Access;

	private:
		enum
		{
			sync = 1, 
			discontinue = 2, 
		};
		
        uint32 index_;
        uint32 flag_;
        uint64 time_;
		Array<uint8> ^ data_;
	};

    public ref class Demuxer sealed
    {
    public:
        Demuxer();

        virtual ~Demuxer();

    public:
		void async_open(
			String ^ playlink, 
			String ^ config, 
			Callback ^ callback);

		Error get_media(
			Media ^* media);

		Error seek(
			uint64 time);

		Error get_sample(
			Sample ^* sample);

		void close();

	private:
		static void s_call_back(
			void * ctx, 
			long ec);

	private:
		Demuxer ^ self_ref_;
		Callback ^ callback_;
        CRITICAL_SECTION mutex_;
    };

}