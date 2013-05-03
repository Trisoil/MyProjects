//------------------------------------------------------------------------------
// File: PushGuids.h
//
// Desc: DirectShow sample code - GUID definitions for PushSource filter set
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#pragma once

#ifndef __PUSHGUIDS_DEFINED
#define __PUSHGUIDS_DEFINED

// {6A881765-07FA-404b-B9B8-6ED429385ECC}
DEFINE_GUID(CLSID_PpboxSource, 
            0x6a881765, 0x7fa, 0x404b, 0xb9, 0xb8, 0x6e, 0xd4, 0x29, 0x38, 0x5e, 0xcc);

DEFINE_GUID(MEDIASUBTYPE_AVC1,
            0x31435641, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

#define WAVE_FORMAT_RAW_AAC1 0x00FF
// {000000FF-0000-0010-8000-00AA00389B71}
DEFINE_GUID(MEDIASUBTYPE_RAW_AAC1,
            WAVE_FORMAT_RAW_AAC1, 0x000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);


#endif
