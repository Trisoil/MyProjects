// peer.cpp : Defines the entry point for the DLL application.
//
#include "StdAfx.h"

#define LIVE_SOURCE

#include "app/controller.h"
#include <ppl/os/shell.h>

#include <synacast/pub/peerdll.h>
#include <synacast/pub/CoreStatus.h>

#include <ppl/os/shell.h>

#include "common/VersionNumber.h"
#include "live/live/Version.h"

#ifndef _WIN32
PPL_DEFINE_GUID_NULL;
#endif

static LiveChannelController s_LiveController;

#if __cplusplus
extern "C" {
#endif // __cplusplus


bool LIVE_DECL LiveStartup(int node_type)
{
	return s_LiveController.Start(node_type);
}

void LIVE_DECL LiveCleanup()
{
	s_LiveController.Stop();
}
 
LIVE_DECL void* LiveStartChannel(const char* urlstr, int tcpPort, int udpPort)
{
	LiveChannelPtr channel = s_LiveController.StartChannel(urlstr, tcpPort, udpPort);
	return static_cast<void*>(channel.get());
}

void LIVE_DECL LiveStopChannel(void* handle)
{
	s_LiveController.StopChannel(handle);
}

bool LIVE_DECL LiveGetChannelParameter(void* handle, CCoreParameter* param)
{
	if (NULL == param)
	{
		assert(false);
		return false;
	}
	LiveChannel* channel = s_LiveController.GetChannel(handle);
	if (NULL == channel)
		return false;
	const CLiveInfo* liveInfo = channel->GetInfo();
	const SYSINFO* sysInfo = channel->GetSysInfo();
	if (NULL == liveInfo || NULL == sysInfo)
		return false;
	param->m_usTCPListenPort = sysInfo->TcpPort;
	param->m_usUdpListenPort = sysInfo->UdpPort;
	//param->m_usTCPUPNPPort = sysInfo->ExtenalTcpPort;
	//param->m_usUdpUPNPPort = sysInfo->ExtenalUdpPort;
	param->m_MaxConnectionsPerChannel = sysInfo->MaxAppPeerCount;
	param->m_MaxCoPendingConnections = sysInfo->MaxAppPeerCount;
	param->m_ConnectionCtrl = liveInfo->IntelligentConnectionControlDisabled ? 0 : 1;
	param->m_TransferMode = liveInfo->TransferMethod;
	param->m_GraphMode = liveInfo->GraphMode;

	param->m_dwFileVersionMS = 0;
	param->m_dwFileVersionLS = P2P_MODULE_VERSION_NUMBER_UINT16;
	return true;
}

bool LIVE_DECL LiveSetChannelParameter(void* handle, const CCoreParameter* param)
{
	if (NULL == param)
	{
		assert(false);
		return false;
	}
	LiveChannel* channel = s_LiveController.GetChannel(handle);
	if (NULL == channel)
		return false;
	CLiveInfo* liveInfo = channel->GetInfo();
	SYSINFO* sysInfo = channel->GetSysInfo();
	if (NULL == liveInfo || NULL == sysInfo)
		return false;
	//sysInfo->TcpPort = param->m_usTCPListenPort;
	//sysInfo->UdpPort = param->m_usUdpListenPort;
	//sysInfo->ExtenalTcpPort = param->m_usTCPUPNPPort;
	//sysInfo->ExtenalUdpPort = param->m_usUdpUPNPPort;
	sysInfo->MaxAppPeerCount = param->m_MaxConnectionsPerChannel;
	LIMIT_MIN(sysInfo->MaxAppPeerCount, 15);
	sysInfo->MaxConnectPendingCount = param->m_MaxCoPendingConnections;
	LIMIT_MIN(sysInfo->MaxConnectPendingCount, 1);
	liveInfo->IntelligentConnectionControlDisabled = 0 == param->m_ConnectionCtrl;
	liveInfo->TransferMethod = param->m_TransferMode;
	liveInfo->GraphMode = param->m_GraphMode;
	return true;
}

bool LIVE_DECL LiveGetChannelStatus(void* handle, CCoreStatus* status)
{
	if (NULL == status)
	{
		assert(false);
		return false;
	}
	LiveChannel* channel = s_LiveController.GetChannel(handle);
	if (NULL == channel)
		return false;
	const CLiveInfo* liveInfo = channel->GetInfo();
	if (NULL == liveInfo)
		return false;
	status->m_MediaType = liveInfo->MediaType;
	UINT8 skipPercent = liveInfo->LocalPeerInfo.StatusInfo.Status.SkipPercent;
	assert(skipPercent <= 100);
	LIMIT_MAX(skipPercent, 100);
	status->m_BufferPercent = 100 - skipPercent;
	status->m_BufferTime = liveInfo->LocalPeerInfo.Statistics.BufferTime;
	status->m_DownloadSpeed = liveInfo->LocalPeerInfo.Flow.GetRecentDownloadSpeed();
	status->m_UploadSpeed = liveInfo->LocalPeerInfo.Flow.GetRecentUploadSpeed();

	status->m_ConnectionCountt = liveInfo->RemotePeerCount;
	status->m_PendingPeerCount = liveInfo->IPPoolInfo.PendingConnectionCount;
	status->m_TotalPeerCount = liveInfo->IPPoolInfo.TotalPoolSize;

	status->m_TotalDownloadBytes = liveInfo->ChannelInfo.TotalDownloadBytes;
	status->m_TotalUploadBytes = liveInfo->ChannelInfo.TotalUploadBytes;
	status->m_uMediaListenPort = liveInfo->PlayPort;
	return true;
}

bool LIVE_DECL LiveSetChannelPlayerStatus(void* handle, int pstatus)
{
	LiveChannel* channel = s_LiveController.GetChannel(handle);
	if (NULL == channel)
		return false;
	channel->SetPlayerStatus(pstatus);
	return true;
}

bool LIVE_DECL LiveSetChannelUPNP(void* handle, const CCoreUPNP* upnpInfo)
{
	if (NULL == upnpInfo)
	{
		assert(false);
		return false;
	}
	LiveChannel* channel = s_LiveController.GetChannel(handle);
	if (NULL == channel)
		return false;
	SYSINFO* sysInfo = channel->GetSysInfo();
	if (NULL == sysInfo)
	{
		assert(false);
		return false;
	}
	if (upnpInfo->m_usUdpUpnpIn != 0 && upnpInfo->m_usUdpUpnpIn == sysInfo->UdpPort)
	{
		sysInfo->ExtenalUdpPort = upnpInfo->m_usUdpUpnpOut;
	}
	if (upnpInfo->m_usTcpUpnpIn != 0 && upnpInfo->m_usTcpUpnpIn == sysInfo->TcpPort)
	{
		sysInfo->ExtenalTcpPort = upnpInfo->m_usTcpUpnpOut;
	}
	return true;
}

bool LIVE_DECL LiveSetChannelCallback(void* handle, FUNC_CallBack callback, unsigned int channelHandle)
{
	LiveChannel* channel = s_LiveController.GetChannel(handle);
	if (NULL == channel)
		return false;
	channel->SetPlayerCallback(callback, channelHandle);
	return true;
}

#if __cplusplus
}
#endif // __cplusplus
