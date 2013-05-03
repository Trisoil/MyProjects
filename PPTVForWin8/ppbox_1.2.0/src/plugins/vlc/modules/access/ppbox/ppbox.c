/*****************************************************************************
 * ppbox.c : PPBOX demux module
 *****************************************************************************
 * Copyright © 2002-2004, 2006-2008, 2010 the VideoLAN team
 *
 * Authors: Laurent Aimar <fenrir@via.ecp.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

/*****************************************************************************
 * Preamble
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_demux.h>
#include <vlc_dialog.h>

#include <vlc_meta.h>                  /* vlc_meta_Set*, vlc_meta_New */
#include <vlc_access.h>                /* GET_PRIVATE_ID_STATE */
#include <vlc_codecs.h>                /* BITMAPINFOHEADER, WAVEFORMATEX */

#include <ppbox/ppbox.h>

//#define N_(str)           gettext_noop (str)

#undef msg_Dbg
#define msg_Dbg(p_this, ...) printf(__VA_ARGS__); putchar('\n')

#undef msg_Warn
#define msg_Warn(p_this, ...) printf(__VA_ARGS__); putchar('\n')

#ifdef WIN32
#define usleep(x) Sleep(x / 1000)
#endif

/* TODO
 *  - add support for the newly added object: language, bitrate,
 *                                            extended stream properties.
 */

/*****************************************************************************
 * Module descriptor
 *****************************************************************************/
static int  Open  ( vlc_object_t * );
static void Close ( vlc_object_t * );

vlc_module_begin ()
    set_category( CAT_INPUT )
    set_subcategory( SUBCAT_INPUT_DEMUX )
    set_description( "PPBOXdemuxer" )
    set_capability( "access_demux", 200 )
    set_callbacks( Open, Close )
    add_shortcut("ppvod")
    add_shortcut("ppvod2")
    add_shortcut("pplive")
    add_shortcut("pplive2")
    add_shortcut("pplive3")
    add_shortcut("pptv")
vlc_module_end ()


/*****************************************************************************
 * Local prototypes
 *****************************************************************************/
static int Demux  ( demux_t * );
static int Control( demux_t *, int i_query, va_list args );

typedef struct
{
    int i_cat;

    es_out_id_t     *p_es;

    PPBOX_StreamInfoEx *p_si;

    mtime_t i_time;
} ppbox_track_t;

struct demux_sys_t
{
    mtime_t             i_time;     /* s */
    mtime_t             i_length;   /* length of file file */
    int64_t             i_bitrate;  /* global file bitrate */

    bool                b_seek;

    unsigned int        i_track;
    ppbox_track_t       *track[128]; /* track number is stored on 7 bits */
};

static mtime_t  GetMoviePTS( demux_sys_t * );
static int      DemuxInit( demux_t * );
static void     DemuxEnd( demux_t * );

PP_int32 g_ret = 0;
int g_call_back = 0;

static void ppbox_open_call_back(PP_int32 ret)
{
  g_ret = ret;
  g_call_back = 1;
}

/*****************************************************************************
 * Open: check file and initializes PPBOX structures
 *****************************************************************************/
static int Open( vlc_object_t * p_this )
{
    demux_t     *p_demux = (demux_t *)p_this;
    demux_sys_t *p_sys;
    int         close_called = 0;

    char playlink[1024] = {0};
    strcat(playlink, p_demux->psz_access);
    strcat(playlink, "://");
#if PACKAGE_VERSION_MAJOR == 1
    strcat(playlink, p_demux->psz_path);
#elif PACKAGE_VERSION_MAJOR == 2
    strcat(playlink, p_demux->psz_location);
#endif
    
    msg_Dbg(p_demux, "PPBOX_AsyncOpen %s", playlink);

    g_ret = 0;
    g_call_back = 0;
    PPBOX_AsyncOpen()(playlink, ppbox_open_call_back);

    while (!g_call_back) {
        if (!vlc_object_alive(p_demux) && !close_called) {
            PPBOX_Close()();
            close_called = 1;
        }
        usleep(100000);
    }

    if (g_ret != ppbox_success) {
        if (!close_called)
            PPBOX_Close()();
        return VLC_EGENERIC;
    }

    /* Set p_demux fields */
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    p_demux->p_sys = p_sys = (demux_sys_t *)calloc(1, sizeof(demux_sys_t));

    /* Load the headers */
    if( DemuxInit( p_demux ) )
    {
        free( p_sys );
        PPBOX_Close()();
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}


/*****************************************************************************
 * Demux: read packet and send them to decoders
 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;

    for( ;; )
    {
        PP_int32 ret;

        if (!vlc_object_alive(p_demux))
            return -1;

        PPBOX_SampleEx2 sample;
        ret = PPBOX_ReadSampleEx2()(&sample);
        /* Read and demux a packet */
        if (ret == ppbox_success) {
            //msg_Dbg( p_demux, "PPBOX_ReadSampleEx2 stream_index: %lu, start_time: %llu",
            //    sample.stream_index, sample.start_time);
            if (p_sys->i_time < 0)
                es_out_Control(p_demux->out, ES_OUT_SET_PCR, sample.start_time + 1);
            
            ppbox_track_t * tk = p_sys->track[sample.stream_index];

            block_t * p_block = block_Alloc(sample.buffer_length);
            p_block->i_pts = sample.start_time + sample.composite_time_delta;
            p_block->i_dts = sample.start_time;
            memcpy(p_block->p_buffer, sample.buffer, sample.buffer_length);

            tk->i_time = sample.start_time;
            //p_sys->i_time = GetMoviePTS(p_sys);
            p_sys->i_time = sample.start_time;
            es_out_Send(p_demux->out, tk->p_es, p_block);
            es_out_Control(p_demux->out, ES_OUT_SET_PCR, sample.start_time + 1);
            return 1;
        } else if (ret == ppbox_would_block) {
            msg_Dbg(p_demux, "PPBOX_ReadSampleEx2 ppbox_would_block");
            usleep(10000);
            if (!vlc_object_alive(p_demux))
                return -1;
        } else if (ret == ppbox_stream_end) {
            msg_Dbg(p_demux, "PPBOX_ReadSampleEx2 ppbox_stream_end");
            return 0;
        } else {
            msg_Dbg(p_demux, "PPBOX_ReadSampleEx2 %s", PPBOX_GetLastErrorMsg()());
            return -1;
        }
    }

    return 1;
}

/*****************************************************************************
 * Close: frees unused data
 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    demux_t     *p_demux = (demux_t *)p_this;

    DemuxEnd( p_demux );
    
    PPBOX_Close()();

    free( p_demux->p_sys );
}

static int SeekIndex( demux_t *p_demux, mtime_t i_date, float f_pos )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    PP_int32 ret;

    msg_Dbg( p_demux, "seek with index: %i seconds, position %f",
             i_date >= 0 ? (int)(i_date/1000000) : -1, f_pos );

    if( i_date < 0 )
        i_date = p_sys->i_length * f_pos;

    ret = PPBOX_Seek()(i_date / 1000);
    
    return (ret == ppbox_success || ret == ppbox_would_block) ? VLC_SUCCESS : VLC_EGENERIC;
}

static void SeekPrepare( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;

    p_sys->i_time = -1;
    for( int i = 0; i < 128 ; i++ )
    {
        ppbox_track_t *tk = p_sys->track[i];
        if( !tk )
            continue;
        tk->i_time = 1;
    }
}

/*****************************************************************************
 * Control:
 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    bool    *pb_bool;
    int64_t     i64, *pi64;
    double      f, *pf;

    switch (i_query)
    {
    case DEMUX_CAN_SEEK:
        pb_bool = (bool*)va_arg(args, bool*);
        *pb_bool = p_sys->b_seek;
        return VLC_SUCCESS;

    case DEMUX_CAN_CONTROL_PACE:
        pb_bool = (bool*)va_arg(args, bool*);
        *pb_bool = true;
        return VLC_SUCCESS;

    case DEMUX_GET_LENGTH:
        pi64 = (int64_t*)va_arg(args, int64_t *);
        *pi64 = p_sys->i_length;
        return VLC_SUCCESS;

    case DEMUX_GET_TIME:
        if (p_sys->i_time < 0) return VLC_EGENERIC;
        pi64 = (int64_t*)va_arg(args, int64_t *);
        *pi64 = p_sys->i_time;
        return VLC_SUCCESS;

    case DEMUX_SET_TIME:
        SeekPrepare(p_demux);
        {
            va_list acpy;
            va_copy(acpy, args);
            i64 = (int64_t)va_arg(acpy, int64_t);
            va_end(acpy);

            if( !SeekIndex( p_demux, i64, -1 ) )
                return VLC_SUCCESS;
        }
        return VLC_EGENERIC;

    case DEMUX_GET_POSITION:
        if (p_sys->i_time < 0) return VLC_EGENERIC;
        if (p_sys->i_length > 0)
        {
            pf = (double*)va_arg(args, double *);
            *pf = p_sys->i_time / (double)p_sys->i_length;
            return VLC_SUCCESS;
        }
        return VLC_EGENERIC;

    case DEMUX_SET_POSITION:
        SeekPrepare(p_demux);
        if (p_sys->i_length > 0)
        {
            va_list acpy;
            va_copy(acpy, args);
            f = (double)va_arg(acpy, double);
            va_end(acpy);

            if (!SeekIndex(p_demux, -1, f))
                return VLC_SUCCESS;
        }
        return VLC_EGENERIC;

    case DEMUX_GET_PTS_DELAY:
        {
            int64_t *pi_pts_delay = va_arg( args, int64_t * );
            *pi_pts_delay = DEFAULT_PTS_DELAY;
            return VLC_SUCCESS;
        }

    default:
        return VLC_EGENERIC;
    }
}

/*****************************************************************************
 *
 *****************************************************************************/
static mtime_t GetMoviePTS(demux_sys_t *p_sys)
{
    mtime_t i_time = -1;
    int     i;

    for( i = 0; i < 128 ; i++ )
    {
        ppbox_track_t *tk = p_sys->track[i];

        if( tk && tk->p_es && tk->i_time > 0)
        {
            if( i_time < 0 ) i_time = tk->i_time;
            else i_time = __MIN( i_time, tk->i_time );
        }
    }

    return i_time;
}

/*****************************************************************************
 *
 *****************************************************************************/
static int DemuxInit( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;

    /* init context */
    p_sys->i_time   = -1;
    p_sys->i_length = 0;
    p_sys->i_bitrate = 0;
    for( int i = 0; i < 128; i++ )
    {
        p_sys->track[i] = NULL;
    }

    p_sys->i_length =  (mtime_t)PPBOX_GetDuration()() * 1000;
    p_sys->b_seek = p_sys->i_length > 0;

    p_sys->i_track = PPBOX_GetStreamCount()();
    if( p_sys->i_track <= 0 )
    {
        msg_Warn( p_demux, "PPBOX plugin discarded (cannot find any stream!)" );
        return VLC_EGENERIC;
    }
 
    msg_Dbg( p_demux, "found %d streams", p_sys->i_track );

    for( unsigned i_stream = 0; i_stream < p_sys->i_track; i_stream++ )
    {
        ppbox_track_t    *tk;

        tk = p_sys->track[i_stream] = (ppbox_track_t *)malloc(sizeof(ppbox_track_t));
        memset(tk, 0, sizeof(ppbox_track_t));

        tk->i_time = -1;
        tk->p_es = NULL;

        PPBOX_StreamInfoEx * p_si = (PPBOX_StreamInfoEx *)malloc(sizeof(PPBOX_StreamInfoEx));
        PPBOX_GetStreamInfoEx()(i_stream, p_si);
        
        tk->p_si = p_si;
				
        es_format_t fmt;

        if (p_si->type == ppbox_audio)
        {
            es_format_Init(&fmt, AUDIO_ES, 0);
            fmt.i_codec           = VLC_FOURCC( 'm', 'p', '4', 'a' );
            fmt.audio.i_channels  = p_si->audio_format.channel_count;
            fmt.audio.i_rate      = p_si->audio_format.sample_rate;
            fmt.audio.i_bitspersample   = p_si->audio_format.sample_size;
            fmt.i_extra           = p_si->format_size;
            fmt.p_extra           = malloc( fmt.i_extra );
            memcpy( fmt.p_extra, p_si->format_buffer, fmt.i_extra );

            msg_Dbg(p_demux, "added new audio stream(codec:0x%x,ID:%d)", fmt.i_codec, i_stream);
        }
        else if (p_si->type == ppbox_video)
        {
            es_format_Init( &fmt, VIDEO_ES, 0);
            fmt.i_codec           = VLC_FOURCC( 'a', 'v', 'c', '1' );
            fmt.video.i_width     = p_si->video_format.width;
            fmt.video.i_height    = p_si->video_format.height;
            fmt.video.i_frame_rate = 10000000;
            fmt.video.i_frame_rate_base = 1;
            fmt.i_extra           = p_si->format_size;
            fmt.p_extra           = malloc( fmt.i_extra );
            memcpy( fmt.p_extra, p_si->format_buffer, fmt.i_extra );

            msg_Dbg(p_demux, "added new video stream(ID:%d)", i_stream);
        }
        else
        {
            es_format_Init( &fmt, UNKNOWN_ES, 0 );
        }

        tk->i_cat = fmt.i_cat;
        if( fmt.i_cat != UNKNOWN_ES )
        {
            tk->p_es = es_out_Add( p_demux->out, &fmt );
        }
        else
        {
            msg_Dbg(p_demux, "ignoring unknown stream(ID:%d)", i_stream);
        }
        es_format_Clean( &fmt );
    }

    return VLC_SUCCESS;
}
/*****************************************************************************
 *
 *****************************************************************************/
static void DemuxEnd( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    int         i;

    for( i = 0; i < 128; i++ )
    {
        ppbox_track_t *tk = p_sys->track[i];

        if( tk )
        {
            if( tk->p_es )
            {
                es_out_Del( p_demux->out, tk->p_es );
            }
            free( tk );
        }
        p_sys->track[i] = NULL;
    }
}

