#include "Common.h"
#include "UdpServer.h"
#include "UdpPacket.h"

#include <Framework/ErrorMsg.h>
#include <Framework/PeriodAction.h>
#include <Memory/AsioHandlerAllocator.h>
using namespace Util;

/******************************************************************
������: 
Confuse
�����б�:
char *pcBuf��(�������)��Ҫ����������Buffer
UINT32 uBufLen��(����)Buffer�ĳ���
����ֵ:
��
����: 
�������pcBuf����������VoDЭ��(����ͷ��+P2Pͷ��)�����л�������
�����pcBuf�ǻ���֮������ݣ�

ע��	
1.�����pcBuf����ͷ����Ҫ����һ��key��m_uConfusionKey(�����������GetTickCount)
2.���������buffer����С������ͷ������
******************************************************************/
static void Confuse(char *pcBuf, size_t uBufLen)
{
	if (NULL == pcBuf || uBufLen < sizeof(UdpNetHeader))
		return;

	if (uBufLen > sizeof(UdpHeader)) {
		uBufLen = sizeof(UdpHeader);
	}

	UdpNetHeader *pnh = (UdpNetHeader*)pcBuf;
	char * ptr_a = (char *)&pnh->m_uConfusionKey;
	char * ptr_b = ptr_a + 1;

	char * ptr_beg = (char *)&pnh->m_usVersion;
	char * ptr_end = ptr_beg + uBufLen - 4;

	if (ptr_b == ptr_end) {
		ptr_b = ptr_beg; // �����������Σ�KyeΪ1B�����������
	}

	size_t len = uBufLen - 4;
	while (len--) {
		*ptr_b =  (*ptr_a) ^ (*ptr_b);
		ptr_a++;
		ptr_b++;
		if (ptr_a == ptr_end) {
			ptr_a = ptr_beg;
		}
		if (ptr_b == ptr_end) {
			ptr_b = ptr_beg;
		}
	}

}

/******************************************************************
������: 
unConfuse
�����б�:
char *pcBuf��(�������)��Ҫȥ������������Buffer
UINT32 uBufLen��(����)Buffer�ĳ���
����ֵ:
��
����: 
�������pcBuf����������VoDЭ��(����ͷ��+P2Pͷ��)��ȥ������
�����pcBuf��ȥ������֮������ݣ�
ע�⣺
1.���������buffer����С������ͷ������
******************************************************************/
static void unConfuse(char *pcBuf, size_t uBufLen)
{
	if (NULL == pcBuf || uBufLen < sizeof(UdpNetHeader))
		return;

	if (uBufLen > sizeof(UdpHeader)) {
		uBufLen = sizeof(UdpHeader);
	}

	UdpNetHeader *pnh = (UdpNetHeader*) pcBuf;    
	char * ptr_a = (char *)&pnh->m_uConfusionKey;
	char * ptr_b = ptr_a - 1;

	char * ptr_beg = (char *)&pnh->m_usVersion - 1;
	char * ptr_end = ptr_beg + uBufLen - 4;

	if (ptr_b == ptr_beg) {
		ptr_b = ptr_end; // �����������Σ�Key����ǰ
	}

	size_t len = uBufLen - 4;
	while (len--) {
		* ptr_a = (*ptr_a) ^ (*ptr_b);
		ptr_a--;
		ptr_b--;
		if (ptr_a == ptr_beg) {
			ptr_a = ptr_end;
		}
		if (ptr_b == ptr_beg) {
			ptr_b = ptr_end;
		}
	}
}


/****************************************************************************************
������:
checksum
�����б�: 
USHORT *buffer������������ݻ����׵�ַ 
int size�����������ݳ���
����ֵ:
UINT16����ɺ�İ���У���
����:	
�������У��ͣ��ڷ������ͻ��˵�ʱ����Ҫ���øú���
*****************************************************************************************/	
static UInt16 checksum(UInt16 *buffer, size_t size)
{
	UInt32 cksum = 0;

	while (size > 1) {
		cksum += *buffer++;
		size -= sizeof(UInt16);
	}
	if (size) {
		cksum += *(UInt8 *)buffer;
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >>16);
	return (UInt16)(~cksum);
}

std::string UdpServer::ip = "0.0.0.0";
int UdpServer::port = 7100;
size_t UdpServer::max_sync_recv = 1000;
size_t UdpServer::interval_statistics = 1;
int UdpServer::log_level = 2;

boost::asio::ip::udp::endpoint UdpServer::endpoint_;
boost::asio::ip::udp::socket * UdpServer::socket_ = NULL;

UdpServerStatistics * UdpServer::stat_ = NULL;
size_t UdpServer::wait_sync_recv = 0;

struct UdpServerStatistics
    : SharedMemoryPool::PoolObjectNoThrow
{
    Statistics recv;
    Statistics recv_err;
    Statistics send;
    Statistics send_err;
    Statistics req;
    Statistics resp;
    Statistics resp_err;
};

ErrorCode UdpServer::init(
                    Config & conf)
{

    Config::ConfigItem params[] = {
        CONFIG_PARAM(ip, Config::allow_get), 
        CONFIG_PARAM(port, Config::allow_get), 
        CONFIG_PARAM(max_sync_recv, Config::allow_get), 
        CONFIG_PARAM(interval_statistics, Config::allow_get), 
        CONFIG_PARAM(log_level, Config::allow_get | Config::allow_set), 
    };

    conf.register_module("UdpServer", sizeof(params) / sizeof(params[0]), params);

    return ERROR_SUCCEED;
}

ErrorCode UdpServer::start(
                     boost::asio::io_service & io_service)
{
    try {
        socket_ = new boost::asio::ip::udp::socket(io_service);
        socket_->open(endpoint_.protocol());
        endpoint_.address(boost::asio::ip::address::from_string(ip));
        endpoint_.port(port);
        socket_->bind(endpoint_);
    } catch (...) {
        return ERROR_ERROR;
    }

    stat_ = new (SHID_UDP_SERVER) UdpServerStatistics;

    for (size_t i = 0; i < (size_t)max_sync_recv; i++) {
        UdpDataRequestPacket2 * packet = new UdpDataRequestPacket2;
        if (!packet) {
            logger.Log(Logger::kLevelAlarm, log_level, "[UdpServer::start] no memory");
            return ERROR_NO_MOMERY;
        }
        // async_receive_from ����С�ڴ���䣬asio_alloc_helper
        socket_->async_receive_from(
            boost::asio::buffer(packet, UDP_RECV_BUFFER_SIZE), 
            packet->remote, 
            make_asio_alloc_handler_lock(
                boost::bind(UdpServer::HandleReceive, 
                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, packet)));
    }

    boost::posix_time::ptime now = 
        boost::posix_time::second_clock::universal_time();

    PeriodAction::register_action(
        "UpdServerStatistics", 
        boost::bind(UdpServer::Statistics), 
        now, 
        boost::posix_time::seconds((long)interval_statistics));

    return ERROR_SUCCEED;
}

ErrorCode UdpServer::stop()
{
    delete socket_;
    return ERROR_SUCCEED;
}

void UdpServer::HandleReceive(
                              boost::asio::error_code const & err, 
                              size_t size, 
                              UdpDataRequestPacket2 * packet)
{
    if (!err) {
        stat_->recv++;
        if (size >= sizeof(UdpHeader)) {
            if (packet->m_usNetType == NHTYPE_CONFUSE_PFS) {//Ϊ����,��Ҫ���
                unConfuse((char *)packet, size);
                if (packet->pfsSymbol == PFS_CMD_HEADER) {
                    if (packet->protocolType == REQ_SUBPIECE) {
                        packet->protocolType = RTN_SUBPIECE;
                        stat_->req++;
                        CacheManager::ReadSubPiece(packet->resid, packet->piece_index, packet->addParam, packet->data_buf, 
                            boost::bind(UdpServer::HandleReadData, _1, _2, packet));
                        // ���������ڴ�����µ�����
                        packet = new UdpDataRequestPacket2;
                    } else { // if (packet->protocolType == REQ_SUBPIECE)
                        logger.Log(Logger::kLevelDebug, log_level, "[UdpServer::HandleReceive] unknown protocol type (%u)", (UInt32)packet->protocolType);
                    }
                } else { // if (lpCmdPLHeader->pfsSymbol == PFS_CMD_HEADER)
                    logger.Log(Logger::kLevelDebug, log_level, "[UdpServer::HandleReceive] invalid pfs symbol (%u)", (UInt32)packet->pfsSymbol);
                }	
            } else { // if (packet->m_usNetType == NHTYPE_CONFUSE_PFS)
                logger.Log(Logger::kLevelDebug, log_level, "[UdpServer::HandleReceive] unknown net type (%u)", (UInt32)packet->m_usNetType);
            }
        } else { // if (size < sizeof(UdpHeader))
            logger.Log(Logger::kLevelDebug, log_level, "[UdpServer::HandleReceive] packet too small (%u)", size);
        }
    } else {
        stat_->recv_err++;
        logger.Log(Logger::kLevelDebug, log_level, "[UdpServer::HandleReceive] recv error (%s)", sys_err_msg(err).c_str());
    }

    if (!packet) {
        logger.Log(Logger::kLevelAlarm, log_level, "[UdpServer::HandleReceive] no memory");
        wait_sync_recv++;
    } else {
        socket_->async_receive_from(
            boost::asio::buffer(packet, UDP_RECV_BUFFER_SIZE), 
            packet->remote, 
            make_asio_alloc_handler_lock(
                boost::bind(UdpServer::HandleReceive, 
                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, packet)));
    }
}

void UdpServer::HandleReadData(
                               ErrorCode err, 
                               size_t size, 
                               UdpDataRequestPacket2 * packet)
{
    stat_->resp++;
    //logger.Log(Logger::kLevelDebug1, log_level, "[UdpServer::HandleReadData]");
    if (err != ERROR_SUCCEED) {
        logger.Log(Logger::kLevelDebug, log_level, "[UdpServer::HandleReadData] read data error (%s)", 
            ErrorMsg::error_message(err).c_str());
        packet->protocolType = RTN_SUBPIECE_ERR;
        packet->codeType = NO_PIECE_ERROR;
        stat_->resp_err++;
    }
    packet->length = (UInt16)size;
    size_t len = sizeof(UdpHeader) + size;
    Confuse((char *)packet, len);
    packet->m_usCheckSum = 0;
    packet->m_usCheckSum = checksum((UInt16 *)packet, len);
    socket_->async_send_to(boost::asio::buffer(packet, len), packet->remote, 
        make_asio_alloc_handler_lock(
            boost::bind(UdpServer::HandleSend, boost::asio::placeholders::error, packet)));
}

void UdpServer::HandleSend(
                           boost::asio::error_code const & err, 
                           UdpDataRequestPacket2 * packet)
{
    if (err)
        stat_->send_err++;
    else
        stat_->send++;

    if (wait_sync_recv) {
        socket_->async_receive_from(
            boost::asio::buffer(packet, UDP_RECV_BUFFER_SIZE), 
            packet->remote, 
            make_asio_alloc_handler_lock(
                boost::bind(UdpServer::HandleReceive, 
                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, packet)));
        wait_sync_recv--;
    } else {
        delete packet;
    }
}

void UdpServer::Statistics()
{
    stat_->recv.tick();
    stat_->recv_err.tick();
    stat_->send.tick();
    stat_->send_err.tick();
    stat_->req.tick();
    stat_->resp.tick();
    stat_->resp_err.tick();
}
