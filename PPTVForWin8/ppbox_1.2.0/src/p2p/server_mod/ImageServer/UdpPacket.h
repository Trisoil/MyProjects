// UdpPacket.h

#ifndef _UDPPACKET_H_
#define _UDPPACKET_H_

#include <MsgTrans/Types.h>

//网络头部信息类型
#define  NHTYPE_REQUEST				0x01
//测量带宽质量信令
#define  NHTYPE_EVALUATEIMS			0x06

//支持混淆加密协议的网络头部类型
#define  NHTYPE_CONFUSE_DIFF        0x20
//VOD UDP协议中请求、数据传输等的网络类型
#define  NHTYPE_CONFUSE_PFS         (NHTYPE_REQUEST + NHTYPE_CONFUSE_DIFF)

//协议类型定义
//pfs流数据包类型
#define PFS_STREAM_HEADER			0xEE

//pfs信令包类型
#define PFS_CMD_HEADER				0xAC

//protocol type define
//向其他Client请求下载分片
#define REQ_PIECE					0x01
//返回下载分片结果
#define RTN_PIECE					0x02


//请求资源的subpiece (P2P使用)				
#define REQ_SUBPIECE				0x04
//返回的Subpiece (P2P使用)					
#define RTN_SUBPIECE				0x05


#define RTN_SUBPIECE_ERR			0x06

#define NO_PIECE_ERROR				0x02

#define RTN_PIECE_ERR				0x03

#pragma pack (1)

struct UdpNetHeader
{
    Util::UInt16 m_usNetType;     //0网络报文类型
    Util::UInt16 m_usCheckSum;    //2校验和
    Util::UInt16 m_usVersion;     //4数字版本号
    Util::UInt16 m_usReserve;     //6保留字段
    Util::UInt32 m_uConfusionKey; //8协议混淆的Key
    Util::UInt32 m_uFileHandle;   //12网络句柄(P2P使用)
};

const Util::UInt32 RESID_LENGTH = 34;
struct UdpHeader
    : UdpNetHeader
{
    Util::UInt8 pfsSymbol; // pfs标志,1字节,默认为command类型
    Util::UInt8 protocolType; //协议类型,1字节
    Util::UInt8 addParam; //附加参数,1字节
    Util::UInt8 codeType; //数据区编码类型,1字节	
    Util::UInt16 length; //数据区长度,2字节	
    Util::UInt32 reserve;
    Util::UInt32 requestsendtime; //时间戳,4字节	
    char resid[RESID_LENGTH]; //32字节的资源ID，保证全局唯一的（定为资源内容的MD5值）
    Util::UInt32 piece_index; //流媒体数据分片的index
};

const Util::UInt32 UDP_RECV_BUFFER_SIZE = ZERO_SUB_PIECE_SIZE;
struct UdpDataRequestPacket
    : public UdpHeader
{
    Util::UInt8 data_buf[UDP_RECV_BUFFER_SIZE];
};

#pragma pack ()

struct UdpDataRequestPacket2
    : public UdpDataRequestPacket
    , Util::ObjectMemoryPoolObjectNoThrow<UdpDataRequestPacket2, Util::NullLock>
{
    boost::asio::ip::udp::endpoint remote;
};

#endif // #ifndef _DATAREQUESTPACKET_H_
