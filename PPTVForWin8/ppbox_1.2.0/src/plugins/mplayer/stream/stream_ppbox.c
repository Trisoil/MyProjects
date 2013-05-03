/*
 * stream layer for multiple files input, based on previous work from Albeu
 *
 * Copyright (C) 2006 Benjamin Zores
 *
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
#include <string.h>

#include "ppbox/ppbox.h"

#include "stream.h"
#include "input/input.h"
#include "libmpdemux/demuxer.h"

static void
ppbox_stream_close (stream_t *stream)
{
  PPBOX_Close()();
}

PP_int32 g_ret = 0;
int g_call_back = 0;

static void ppbox_open_call_back(PP_int32 ret)
{
  g_ret = ret;
  g_call_back = 1;
}

static int
ppbox_stream_open (stream_t *stream, int mode, void *opts, int *file_format)
{
  int close_called = 0;

  g_ret = 0;
  g_call_back = 0;
  PPBOX_AsyncOpen()(stream->url, ppbox_open_call_back);

  while (!g_call_back) {
    if (mp_input_check_interrupt(100) && !close_called) {
      PPBOX_Close()();
      close_called = 1;
    }
  }

  mp_msg(MSGT_OPEN,MSGL_ERR, "PPBOX_AsyncOpen = %lu\n", g_ret);
  if (g_ret != ppbox_success) {
    if (!close_called)
      PPBOX_Close()();
    return STREAM_ERROR;
  }

  stream->type = STREAMTYPE_PPBOX;
  stream->close = ppbox_stream_close;

  *file_format = DEMUXER_TYPE_PPBOX;

  return STREAM_OK;
}

const stream_info_t stream_info_ppbox = {
  "Ppbox input",
  "ppbox",
  "Cmguo",
  "",
  ppbox_stream_open,
  { "ppvod", "ppvod2", "pplive", "pplive2", "pplive3", "pptv", NULL },
  NULL,
  1
};
