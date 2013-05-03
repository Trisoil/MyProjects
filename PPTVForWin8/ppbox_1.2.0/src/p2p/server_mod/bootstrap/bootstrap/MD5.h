//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _MD5_
#define _MD5_

//#ifdef WIN32
#pragma pack(1)
//#endif

#ifndef WIN32
typedef framework::string::UUID GUID;
#endif

typedef struct {
    unsigned long state[4];    /* state (ABCD) */
    unsigned long count[2];    /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];    /* input buffer */
} MD5CTX;
#pragma pack()

class CMD5
{
public:
    static Guid GenerateRID(vector<MD5> block_md5s, unsigned int block_count);
    // 可以采用以下方式生成
    // content_info_.content_md5 = CMD5::GenerateMD5(buffer, content_info_.content_len);
    static Guid GenerateMD5(const unsigned char *data, unsigned int data_len);
    static char* Guid2Str(const GUID* guid, char* buf);

private:
    void MD5Init ();
    void MD5Update (const unsigned char *, unsigned int);
    void MD5Final ();
    void MD5Transform (unsigned long state[4], const unsigned char block[64]);
    void Encode (unsigned char* output, unsigned long* input, unsigned int len);
    void Decode (unsigned long* output, const unsigned char* input, unsigned int len);
    
private:
    MD5CTX m_ctx;
    unsigned char m_digest[16];
};

#endif
