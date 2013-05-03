/*
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define PPBOX_EXTERN
#include "ppbox/ppbox.h"

#include "m_option.h"
#include "mp_msg.h"
#include "input/input.h"
#include "stream/stream.h"
#include "demuxer.h"
#include "stheader.h"

typedef struct ppbox_demux_priv {
  uint64_t duration;
  uint32_t stream_count;
  demux_stream_t * streams[8];
} ppbox_demux_priv_t;

static demuxer_t* demux_ppbox_open(demuxer_t* demuxer) {
  ppbox_demux_priv_t * priv = (ppbox_demux_priv_t *)calloc(sizeof(ppbox_demux_priv_t), 1);
  priv->duration = PPBOX_GetDuration()();
  priv->stream_count = PPBOX_GetStreamCount()();

  mp_msg(MSGT_DEMUX, MSGL_ERR, "demux_ppbox_open duration %u\n", priv->duration);
  mp_msg(MSGT_DEMUX, MSGL_ERR, "demux_ppbox_open stream count %u\n", priv->stream_count);

  for (uint32_t idx = 0; idx < priv->stream_count; ++idx) {
    PPBOX_StreamInfoEx info;
    PPBOX_GetStreamInfoEx()(idx, &info);
    if (info.type == ppbox_video) {
      mp_msg(MSGT_DEMUX, MSGL_ERR, "video stream\n");
      sh_video_t* sh_video = new_sh_video(demuxer, idx);;
      BITMAPINFOHEADER *bih = calloc(sizeof(*bih) + info.format_size, 1);
      bih->biSize = sizeof(*bih) + info.format_size;
      bih->biCompression = mmioFOURCC('H', '2', '6', '4');
      bih->biWidth = info.video_format.width;
      bih->biHeight = info.video_format.height;
      memcpy(bih + 1, info.format_buffer, info.format_size);
      sh_video->format = bih->biCompression;
      sh_video->disp_w = info.video_format.width;
      sh_video->disp_h = info.video_format.height;
      sh_video->fps = info.video_format.frame_rate;
      sh_video->frametime = 1.0 / info.video_format.frame_rate;
      sh_video->bih = bih;
      demuxer->video->sh = sh_video;
      priv->streams[idx] = demuxer->video;
    } else if (info.type == ppbox_audio) {
      mp_msg(MSGT_DEMUX, MSGL_ERR, "audio stream\n");
#if STREAMTYPE_PPBOX == 21
      sh_audio_t* sh_audio = new_sh_audio(demuxer, idx);
#else
      sh_audio_t* sh_audio = new_sh_audio(demuxer, idx, NULL);
#endif
      WAVEFORMATEX *wf = calloc(sizeof(*wf) + info.format_size, 1);;
      wf->wFormatTag = 255;
      wf->nChannels = info.audio_format.channel_count;
      wf->nSamplesPerSec = info.audio_format.sample_rate;
      wf->wBitsPerSample = info.audio_format.sample_size;
      wf->cbSize = info.format_size;
      memcpy(wf + 1, info.format_buffer, info.format_size);
      sh_audio->wf = wf;
      sh_audio->format = mmioFOURCC('M', 'P', '4', 'A');
      sh_audio->samplerate = info.audio_format.sample_rate;
      sh_audio->samplesize = info.audio_format.sample_size;
      sh_audio->channels = info.audio_format.channel_count;
      demuxer->audio->sh = sh_audio;
      demuxer->audio->id = idx;
      priv->streams[idx] = demuxer->audio;
    }
  }
  demuxer->seekable = priv->duration > 0;
  demuxer->priv = priv;

  return demuxer;
}

static int demux_ppbox_fill_buffer(demuxer_t* demuxer, demux_stream_t *ds) {
  ppbox_demux_priv_t *priv = (ppbox_demux_priv_t *) demuxer->priv;
  PPBOX_SampleEx2 sample;
  PP_int32 ret;

  while (1) {
    ret = PPBOX_ReadSampleEx2()(&sample);
    if (ret == ppbox_success) {
      demux_packet_t *dp = new_demux_packet(sample.buffer_length);
      memcpy(dp->buffer, sample.buffer, sample.buffer_length);
      dp->pts = (double)(sample.start_time + sample.composite_time_delta) * 0.000001;
      ds_add_packet(priv->streams[sample.stream_index], dp);
      if (priv->streams[sample.stream_index] == ds)
        return 1;
    } else if (ret == ppbox_would_block) {
        if (mp_input_check_interrupt(100))
          return 0;
    } else if (ret == ppbox_stream_end) {
      demuxer->stream->eof = 1;
      return 0;
    } else {
      return 0;
    }
  };
  return 0;
}

static void demux_ppbox_seek(demuxer_t *demuxer,float rel_seek_secs,float audio_delay,int flags)
{
  PPBOX_Seek()((uint32_t)rel_seek_secs * 1000);
}


const demuxer_desc_t demuxer_desc_ppbox = {
  "Ppbox demuxer",
  "ppbox",
  "ppbox",
  "?",
  "",
  DEMUXER_TYPE_PPBOX,
  0, // no autodetect
  NULL,
  demux_ppbox_fill_buffer,
  demux_ppbox_open,
  NULL,
  demux_ppbox_seek,
  NULL
};
