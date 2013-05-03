// UdpPacket.h

#ifndef _UDPPACKET_H_
#define _UDPPACKET_H_

#include <MsgTrans/Types.h>

//����ͷ����Ϣ����
#define  NHTYPE_REQUEST				0x01
//����������������
#define  NHTYPE_EVALUATEIMS			0x06

//֧�ֻ�������Э�������ͷ������
#define  NHTYPE_CONFUSE_DIFF        0x20
//VOD UDPЭ�����������ݴ���ȵ���������
#define  NHTYPE_CONFUSE_PFS         (NHTYPE_REQUEST + NHTYPE_CONFUSE_DIFF)

//Э�����Ͷ���
//pfs�����ݰ�����
#define PFS_STREAM_HEADER			0xEE

//pfs���������
#define PFS_CMD_HEADER				0xAC

//protocol type define
//������Client�������ط�Ƭ
#define REQ_PIECE					0x01
//�������ط�Ƭ���
#define RTN_PIECE					0x02


//������Դ��subpiece (P2Pʹ��)				
#define REQ_SUBPIECE				0x04
//���ص�Subpiece (P2Pʹ��)					
#define RTN_SUBPIECE				0x05


#define RTN_SUBPIECE_ERR			0x06

#define NO_PIECE_ERROR				0x02

#define RTN_PIECE_ERR				0x03

#pragma pack (1)

struct UdpNetHeader
{
    Util::UInt16 m_usNetType;     //0���籨������
    Util::UInt16 m_usCheckSum;    //2У���
    Util::UInt16 m_usVersion;     //4���ְ汾��
    Util::UInt16 m_usReserve;     //6�����ֶ�
    Util::UInt32 m_uConfusionKey; //8Э�������Key
    Util::UInt32 m_uFileHandle;   //12������(P2Pʹ��)
};

const Util::UInt32 RESID_LENGTH = 34;
struct UdpHeader
    : UdpNetHeader
{
    Util::UInt8 pfsSymbol; // pfs��־,1�ֽ�,Ĭ��Ϊcommand����
    Util::UInt8 protocolType; //Э������,1�ֽ�
    Util::UInt8 addParam; //���Ӳ���,1�ֽ�
    Util::UInt8 codeType; //��������������,1�ֽ�	
    Util::UInt16 length; //����������,2�ֽ�	
    Util::UInt32 reserve;
    Util::UInt32 requestsendtime; //ʱ���,4�ֽ�	
    char resid[RESID_LENGTH]; //32�ֽڵ���ԴID����֤ȫ��Ψһ�ģ���Ϊ��Դ���ݵ�MD5ֵ��
    Util::UInt32 piece_index; //��ý�����ݷ�Ƭ��index
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
