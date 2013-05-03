// IDispatch.h

#ifndef _PPBOX2_PPBOX2_I_DISPATCH_H_
#define _PPBOX2_PPBOX2_I_DISPATCH_H_

#include "IPpbox.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

    //设置下载缓冲区大小 （只能在Open前调用）
    // 主要用于控制内存，如果下载速度大于ReadSample的速度，那么下载的数据将存放
    // 于内存之中，当内存中的下载缓冲大于这个预设值，那么将停止下载。直到被调用了
    // ReadSample，少了一些内存占用后，再继续下载，
    // length: 预设的下载内存缓冲的大小
    PPBOX_DECL void PPBOX2_SetBufferSize(
        PP_uint32 length);

    //打开一个视频
    PPBOX_DECL PPBOX_HANDLE PPBOX2_Open(
        PP_char const * playlink, 
        PP_char const * config);

    typedef void (*PPBOX2_Open_Callback)(PP_err);

    PPBOX_DECL PPBOX_HANDLE PPBOX2_AsyncOpen(
        PP_char const * playlink, 
        PP_char const * config, 
        PPBOX2_Open_Callback callback);

    //获得有多少流
    PPBOX_DECL PP_uint32 PPBOX2_GetStreamCount(
        PPBOX_HANDLE handle);

    enum PPBOX2_StreamTypeEnum
    {
        ppbox2_video = 1, 
        ppbox2_audio = 2, 
    };

    enum PPBOX2_StreamSubTypeEnum
    {
        ppbox2_video_avc = 1, 
        ppbox2_audio_aac = 2, 
        ppbox2_audio_mp3 = 3, 
        ppbox2_audio_wma = 4,
    };

    enum PPBOX2_StreamFormatTypeEnum
    {
        ppbox2_video_avc_packet = 1, 
        ppbox2_video_avc_byte_stream = 2, 
        ppbox2_audio_microsoft_wave = 3, 
        ppbox2_audio_iso_mp4 = 4, 
    };

    typedef struct tag_PPBOX2_VideoInfo
    {
        PP_uint32 width;
        PP_uint32 height;
        PP_uint32 frame_rate;
    } PPBOX2_VideoInfo;

    typedef struct tag_PPBOX2_AudioInfo
    {
        PP_uint32 channel_count;
        PP_uint32 sample_size;
        PP_uint32 sample_rate;
    } PPBOX2_AudioInfo;

    typedef struct tag_PPBOX2_StreamInfo
    {
        PP_int32 type;          // TypeEnum
        PP_int32 sub_type;      // SubTypeEnum
        PP_uint32 time_scale;   // 时间精度
        union 
        {
            PPBOX2_VideoInfo video_format;
            PPBOX2_AudioInfo audio_format;
        };
        PP_int32 format_type;   // 格式说明的类型
        PP_uint32 format_size;  // 格式说明的大小
        PP_uchar const * format_buffer;   // 不同的解码不同的结构体，格式说明的的内容
    } PPBOX2_StreamInfo;

    //获得流的详细扩展信息
    PPBOX_DECL PP_err PPBOX2_GetStreamInfo(
        PPBOX_HANDLE handle, 
        PP_uint32 index,
        PPBOX2_StreamInfo *info);

    //获得总时长
    PPBOX_DECL PP_uint64 PPBOX2_GetDuration(
        PPBOX_HANDLE handle);

    //暂停
    //PPBOX_DECL void PPBOX2_AsyncDispatch(
    //    PPBOX_HANDLE handle, 
    //    );

    PPBOX_DECL PP_err PPBOX2_Pause(
        PPBOX_HANDLE handle);

    //暂停
    PPBOX_DECL PP_err PPBOX2_Resume(
        PPBOX_HANDLE handle);

    enum PPBOX2_SeekTypeEnum
    {
    };

    //跳到某个时刻开始播放
    PPBOX_DECL PP_err PPBOX2_Seek(
        PPBOX_HANDLE handle, 
        PP_int32 type, 
        PP_uint64 start, 
        PP_uint64 end);

    enum PPBOX2_SampleFlagEnum
    {
        ppbox2_sample_sync = 1, 
    };

    typedef struct tag_PPBOX2_Sample
    {
        PP_uint32 stream_index;     // 流的编号
        PP_uint32 flags;            // Sample flags
        PP_uint64 start_time;       // Sample对应的时间戳, 单位是微妙
        PP_uint64 decode_time;      // Sample对应的时间戳, PPBOX2_StreamInfo 中指示的 time scale 精度
        PP_uint32 composite_time_delta; // PPBOX2_StreamInfo 中指示的 time scale 精度
        PP_uint32 duration;         // PPBOX2_StreamInfo 中指示的 time scale 精度
        PP_uint32 buffer_length;    // Sample的大小
        PP_uchar const * buffer;    // Sample的内容
        //当调用本函数的后，我们申请内存然后给他Sample
        //当调用下一个ReadSample函数的时候，或者Stop的时候，内存释放掉
    } PPBOX2_Sample;

    PPBOX_DECL PP_err PPBOX2_ReadSample(
        PPBOX_HANDLE handle, 
        PPBOX2_Sample * sample);

    typedef struct tag_PPBOX2_StreamStatus
    {
        PP_uint32 buffer_time;      //下载缓冲区数据的总时间
    } PPBOX2_StreamStatus;

    //获得播放信息
    //返回值: 错误码
    //    ppbox2_success      表示成功
    //    其他数值表示失败
    PPBOX_DECL PP_err PPBOX2_GetStreamStatus(
        PPBOX_HANDLE handle, 
        PPBOX2_StreamStatus * status);

    typedef struct tag_PPBOX2_BufferStatus
    {
        PP_uint32 total;    // 总共下载的字节数
        PP_uint32 speed;    // 总共上传时字节数
    } PPBOX2_BufferStatus;

    //获得下载信息
    // download_Msg: 调用这个接口后，用于获取的下载信息数据
    //返回值: 错误码
    //    ppbox2_success      表示成功
    //    其他数值表示失败
    PPBOX_DECL PP_err PPBOX2_GetBufferStatus(
        PPBOX_HANDLE handle, 
        PPBOX2_BufferStatus * status);

    //强制结束
    PPBOX_DECL void PPBOX2_Close(
        PPBOX_HANDLE handle);

#if __cplusplus
}
#endif // __cplusplus

#endif // _PPBOX2_PPBOX2_I_DISPATCH_H_
