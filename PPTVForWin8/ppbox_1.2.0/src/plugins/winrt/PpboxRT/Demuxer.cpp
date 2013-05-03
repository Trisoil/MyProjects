// Demuxer.cpp

#include <windows.h>

#include "Demuxer.h"

#include <plugins/ppbox/ppbox.h>

using namespace PpboxRT;
using namespace Platform;

namespace PpboxRT
{
	class Access
	{
	public:
		static Stream ^ new_stream(
			PPBOX_StreamInfoEx & stream)
		{
			return ref new Stream(stream);
		}

		static Media ^ new_media(
			uint64 duration, 
			Array<Stream ^> ^ streams)
		{
			return ref new Media(duration, streams);
		}

		static Sample ^ new_sample(
			PPBOX_SampleEx2 & sample)
		{
			return ref new Sample(sample);
		}
	};
}

Stream::Stream(
	PPBOX_StreamInfoEx & info)
{
	type_ = (StreamType)info.type;
	sub_type_ = (StreamSubType)info.sub_type;
	if (type_ == StreamType::video) {
		video_.width = info.video_format.width;
		video_.height = info.video_format.height;
		video_.frame_rate = info.video_format.frame_rate;
	} else {
		audio_.channel_count = info.audio_format.channel_count;
		audio_.sample_rate = info.audio_format.sample_rate;
		audio_.sample_size = info.audio_format.sample_size;
	}
	codec_data_ = ref new Platform::Array<uint8>((uint8 *)info.format_buffer, info.format_size);
}

Media::Media(
	uint64 duration, 
	Array<Stream ^> ^ streams)
	: duration_(duration)
	, streams_(streams)
{
}

Sample::Sample(
	PPBOX_SampleEx2 & sample)
{
	index_ = sample.stream_index;
	flag_ = 0;
	if (sample.is_sync) {
		flag_ |= sync;
	}
	if (sample.is_discontinuity) {
		flag_ |= discontinue;
	}
	time_ = sample.decode_time + sample.composite_time_delta;
	data_ = ref new Platform::Array<uint8>((uint8 *)sample.buffer, sample.buffer_length);
}

Demuxer::Demuxer()
{
    InitializeCriticalSectionEx(&mutex_, 100, 0);
}

Demuxer::~Demuxer()
{
    DeleteCriticalSection(&mutex_);
}

void Demuxer::async_open(
	String ^ playlink, 
	String ^ config, 
	Callback ^ callback)
{
	char playlinkc[MAX_PATH];
	char configc[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, playlink->Data(), -1, playlinkc, MAX_PATH, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, config->Data(), -1, configc, MAX_PATH, 0, 0);

	self_ref_ = this;
	callback_ = callback;
	PPBOX_AsyncOpenEx(playlinkc, configc, &self_ref_, s_call_back);
}

Error Demuxer::get_media(
	Media ^* media)
{
	PP_uint64 d = PPBOX_GetDuration();
	PP_uint32 n = PPBOX_GetStreamCount();
	Array<Stream ^> ^ streams = ref new Platform::Array<Stream ^>(n);
	for (PP_uint32 i = 0; i < n; ++i) {
		PPBOX_StreamInfoEx info;
		PPBOX_GetStreamInfoEx(i, &info);
		Stream ^ s = Access::new_stream(info);
		streams->set(i, s);
	}
	*media = Access::new_media(d * 10000, streams);
	return Error::success;
}

Error Demuxer::seek(
	uint64 time)
{
    EnterCriticalSection(&mutex_);
	PP_err ec = PPBOX_Seek((PP_uint32)(time / 10000));
    LeaveCriticalSection(&mutex_);
	return (Error)ec;
}

Error Demuxer::get_sample(
	Sample ^* sample)
{
    EnterCriticalSection(&mutex_);
	PPBOX_SampleEx2 s;
	PP_err ec = PPBOX_ReadSampleEx2(&s);
	if (ec == 0) {
		*sample = Access::new_sample(s);
	}
    LeaveCriticalSection(&mutex_);
	return (Error)ec;
}

void Demuxer::close()
{
	PPBOX_Close();
}

void Demuxer::s_call_back(
	void * ctx, 
	long ec)
{
	Demuxer ^ inst = *(Demuxer ^ *)ctx;
	inst->self_ref_ = nullptr;

	inst->callback_->Invoke((Error)ec);
	inst->callback_ = nullptr;
}
