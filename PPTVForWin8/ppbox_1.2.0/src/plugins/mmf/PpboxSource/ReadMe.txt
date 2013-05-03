Overview
--------

This sample contains an Ppbox source for Media Foundation.

- The source parses Ppbox systems-layer streams and generates 
  samples that contain Ppbox payloads.
- It does not support files that contain a raw Ppbox video or 
  audio stream.
- It does not support seeking.

This sample requires Windows Vista or later.
  
  
To use this sample:
-------------------

1. Build the sample.
2. Regsvr32 PpboxSource.dll
3. Use the BasicPlayback sample to play an Ppbox video file.  

  

Classes:
--------

Buffer: Resizable buffer used to hold the Ppbox data

PpboxByteStreamHandler: Bytestream handler for the Ppbox source.

PpboxSource: Ppbox source. Implements IMFMediaSource.

PpboxStream: Ppbox stream. Implements IMFMediaStream.

Parser: Ppbox elementary stream parser.



THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright (c) Microsoft Corporation. All rights reserved.