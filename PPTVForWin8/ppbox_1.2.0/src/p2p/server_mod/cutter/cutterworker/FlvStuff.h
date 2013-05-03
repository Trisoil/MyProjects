#pragma once

#ifndef WIN32
typedef uint32_t UINT32;
#endif

#define FLV_UI32(x) (int)(((x[0]) << 24) | ((x[1]) << 16) | ((x[2]) << 8) | (x[3]))
#define FLV_UI24(x) (int)(((x[0]) << 16) + ((x[1]) << 8) + (x[2]))
#define FLV_UI16(x) (int)(((x[0]) << 8) + (x[1]))
#define FLV_UI8(x)  (int)((x))

const UINT32 FLV_FILEHEADER_T_SIZE = 13;
const UINT32 FLV_TAGHEADER_T_SIZE = 11;
const UINT32 FLV_TAGDATAHEADER_T_SIZE = 2;
const UINT32 FLV_PREVIOUSTAGSIZE_SIZE = 4;

const unsigned char FLV_AUDIODATA = 8;
const unsigned char FLV_VIDEODATA = 9;
const unsigned char FLV_SCRIPTDATAOBJECT = 18;

typedef struct {
	unsigned char signature[3];
	unsigned char version;
	unsigned char flags;
	unsigned char headersize[4];
	unsigned char fristtagsize[4]; // Frist Previous_Tag_size, always zero.
} FLVFileHeader_t;

typedef struct _FLVTagHeader_t{
	unsigned char type;
	unsigned char datasize[3];
	unsigned char timestamp[3];
	unsigned char timestamp_ex;
	unsigned char streamid[3];

	static UINT32 get_tag_ts(_FLVTagHeader_t* header)
	{
		UINT32 ts = FLV_UI24(header->timestamp);
		ts |= (header->timestamp_ex << 24);
		return ts;
	}

	static void set_tag_ts(_FLVTagHeader_t* header, UINT32 ts)
	{
		header->timestamp[2] = ts & 0xff;
		header->timestamp[1] = (ts >> 8) & 0xff;
		header->timestamp[0] = (ts >> 16) & 0xff;
		header->timestamp_ex = (ts >> 24) & 0xff;
	}
} FLVTagHeader_t;

typedef struct {
	unsigned char frameandcodetype;
	unsigned char avcpackettype;
} VideoTagDataHeader_t;

typedef struct {
	unsigned char audioformat;
	unsigned char aacpackettype;
} AudioTagDataHeader_t;



