// P2PDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "P2PMonitor.h"
#include "P2PDlg.h"
#include "DataStruct.h"
#include "Common.h"
#include "P2PMonitorDlg.h"
#include "FileVersion.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;
using namespace p2p_monitor;

// CP2PDlg 对话框

IMPLEMENT_DYNAMIC(CP2PDlg, CDialog)

CP2PDlg::CP2PDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CP2PDlg::IDD, pParent)
    , m_filelen(0)
    , m_Send(0)
    , m_Receive(0)
    , m_http_kbps(0)
    , m_p2p_kbps(0)
    , m_datarate(0)
    , m_peer_connect_count(_T(""))
    , m_switch_state(_T(""))
    , m_sort_col(-1)
    , m_filename(_T(""))
    , m_list_update(TRUE)
    , m_sort(0)
    , m_total_http(0)
    , m_total_p2p(0)
    , m_save_rate(_T(""))
    , m_is_saving(FALSE)
    , m_strIP(_T(""))
    , m_strLocation(_T(""))
    , m_locater("IP.dat")
    , m_re_rate(_T(""))
    , m_lost_rate(_T(""))
    , m_http_server_status(0)
    , m_NonConsistentSize(0)
    , m_rest_time(0)
    , m_speed_limit(0)
    , m_bandwidth(0)
    , m_strConnectKick(_T(""))
    , m_GlobalWindowSize(0)
    , m_GlobalRequestSendCount(0)
    , m_MemoryPoolLeftSize(0)
    , m_empty_subpiece_distance(0)
    , m_is_created(FALSE)
    , m_p2p_sn_kbps(0)
    , m_p2p_peer_kbps(0)
    , full_block_peer_count(0)
    , peer_count(0)
    , ip_pool_peer_count(0)
    , import_file_type(0)
    , m_second(0)
    , m_is_pause(TRUE)
    , m_total_sn(0)
    , m_line_number(0)
    , last_line_number_(0)
    , m_sn_state(0)
{
    m_label_info.resize(0);
    m_second = 0;
}

BOOL CP2PDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
#if !_PUBLIC_VERSION_ 
    m_hOn = AfxGetApp()->LoadIcon(IDI_ON);
    m_hOff = AfxGetApp()->LoadIcon(IDI_OFF);

    m_switch_http.SetIcon((HICON__*)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_OFF), 
        IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));

    m_switch_p2p.SetIcon((HICON__*)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_OFF), 
        IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
#endif

    m_peerlist.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
#if !_PUBLIC_VERSION_
    h_on = (HICON__*)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    h_off = (HICON__*)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_OFF), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
#endif

#if _PUBLIC_VERSION_

    GetDlgItem(IDC_STATIC8)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SAVE_RATE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC22)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SWITCH_HP)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SWITCH_H)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SWITCH_P)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RESET)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC23)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC24)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC25)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC26)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC27)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC28)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC29)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SEND)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RECEIVE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CONSISTENT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RE_RATE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_LOST_RATE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_REST_TIME)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC30)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SPEED_LIMIT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC31)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BAND)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC21)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SWITCH_STATE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC19)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC20)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC16)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CONNECT_COUNT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC15)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_DATARATE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC32)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC3333)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CONNECT_KICK)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC3434)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GLOBAL_WINDOW_SIZE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC3535)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GLOBAL_REQUEST_SEND_COUNT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC3636)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_MEMORY_POOL_LEFT_SIZE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC37)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EMPTY_DISTANCE)->ShowWindow(SW_HIDE);
#endif
    GetClientRect(&m_rect);
    m_p2p_ori_h = m_rect.Height();
    m_p2p_ori_w = m_rect.Width();
    return TRUE;
}

CP2PDlg::~CP2PDlg()
{
}

void CP2PDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RID_LIST, m_ridlist);
    DDX_Control(pDX, IDC_LIST, m_peerlist);

    DDX_Text(pDX, IDC_TOTAL_HTTP, m_total_http);
    DDX_Text(pDX, IDC_TOTAL_P2P, m_total_p2p);
    DDX_Text(pDX, IDC_SAVE_RATE, m_save_rate);
    DDX_Text(pDX, IDC_IP, m_strIP);
    DDX_Text(pDX, IDC_LOCATION, m_strLocation);
    DDX_Text(pDX, IDC_HTTP_SERVER_STATUS, m_http_server_status);

    DDX_Text(pDX, IDC_HTTP_KBPS, m_http_kbps);
    DDX_Text(pDX, IDC_P2P_KBPS, m_p2p_kbps);

    DDX_Text(pDX, IDC_FILELEN, m_filelen);
    DDX_Text(pDX, IDC_FILENAME, m_filename);
#if !_PUBLIC_VERSION_
    DDX_Text(pDX, IDC_SEND, m_Send);
    DDX_Text(pDX, IDC_RECEIVE, m_Receive);
    DDX_Text(pDX, IDC_CONNECT_COUNT, m_peer_connect_count);
    DDX_Text(pDX, IDC_DATARATE, m_datarate);
    DDX_Text(pDX, IDC_SWITCH_STATE, m_switch_state);
    DDX_Control(pDX, IDC_SWITCH_HTTP, m_switch_http);
    DDX_Control(pDX, IDC_SWITCH_P2P, m_switch_p2p);
    DDX_Text(pDX, IDC_RE_RATE, m_re_rate);
    DDX_Text(pDX, IDC_LOST_RATE, m_lost_rate);
    DDX_Text(pDX, IDC_CONSISTENT, m_NonConsistentSize);
    DDX_Text(pDX, IDC_REST_TIME, m_rest_time);
    DDX_Text(pDX, IDC_SPEED_LIMIT, m_speed_limit);
    DDX_Text(pDX, IDC_BAND, m_bandwidth);
#endif

    DDX_Text(pDX, IDC_CONNECT_KICK, m_strConnectKick);
    DDX_Text(pDX, IDC_GLOBAL_WINDOW_SIZE, m_GlobalWindowSize);
    DDX_Text(pDX, IDC_GLOBAL_REQUEST_SEND_COUNT, m_GlobalRequestSendCount);
    DDX_Text(pDX, IDC_MEMORY_POOL_LEFT_SIZE, m_MemoryPoolLeftSize);
    DDX_Text(pDX, IDC_EMPTY_DISTANCE, m_empty_subpiece_distance);
    DDX_Text(pDX, IDC_P2P_CURRENT, m_p2p_sn_kbps);
    DDX_Text(pDX, IDC_HTTP_CURRENT, m_p2p_peer_kbps);
    DDX_Text(pDX, IDC_TOTAL_SN, m_total_sn);
    DDX_Text(pDX, IDC_SN_STATE, m_sn_state);
}


BEGIN_MESSAGE_MAP(CP2PDlg, CDialog)
    ON_LBN_SELCHANGE(IDC_RID_LIST, &CP2PDlg::OnLbnSelchangeRidList)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST, &CP2PDlg::OnLvnColumnclickList)
#if !_PUBLIC_VERSION_
    ON_BN_CLICKED(IDC_SWITCH_HP, &CP2PDlg::OnBnClickedSwitchHp)
    ON_BN_CLICKED(IDC_SWITCH_H, &CP2PDlg::OnBnClickedSwitchH)
    ON_BN_CLICKED(IDC_SWITCH_P, &CP2PDlg::OnBnClickedSwitchP)
    ON_BN_CLICKED(IDC_RESET, &CP2PDlg::OnBnClickedReset)
#endif
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CP2PDlg 消息处理程序

void CP2PDlg::OnShareMemoryTimer()
{
    if (FALSE == m_is_online && 0 == import_file_type)
    {
        if(!m_is_pause)
        {
            if(m_line_number < m_import_file.size() - 1)
            {
                ++m_line_number;
                ReadFile();
            }
        }
        else if (m_line_number > 0 && m_line_number != last_line_number_)
        {
            ReadFile();
            last_line_number_ = m_line_number;
        }
    }
    else
    {
        CString strLastGuid;

        
        // 保存上次选择
        int index = m_ridlist.GetCurSel();
        if (index >= 0)
        {
            m_ridlist.GetText(index, strLastGuid);
        }

        int count = m_ridlist.GetCount();
        

        // 读共享内存并添加
        if (TRUE == m_is_online)
        {
            m_ridlist.ResetContent();
            ClearContent();

            if (((CP2PMonitorDlg*)GetParent()->GetParent())->ReadShareMemory("PPVIDEO_" + m_strProcessID, sizeof(m_si), &m_si))
            {
                DOWNLOADDRIVER_STATISTIC_INFO_EX dd_info_ex = DOWNLOADDRIVER_STATISTIC_INFO_EX(100);
                DOWNLOADDRIVER_STATISTIC_INFO& dd_info = dd_info_ex.Ref();

                for (int i=0; i<256; i++)
                {
                    if (m_si.DownloadDriverIDs[i] != 0)
                    {
                        CString str;
                        str.Format("%d", m_si.DownloadDriverIDs[i]);
                        if (((CP2PMonitorDlg*)GetParent()->GetParent())->ReadShareMemory("DOWNLOADDRIVER_" + m_strProcessID + "_" + str, dd_info_ex.Size(), &dd_info))
                        {
                            char buf[MAX_PATH] = "";
                            GUID guid = dd_info.ResourceID;
                            // if (guid != GUID_NULL)
                            // {
                                sprintf(buf, "%d_%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X", i, guid.Data1, guid.Data2, guid.Data3, guid.Data4[0],guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
                                m_ridlist.InsertString(m_ridlist.GetCount(), buf);
                            // }

                            // 如果和上次选择相同，默认选中
                            if (strLastGuid.CompareNoCase(buf) == 0)
                            {
                                for(int i=0; i<m_ridlist.GetCount(); i++)
                                {
                                    CString strtmp;
                                    m_ridlist.GetText(i, strtmp);

                                    if (strLastGuid.CompareNoCase(strtmp) == 0)
                                    {
                                        m_ridlist.SetCurSel(i);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (1 == import_file_type && !m_is_pause && m_second < m_statistics_datas.size())
        {
            m_ridlist.ResetContent();
            ClearContent();

            for (int i = 0; i < m_statistics_datas[m_second]->GetVodDownloadDriverStatistics().size(); ++i)
            {
                //DOWNLOADDRIVER_STATISTIC_INFO dd_info(m_statistics_datas[m_second]->GetVodDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo());
            
                char buf[MAX_PATH] = "";
                std::string rid = m_statistics_datas[m_second]->GetVodDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo().ResourceID.to_string();
                sprintf(buf, "%d_%s", i, rid.c_str());
                m_ridlist.InsertString(m_ridlist.GetCount(), buf);


                // 如果和上次选择相同，默认选中
                if (strLastGuid.CompareNoCase(buf) == 0)
                {
                    for(int i=0; i<m_ridlist.GetCount(); i++)
                    {
                        CString strtmp;
                        m_ridlist.GetText(i, strtmp);

                        if (strLastGuid.CompareNoCase(strtmp) == 0)
                        {
                            m_ridlist.SetCurSel(i);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            return;
        }

        if(count < m_ridlist.GetCount())
        {
            count = m_ridlist.GetCount();

            CString temp, max;
            int cur = 0;
            m_ridlist.GetText(0, temp);
            max = temp;

            for(int i = 1; i < count; ++i)
            {
                m_ridlist.GetText(i, temp);
                if(max < temp)
                {
                    max = temp;
                    cur = i;
                }
            }
            m_ridlist.SetCurSel(cur);
        }

        // 如果列表中只有一项，默认选中
        if (m_ridlist.GetCount() == 1)
        {
            m_ridlist.SetCurSel(0);
        }
        else if (m_ridlist.GetCount() == 0 && m_is_online)
        {
            MonitorFillZero();
        }

        // 如果存在选中，则 持续 更新数据
        index = m_ridlist.GetCurSel();
        if (index >= 0)
        {
            // 更新数据
            OnLbnSelchangeRidList();
        }
    }
}

void CP2PDlg::SetProcessID(CString processid)
{
    m_strProcessID = processid;
}

void CP2PDlg::InitListControl()
{
    m_peerlist.InsertColumn(0, "DetectIP");
    m_peerlist.InsertColumn(1, "IP");
    m_peerlist.InsertColumn(2, "Location");
    m_peerlist.InsertColumn(3, "Down");
#if !_PUBLIC_VERSION_
    m_peerlist.InsertColumn(4, "Window Size");
    m_peerlist.InsertColumn(5, "RTT AVG");
    m_peerlist.InsertColumn(6, "RTT MAX");
    m_peerlist.InsertColumn(7, "AVG DELTA");
    m_peerlist.InsertColumn(8, "LOST RATE");
    m_peerlist.InsertColumn(9, "Assign");
    m_peerlist.InsertColumn(10, "NAT TYPE");
    m_peerlist.InsertColumn(11, "Guid");
     m_peerlist.InsertColumn(12, "TOTAL REQUEST COUNT");
    m_peerlist.InsertColumn(13, "REQUESTING COUNT");
    m_peerlist.InsertColumn(14, "TOTAL RECEIVE COUNT");
     m_peerlist.InsertColumn(15, "ASSIGN LEFT SUB COUNT");
#endif
    m_peerlist.InsertColumn(16, "TRACKER PRIORITY");

    m_peerlist.SetColumnWidth(0, 100);
    m_peerlist.SetColumnWidth(1, 100);
    m_peerlist.SetColumnWidth(2, 150);
    m_peerlist.SetColumnWidth(3, 60);
#if !_PUBLIC_VERSION_
    m_peerlist.SetColumnWidth(4, 80);
    m_peerlist.SetColumnWidth(5, 60);
    m_peerlist.SetColumnWidth(6, 60);
    m_peerlist.SetColumnWidth(7, 70);
    m_peerlist.SetColumnWidth(8, 70);
    m_peerlist.SetColumnWidth(9, 60);
    m_peerlist.SetColumnWidth(10, 90);
    m_peerlist.SetColumnWidth(11, 150);
     m_peerlist.SetColumnWidth(12, 100);
    m_peerlist.SetColumnWidth(13, 100);
    m_peerlist.SetColumnWidth(14, 100);
     m_peerlist.SetColumnWidth(15, 100);
#endif
    m_peerlist.SetColumnWidth(16, 100);
}

void CP2PDlg::OnLbnSelchangeRidList()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_ridlist.GetCount() == 0)
    {
        return;
    }

    int index = 0;
    CString rid;

    index = m_ridlist.GetCurSel();
    if (index >= 0 && m_list_update)
    {
        m_ridlist.GetText(index, rid);

        int pos = rid.Find('_');

        rid = rid.Mid(pos+1);

        if (TRUE == m_is_online)
        {
            for (int i=0; i<256; i++)
            {
                if (m_si.DownloadDriverIDs[i] != 0)
                {
                    CString str;
                    str.Format("%d", m_si.DownloadDriverIDs[i]);

                    DOWNLOADDRIVER_STATISTIC_INFO_EX dd_info_ex = DOWNLOADDRIVER_STATISTIC_INFO_EX(100);
                    DOWNLOADDRIVER_STATISTIC_INFO& dd_info = dd_info_ex.Ref();

                    if (((CP2PMonitorDlg*)GetParent()->GetParent())->ReadShareMemory("DOWNLOADDRIVER_" + m_strProcessID + "_" + str, dd_info_ex.Size(), &dd_info))
                    {
                        char buf[MAX_PATH] = "";
                        sprintf(buf, "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X", dd_info.ResourceID.Data1, dd_info.ResourceID.Data2, dd_info.ResourceID.Data3, 
                            dd_info.ResourceID.Data4[0], dd_info.ResourceID.Data4[1], 
                            dd_info.ResourceID.Data4[2], dd_info.ResourceID.Data4[3], 
                            dd_info.ResourceID.Data4[4], dd_info.ResourceID.Data4[5],
                            dd_info.ResourceID.Data4[6], dd_info.ResourceID.Data4[7]);

                        if (rid.CompareNoCase(buf) != 0)
                        {
                            continue;
                        }

                        wchar_t *filename = (wchar_t*)(dd_info.FileName);
                        char ansiname[MAX_PATH] = "";
                        WideCharToMultiByte(CP_ACP, 0, filename, -1, ansiname, MAX_PATH, NULL, NULL);

                        if (strlen(ansiname) == 0)
                        {
                            m_filename.Format("%s", "视频名称未知");
                        }
                        else
                        {
                            m_filename.Format("%s", ansiname);
                        }

                        int http_downloader_index = 0;
                        while (http_downloader_index < dd_info.HttpDownloaderCount && dd_info.HttpDownloaders[http_downloader_index].IsPause == 1)
                        {
                            ++http_downloader_index;
                        }
                        if (http_downloader_index >= dd_info.HttpDownloaderCount)
                        {
                            http_downloader_index = 0;
                        }

                        m_sn_state=dd_info.sn_state;
                        m_http_server_status = dd_info.HttpDownloaders[http_downloader_index].LastHttpStatusCode;

                        char *ip = (char*)(dd_info.HttpDownloaders[http_downloader_index].Url);

                        m_strIP.Format("%s", ip);
                        int pos = m_strIP.Find('//', 0);
                        m_strIP = m_strIP.Mid(pos+1);

                        pos = m_strIP.Find('//', 0);
                        m_strIP = m_strIP.Mid(pos+1);

                        pos = m_strIP.Find('//', 0);
                        m_strIP = m_strIP.Mid(0, pos);

                        m_rest_time = dd_info.t;
                        m_bandwidth = dd_info.b;
                        m_speed_limit = dd_info.speed_limit;

                        if (dd_info_ex.Ref().http_state == 2 || (dd_info_ex.Ref().http_state == 3 && dd_info.p2p_state == 0))
                        {
                            if (dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.TotalDownloadBytes / 1024 != 0)
                            {
                                m_total_http = dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.TotalDownloadBytes / 1024;
                            }
                        }

                        string loc_result;

                        loc_result = m_locater.getIpAddr(string(m_strIP.GetBuffer()));
                        m_strLocation.Format("%s", loc_result.c_str());

                        if (rid.CompareNoCase(buf) == 0)
                        {
                            m_http_kbps = dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.NowDownloadSpeed / 1024;
                            m_datarate = dd_info_ex.Ref().DataRate;
                            m_switch_state.Format("%d%d%d%d", dd_info_ex.Ref().http_state, dd_info_ex.Ref().p2p_state,
                                dd_info_ex.Ref().timer_using_state, dd_info_ex.Ref().timer_state);

                            if (dd_info_ex.Ref().http_state == 2)
                            {
                                // HTTP ON
                                m_switch_http.SetIcon(h_on);
                            }
                            else
                            {
                                // HTTP OFF
                                m_switch_http.SetIcon(h_off);
                            }

                            if (dd_info_ex.Ref().p2p_state == 2)
                            {
                                // P2P ON
                                m_switch_p2p.SetIcon(h_on);
                            }
                            else
                            {
                                // P2P OFF
                                m_switch_p2p.SetIcon(h_off);
                            }
                        }

                        UpdateData(FALSE);
                    }
                }
            }

            P2PDOWNLOADER_STATISTIC_INFO_EX p2p_info_ex = P2PDOWNLOADER_STATISTIC_INFO_EX(100);

            P2PDOWNLOADER_STATISTIC_INFO& p2p_info = p2p_info_ex.Ref();

            if (((CP2PMonitorDlg*)GetParent()->GetParent())->ReadShareMemory("P2PDOWNLOADER_" + m_strProcessID + "_" + rid, p2p_info_ex.Size(), &p2p_info))
            {
                m_filelen = p2p_info.FileLength;

                m_Send = p2p_info.TotalRequestSubPieceCount;
                m_Receive = p2p_info.TotalRecievedSubPieceCount;
                int total_receive = p2p_info.TotalUnusedSubPieceCount;

                m_NonConsistentSize = p2p_info.NonConsistentSize;

                int total_requestint_count = 0;
                for (int i=0; i<m_last_pc_count; i++)
                {
                    total_requestint_count += p2p_info_ex.Ref().P2PConnections[i].Requesting_Count;
                }

                m_lost_rate.Format("%.2f%%", (m_Send - total_requestint_count - total_receive)* 100.0 / (m_Send - total_requestint_count + 0.000001));
                m_re_rate.Format("%.2f%%", (total_receive - m_Receive) * 100.0 / (total_receive+0.0000001));

                m_p2p_kbps = p2p_info.SpeedInfo.NowDownloadSpeed / 1024;
                m_p2p_peer_kbps = p2p_info.PeerSpeedInfo.NowDownloadSpeed / 1024;
                m_p2p_sn_kbps = p2p_info.SnSpeedInfo.NowDownloadSpeed / 1024;

                full_block_peer_count = p2p_info_ex.Ref().FullBlockPeerCount;
                peer_count = p2p_info_ex.Ref().PeerCount;
                ip_pool_peer_count = p2p_info_ex.Ref().IpPoolPeerCount;

                m_peer_connect_count.Format("%d - %d - %d", p2p_info_ex.Ref().FullBlockPeerCount,
                    p2p_info_ex.Ref().PeerCount, p2p_info_ex.Ref().IpPoolPeerCount);

                m_total_p2p = p2p_info.TotalP2PDataBytes / 1024;
                m_total_sn = p2p_info.TotalP2PSnDataBytes / 1024;

                m_strConnectKick.Format("%d - %d", p2p_info_ex.Ref().ConnectCount, p2p_info_ex.Ref().KickCount);

                m_GlobalWindowSize = m_si.GlobalWindowSize;

                m_GlobalRequestSendCount = m_si.GlobalRequestSendCount;

                m_MemoryPoolLeftSize = m_si.MemoryPoolLeftSize;

                m_empty_subpiece_distance = p2p_info.empty_subpiece_distance;

                m_last_pc_count = p2p_info_ex.Ref().PeerCount;
                for (int i=0; i<m_last_pc_count; i++)
                {
                    m_last_pc[i] = p2p_info_ex.Ref().P2PConnections[i];
                }

                ShowReport();
            }
            else
            {
                m_total_p2p = 0;            // P2P总下载
                m_total_sn = 0;
                m_p2p_kbps = 0;                // P2P速度
                m_peer_connect_count = "";    // 连接
                m_peerlist.DeleteAllItems();

                m_Send = 0;                    // 总发送
                m_Receive = 0;                // 总收到
                m_NonConsistentSize = 0;    // 距离
                m_re_rate = "";                // 冗余率
                m_lost_rate = "";            // 丢包率
            }

            m_save_rate.Format("%.2f%%", m_total_p2p * 100.0 / (m_total_p2p + m_total_http + m_total_sn));
        }
        else if (1 == import_file_type)
        {
            Display(rid);
        }

        if (m_is_saving)
        {
            Export();
        }

        UpdateData(FALSE);           
    }
}

void CP2PDlg::OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_sort_col = pNMLV->iSubItem;

    if (m_sort == 0)
    {
        m_sort = -1;
    }

    m_sort = -1 * m_sort;

    ShowReport();

    *pResult = 0;
}

void CP2PDlg::SetListReportState(BOOL is_list_update)
{
    m_list_update = is_list_update;
}


void CP2PDlg::Export()
{
    char strDefaultDir[MAX_PATH] = {0};
    char strDocument[MAX_PATH] = {0};
    char strDocumentcsv[MAX_PATH] = {0};
    LPITEMIDLIST pidl = NULL;
    SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &pidl);
    if (pidl && GetCurrentDirectory( MAX_PATH, strDocument) && GetCurrentDirectory( MAX_PATH, strDocumentcsv))
    {
        strcat(strDocument, "\\pplog.txt");
    
        if (m_is_start)
        {
            CreateCSVFileName();
        }
        
        strcat(strDocumentcsv, csvFileName.c_str());
        csvFilePath.Format("%s", strDocumentcsv);
        FilePathName = csvFilePath;
    }

    FILE *fpcsv;   

    if ((fpcsv = fopen(strDocumentcsv, "a+")) != NULL)
    {
        if(m_is_start)
        {
            m_is_start = false;
        #if !_USER_VERSION_
            fprintf(fpcsv, "%s,", "文件名");
            fprintf(fpcsv, "%s,", "文件长度");
            fprintf(fpcsv, "%s,", "码流");
            fprintf(fpcsv, "%s,", "CDN IP");
            fprintf(fpcsv, "%s,", "当前时间");
            fprintf(fpcsv, "%s,", "总发送");
            fprintf(fpcsv, "%s,", "总收到");
            fprintf(fpcsv, "%s,", "丢包率");
            fprintf(fpcsv, "%s,", "冗余率");
            fprintf(fpcsv, "%s,", "http总下载");
            fprintf(fpcsv, "%s,", "p2p总下载");
            fprintf(fpcsv, "%s,", "剩余时间(s)");
            fprintf(fpcsv, "%s,", "http速度");
            fprintf(fpcsv, "%s,", "p2p速度");
            fprintf(fpcsv, "%s,", "总下载速度");
            fprintf(fpcsv, "%s,", "状态机");
            fprintf(fpcsv, "%s,", "连接数");
            fprintf(fpcsv, "%s,", "满资源的节点数");
            fprintf(fpcsv, "%s,", "连接节点数");
            fprintf(fpcsv, "%s,", "IPPool");
            fprintf(fpcsv, "%s,", "P2P Peer 速度");
            fprintf(fpcsv, "%s\n", "P2P SN 速度");
        #endif
        }

        time_t now = time(0);
        char str_now[MAX_PATH]; 

        if(m_strIP == "ppsucai.pptv.com")
        {
            fclose(fpcsv);
            return;
        }

        //CString temp_str;
        //temp_str.Format("%s,%d,%d,%s,%s,%d,%d,%s,%s,%d,%d,%d,%d,%d,%d,%s,%s")
        fprintf(fpcsv, "%s", m_filename); //文件名
        fprintf(fpcsv, ",%d", m_filelen);  //文件长度
        fprintf(fpcsv, ",%d", m_datarate);    //码流
        fprintf(fpcsv, ",%s", m_strIP);    //CDN IP

        strftime(str_now, sizeof(str_now), "%Y/%m/%d %X",localtime(&now)); 
        fprintf(fpcsv, ",%s", str_now);    //当前时间
        
        fprintf(fpcsv, ",%d", m_Send);        //总发送
        fprintf(fpcsv, ",%d", m_Receive);    //总收到
        fprintf(fpcsv, ",%s", m_lost_rate);//丢包率
        fprintf(fpcsv, ",%s", m_re_rate);    //冗余率
        

        
        fprintf(fpcsv, ",%d", m_total_http);//http总下载
        fprintf(fpcsv, ",%d", m_total_p2p); //p2p总下载
        
        fprintf(fpcsv, ",%d", m_rest_time / 1000);//剩余时间
        fprintf(fpcsv, ",%d", m_http_kbps); //http速度
        fprintf(fpcsv, ",%d", m_p2p_kbps);  //p2p速度
        fprintf(fpcsv, ",%d", m_http_kbps + m_p2p_kbps);  //总下载速度
        fprintf(fpcsv, ",%s", m_switch_state);//状态机
        fprintf(fpcsv, ",%s", m_peer_connect_count);    //连接数
        fprintf(fpcsv, ",%d", full_block_peer_count);   //满资源数
        fprintf(fpcsv, ",%d", peer_count);              //连接上的peer数
        fprintf(fpcsv, ",%d", ip_pool_peer_count);      //备选peer数
        fprintf(fpcsv, ",%d", m_p2p_peer_kbps);      //p2p peer 速度
        fprintf(fpcsv, ",%d", m_p2p_sn_kbps);      //p2p sn 速度
        fprintf(fpcsv, ",%d\n", m_total_sn);       // sn 总下载
        fclose(fpcsv);
    }
#if _PPLOG_
    FILE *fp;
    if ((fp = fopen(strDocument, "a+")) != NULL)
    {
        time_t now = time(0);
        char str_now[MAX_PATH]; 
        strftime(str_now, sizeof(str_now), "%Y/%m/%d %X",localtime(&now)); 
        fprintf(fp, "当前时间  : %s\n", str_now);
        fprintf(fp, "--------------------资源信息--------------------\n");
        fprintf(fp, "文件名    : %s\n", m_filename);
        fprintf(fp, "文件长度  : %d\n", m_filelen);
        fprintf(fp, "--------------------SubPiece信息----------------\n");
        fprintf(fp, "总发送    : %d\n", m_Send);
        fprintf(fp, "总收到    : %d\n", m_Receive);
        fprintf(fp, "丢包率    : %s\n", m_lost_rate);
        fprintf(fp, "冗余率    : %s\n", m_re_rate);
        fprintf(fp, "--------------------带宽节约比------------------\n");
        fprintf(fp, "HTTP总下载: %d\n", m_total_http);
        fprintf(fp, "P2P总下载 : %d\n", m_total_p2p);
        fprintf(fp, "SN总下载 : %d\n", m_total_sn);
        fprintf(fp, "带宽节约比: %s\n", m_save_rate);
        fprintf(fp, "--------------------速度 & 状态机--------------------\n");
        fprintf(fp, "状态机    : %s\n", m_switch_state);
        fprintf(fp, "HTTP速度  : %d\n", m_http_kbps);
        fprintf(fp, "P2P 速度  : %d\n", m_p2p_kbps);
        fprintf(fp, "P2P Peer速度  : %d\n", m_p2p_peer_kbps);
        fprintf(fp, "P2P SN 速度  : %d\n", m_p2p_sn_kbps);
        fprintf(fp, "P2P 连接  : %s\n", m_peer_connect_count);
        fprintf(fp, "码流率    : %d\n", m_datarate);
        fprintf(fp, "--------------------Peer节点信息--------------------\n");
        for(int i=0; i<m_last_pc_count; i++)
        {
            P2P_CONNECTION_INFO pcinfo = m_last_pc[i];
            CString str;
            char strIP[MAX_PATH];

            // DetectIP
            Value2IP(pcinfo.PeerInfo.DetectIP, strIP);
            fprintf(fp, "Detect IP: %s ", strIP);

            // IP
            Value2IP(pcinfo.PeerInfo.IP, strIP);
            fprintf(fp, "IP: %s ", strIP);

            // 地域
            if (pcinfo.PeerInfo.DetectIP == 0)
            {
                Value2IP(pcinfo.PeerInfo.IP, strIP);
            }
            else
            {
                Value2IP(pcinfo.PeerInfo.DetectIP, strIP);
            }

            string loc_result;
            loc_result = m_locater.getIpAddr(string(strIP));
            fprintf(fp, "地域: %s ", loc_result.c_str());
            fprintf(fp, "Window Size: %d ", pcinfo.WindowSize);
            fprintf(fp, "Down: %d ", pcinfo.SpeedInfo.NowDownloadSpeed);
            fprintf(fp, "RTT_Average: %d ", pcinfo.RTT_Average);
            fprintf(fp, "RTT_Max: %d ", pcinfo.RTT_Max);
            fprintf(fp, "AvgDeltaTime: %d ", pcinfo.AverageDeltaTime);

            double rate = (pcinfo.Sent_Count - pcinfo.Requesting_Count - pcinfo.Received_Count) * 100.0 / (pcinfo.Sent_Count - pcinfo.Requesting_Count + 0.000001);
            fprintf(fp, "Lost Rate: %f ", rate);

            fprintf(fp, "Assign: %d ", pcinfo.AssignedSubPieceCount);
            fprintf(fp, "Sent_Count: %d ", pcinfo.Sent_Count);
            fprintf(fp, "Requesting_Count: %d ", pcinfo.Requesting_Count);
            fprintf(fp, "Received_Count: %d ", pcinfo.Received_Count);
            fprintf(fp, "AssignedLeftSubPieceCount: %d ", pcinfo.AssignedLeftSubPieceCount);
            fprintf(fp, "\n");
        }
        fprintf(fp, "--------------------连-踢--------------------\n");
        fprintf(fp, "连-踢: %s\n", m_strConnectKick);
        fprintf(fp, "\n\n");
        fclose(fp);
    }
#endif
}

void CP2PDlg::SetSaving(BOOL is_saving)
{
    m_is_saving = is_saving;
}

void CP2PDlg::MonitorFillZero()
{
    m_filelen = 0;
    m_total_http = 0;
    m_total_p2p = 0;
    m_total_sn = 0;
    m_save_rate = "";
    m_Send = 0; 
    m_Receive = 0; 
    m_peerlist.DeleteAllItems();
    m_http_kbps = 0;
    m_p2p_kbps = 0;
    m_p2p_peer_kbps = 0;
    m_p2p_sn_kbps = 0;
    m_peer_connect_count = "";
    m_datarate = 0;
    m_switch_state = "";
    m_switch_http.SetIcon(h_off);
    m_switch_p2p.SetIcon(h_off);
    m_re_rate = "";
    m_lost_rate = "";
    m_rest_time = 0;
    m_speed_limit = 0;
    m_bandwidth = 0;
    m_strIP = "";
    m_strLocation = "";
    m_http_server_status = 0;
    m_filename = "";
    m_NonConsistentSize = 0;
    m_strIP = "";
    m_strLocation = "";
    m_http_server_status = 0;
    m_filelen = 0;
    m_filename = "";
    m_datarate = 0;
    m_strConnectKick = "";
    m_sn_state=0;
    m_ridlist.ResetContent();

    UpdateData(FALSE);
}

void CP2PDlg::OnBnClickedSwitchHp()
{
    // TODO: 在此添加控件通知处理程序代码
    ControlSwitch(2200);
}

void CP2PDlg::OnBnClickedSwitchH()
{
    // TODO: 在此添加控件通知处理程序代码
    ControlSwitch(2300);
}

void CP2PDlg::OnBnClickedSwitchP()
{
    // TODO: 在此添加控件通知处理程序代码
    ControlSwitch(3200);
}

void CP2PDlg::ControlSwitch(int type)
{
    FILE *fp;

    char current_dir[512];
    GetCurrentDirectory(512, current_dir);

    SetCurrentDirectory("C:\\Program Files\\Common Files\\PPLiveNetwork");
    
    if ((fp = fopen("Switch.ini", "w+")) != NULL)
    {
        fprintf(fp, "%d", type);
        fclose(fp);
    }

    SetCurrentDirectory(current_dir);
}
void CP2PDlg::OnBnClickedReset()
{
    // TODO: 在此添加控件通知处理程序代码
    char current_dir[512];
    GetCurrentDirectory(512, current_dir);

    SetCurrentDirectory("C:\\Program Files\\Common Files\\PPLiveNetwork\\");
    DeleteFile("Switch.ini");

    SetCurrentDirectory(current_dir);
}

void CP2PDlg::ShowReport()
{
    for(int i=0; i<m_last_pc_count-1; i++)
    {
        for(int j=i+1; j<m_last_pc_count; j++)
        {
            bool is_swap = false;

            if (m_sort_col == 0)
            {
                // DetectIP
                UINT32 a = m_last_pc[i].PeerInfo.DetectIP;
                UINT32 b = m_last_pc[j].PeerInfo.DetectIP;
                if (m_sort < 0 && a < b)
                {
                    is_swap = true;
                }
                else if (m_sort > 0 && a > b)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 1)
            {
                // IP
                UINT32 a = m_last_pc[i].PeerInfo.IP;
                UINT32 b = m_last_pc[j].PeerInfo.IP;

                if (m_sort < 0 && a < b)
                {
                    is_swap = true;
                }
                else if (m_sort > 0 && a > b)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 2)
            {
                char strIP[MAX_PATH];
                // 地域
                if (m_last_pc[i].PeerInfo.DetectIP == 0)
                {
                    Value2IP(m_last_pc[i].PeerInfo.IP, strIP);
                }
                else
                {
                    Value2IP(m_last_pc[i].PeerInfo.DetectIP, strIP);
                }

                string loc_result1;
                loc_result1 = m_locater.getIpAddr(string(strIP));

                if (m_last_pc[j].PeerInfo.DetectIP == 0)
                {
                    Value2IP(m_last_pc[j].PeerInfo.IP, strIP);
                }
                else
                {
                    Value2IP(m_last_pc[j].PeerInfo.DetectIP, strIP);
                }

                string loc_result2;
                loc_result2 = m_locater.getIpAddr(string(strIP));
                
                if (m_sort < 0 && loc_result1 < loc_result2)
                {
                    is_swap = true;
                }
                else if (m_sort > 0 && loc_result1 > loc_result2)
                {
                    is_swap = true;
                }
            }

            

            if (m_sort_col == 3)
            {
                // Down
                int a = m_last_pc[i].SpeedInfo.NowDownloadSpeed;
                int b = m_last_pc[j].SpeedInfo.NowDownloadSpeed;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 4)
            {
                // window_size
                int a = m_last_pc[i].WindowSize;
                int b = m_last_pc[j].WindowSize;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 5)
            {
                // RTT AVG
                int a = m_last_pc[i].RTT_Average;
                int b = m_last_pc[j].RTT_Average;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 6)
            {
                // RTT MAX
                int a = m_last_pc[i].RTT_Max;
                int b = m_last_pc[j].RTT_Max;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 7)
            {
                // AVG DELTA
                int a = m_last_pc[i].AverageDeltaTime;
                int b = m_last_pc[j].AverageDeltaTime;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 8)
            {
                // LOST RATE
                float a = (m_last_pc[i].Sent_Count - m_last_pc[i].Requesting_Count - m_last_pc[i].Received_Count) * 1.0 / (m_last_pc[i].Sent_Count - m_last_pc[i].Requesting_Count + 0.000001);
                float b = (m_last_pc[j].Sent_Count - m_last_pc[j].Requesting_Count - m_last_pc[j].Received_Count) * 1.0 / (m_last_pc[j].Sent_Count - m_last_pc[j].Requesting_Count + 0.000001);

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 9)
            {
                // ASSIGN
                int a = m_last_pc[i].AssignedSubPieceCount;
                int b = m_last_pc[j].AssignedSubPieceCount;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 10)
            {
                // ASSIGN
                int a = m_last_pc[i].PeerInfo.PeerNatType;
                int b = m_last_pc[j].PeerInfo.PeerNatType;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }


            if (m_sort_col == 12)
            {
                // Sent_Count
                int a = m_last_pc[i].Sent_Count;
                int b = m_last_pc[j].Sent_Count;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 13)
            {
                // Requesting_Count
                int a = m_last_pc[i].Requesting_Count;
                int b = m_last_pc[j].Requesting_Count;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 14)
            {
                // Received_Count
                int a = m_last_pc[i].Received_Count;
                int b = m_last_pc[j].Received_Count;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 15)
            {
                // AssignedLeftSubPieceCount
                int a = m_last_pc[i].AssignedLeftSubPieceCount;
                int b = m_last_pc[j].AssignedLeftSubPieceCount;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 16)
            {
                // TrackerPriority
                int a = m_last_pc[i].PeerInfo.TrackerPriority;
                int b = m_last_pc[j].PeerInfo.TrackerPriority;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (is_swap)
            {
                P2P_CONNECTION_INFO tmp = m_last_pc[i];
                m_last_pc[i] = m_last_pc[j];
                m_last_pc[j] = tmp;
            }
        }
    }

    POSITION pos = m_peerlist.GetFirstSelectedItemPosition();
    if (pos) 
    {
        int sel = m_peerlist.GetNextSelectedItem(pos);
        m_sel_guid = m_peerlist.GetItemText(sel, 11);
    }

    m_peerlist.DeleteAllItems();

    for(int i=0; i<m_last_pc_count; i++)
    {
        P2P_CONNECTION_INFO pcinfo = m_last_pc[i];
        CString str;
        char strIP[MAX_PATH];

        // DetectIP
        Value2IP(pcinfo.PeerInfo.DetectIP, strIP);
        m_peerlist.InsertItem(i, strIP);

        // IP
        Value2IP(pcinfo.PeerInfo.IP, strIP);
        m_peerlist.SetItem(i, 1, LVIF_TEXT, strIP, 0, 0, 0, 0);

        // 地域
        if (pcinfo.PeerInfo.DetectIP == 0)
        {
            Value2IP(pcinfo.PeerInfo.IP, strIP);
        }
        else
        {
            Value2IP(pcinfo.PeerInfo.DetectIP, strIP);
        }

        string loc_result;
        loc_result = m_locater.getIpAddr(string(strIP));
        str.Format("%s", loc_result.c_str());
        m_peerlist.SetItem(i, 2, LVIF_TEXT, str, 0, 0, 0, 0);

        // Down
        str.Format("%d", pcinfo.SpeedInfo.NowDownloadSpeed);
        m_peerlist.SetItem(i, 3, LVIF_TEXT, str, 0, 0, 0, 0);
#if !_PUBLIC_VERSION_
        // WindowSize
        str.Format("%d", pcinfo.WindowSize);
        m_peerlist.SetItem(i, 4, LVIF_TEXT, str, 0, 0, 0, 0);

        // RTT_AVERAGE
        str.Format("%d", pcinfo.RTT_Average);
        m_peerlist.SetItem(i, 5, LVIF_TEXT, str, 0, 0, 0, 0);

        // RTT_MAX
        str.Format("%d", pcinfo.RTT_Max);
        m_peerlist.SetItem(i, 6, LVIF_TEXT, str, 0, 0, 0, 0);

        // AVG_DELTA_TIME
        str.Format("%d", pcinfo.AverageDeltaTime);
        m_peerlist.SetItem(i, 7, LVIF_TEXT, str, 0, 0, 0, 0);

        // LOST RATE
        double rate = (pcinfo.Sent_Count - pcinfo.Requesting_Count - pcinfo.Received_Count) * 100.0 / (pcinfo.Sent_Count - pcinfo.Requesting_Count + 0.000001);
        str.Format("%.2f%%", rate);
        m_peerlist.SetItem(i, 8, LVIF_TEXT, str, 0, 0, 0, 0);

        // Assign
        str.Format("%d", pcinfo.AssignedSubPieceCount);
        m_peerlist.SetItem(i, 9, LVIF_TEXT, str, 0, 0, 0, 0);

        // NAT TYPE
        if (pcinfo.PeerInfo.PeerNatType == 255)
        {
            str.Format("%s", "ERROR");
        }

        if (pcinfo.PeerInfo.PeerNatType == 0)
        {
            str.Format("%s", "FULLCONENAT");
        }

        if (pcinfo.PeerInfo.PeerNatType == 1)
        {
            str.Format("%s", "IP_RESTRICTEDNAT");
        }

        if (pcinfo.PeerInfo.PeerNatType == 2)
        {
            str.Format("%s", "IP_PORT_RESTRICTEDNAT");
        }

        if (pcinfo.PeerInfo.PeerNatType == 3)
        {
            str.Format("%s", "SYMNAT");
        }

        if (pcinfo.PeerInfo.PeerNatType == 4)
        {
            str.Format("%s", "PUBLIC");
        }
#endif

        m_peerlist.SetItem(i, 10, LVIF_TEXT, str, 0, 0, 0, 0);

        char buf[MAX_PATH];
        _snprintf(buf, sizeof(buf)
                , "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X"
                , pcinfo.PeerGuid.Data1
                , pcinfo.PeerGuid.Data2
                , pcinfo.PeerGuid.Data3
                , pcinfo.PeerGuid.Data4[0], pcinfo.PeerGuid.Data4[1]
            , pcinfo.PeerGuid.Data4[2], pcinfo.PeerGuid.Data4[3]
            , pcinfo.PeerGuid.Data4[4], pcinfo.PeerGuid.Data4[5]
            , pcinfo.PeerGuid.Data4[6], pcinfo.PeerGuid.Data4[7]
        );
        
        str.Format("%s", buf);
        m_peerlist.SetItem(i, 11, LVIF_TEXT, str, 0, 0, 0, 0);

        if (str.Compare(m_sel_guid) == 0)
        {
            m_peerlist.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
        }

        // Sent_Count
        str.Format("%d", pcinfo.Sent_Count);
        m_peerlist.SetItem(i, 12, LVIF_TEXT, str, 0, 0, 0, 0);

        // Requesting_Count
        str.Format("%d", pcinfo.Requesting_Count);
        m_peerlist.SetItem(i, 13, LVIF_TEXT, str, 0, 0, 0, 0);

        // Received_Count
        str.Format("%d", pcinfo.Received_Count);
        m_peerlist.SetItem(i, 14, LVIF_TEXT, str, 0, 0, 0, 0);

        // AssignedLeftSubPieceCount
        str.Format("%d", pcinfo.AssignedLeftSubPieceCount);
        m_peerlist.SetItem(i, 15, LVIF_TEXT, str, 0, 0, 0, 0);

        // TrackerPriority
        str.Format("%d", pcinfo.PeerInfo.TrackerPriority);
        m_peerlist.SetItem(i, 16, LVIF_TEXT, str, 0, 0, 0, 0);
        
        
    }
}

void CP2PDlg::ClearContent()
{
    m_filelen = 0;

    m_total_http = 0;
    m_total_p2p = 0;
    m_total_sn = 0;
    m_save_rate = "";

    // 总发送
    m_Send = 0;
    // 总收到
    m_Receive = 0;

    m_http_kbps = 0;
    m_p2p_kbps = 0;
    m_p2p_peer_kbps = 0;
    m_p2p_sn_kbps = 0;
    
    m_datarate = 0;

    m_switch_state = "";

    m_filename = "";

    m_strIP = "";
    m_strLocation = "";

    // 冗余率
    m_re_rate = "";
    // 丢包率
    m_lost_rate = "";
    // HTTP服务器状态
    m_http_server_status = 0;

    m_NonConsistentSize = 0;

    m_rest_time = 0;
    m_speed_limit = 0;
    m_bandwidth = 0;
    m_sn_state=0;

    UpdateData(TRUE);
}

void CP2PDlg::CreateCSVFileName()
{
    time_t t = time(0);
    char tmp[64];
    strftime(tmp, 64, "%X", localtime(&t));

    for (int i=0; i<strlen(tmp); i++)
    {
        if (tmp[i] == ':')
            tmp[i] = '_';
    }
    csvFileName = "\\PeerMonitor_p2p.";
    for (int i = 0; i < 8; ++i)
    {
        csvFileName += tmp[i];
    }
    csvFileName = csvFileName + ".csv";
}

void CP2PDlg::SetStart()
{
    m_is_start = true;
}

CString CP2PDlg::getCSVFilePath()
{
    return csvFilePath;
}

void CP2PDlg::ReadFile()
{
    UpdateData(FALSE);
    int index1 = 0, index2;
    index2 = m_import_file[m_line_number].Find(',');
    m_filename = m_import_file[m_line_number].Mid(index1, index2 - index1);//文件名
    
    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_filelen = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//文件长度

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_datarate = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//码流

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_strIP = m_import_file[m_line_number].Mid(index1, index2 - index1);//CDN IP

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    //当前时间  跳过

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_Send = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//总发送

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_Receive = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//总收到

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_lost_rate = m_import_file[m_line_number].Mid(index1, index2 - index1);//丢包率

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_re_rate = m_import_file[m_line_number].Mid(index1, index2 - index1);//冗余率    

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_total_http = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//http总下载

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_total_p2p = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//p2p总下载

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_rest_time = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//剩余时间

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_http_kbps = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//http速度

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_p2p_kbps = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//p2p速度

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    //总下载速度  跳过

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_switch_state = m_import_file[m_line_number].Mid(index1, index2 - index1);//状态机

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_peer_connect_count = m_import_file[m_line_number].Mid(index1, index2 - index1);//连接数
}

void CP2PDlg::ReadStatisticData()
{
    UpdateData(FALSE);
    
}
void CP2PDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (m_is_created)
    {
        this->GetWindowRect(&m_rect);
        GetParent()->ScreenToClient(&m_rect);
        m_rect.right = m_rect.left + cx;
        m_rect.bottom = m_rect.top + cy;
        this->MoveWindow(&m_rect);
    }
    else
    {
        m_is_created = TRUE;
    }
}

void CP2PDlg::ChangeLabelSize(int cx, int cy, int w, int h)
{
    CWnd *pWnd;
    CRect rect;
    
    int j = m_label_info.size();
    for (int i = 0; i < m_label_info.size(); ++i)
    {
        pWnd = GetDlgItem(m_label_info[i].labelID);
        if(pWnd)
        {
            pWnd->GetWindowRect(&rect);
            this->ScreenToClient(&rect);
            rect.left = m_label_info[i].left_old * cx / w;
            rect.right = (m_label_info[i].right_old - m_label_info[i].left_old) * cx / w + rect.left;
            rect.top = m_label_info[i].top_old * cy / h;
            rect.bottom = (m_label_info[i].bottom_old - m_label_info[i].top_old) * cy / h + rect.top;
            pWnd->MoveWindow(rect);
        }
    }
}

void CP2PDlg::AddLabelInfo()
{
    m_label_id.push_back(IDC_BAND);
    m_label_id.push_back(IDC_CONNECT_COUNT);
    m_label_id.push_back(IDC_CONNECT_KICK);
    m_label_id.push_back(IDC_CONSISTENT);
    m_label_id.push_back(IDC_DATARATE);
    m_label_id.push_back(IDC_EMPTY_DISTANCE);
    m_label_id.push_back(IDC_FILELEN);
    m_label_id.push_back(IDC_FILENAME);
    m_label_id.push_back(IDC_GLOBAL_REQUEST_SEND_COUNT);
    m_label_id.push_back(IDC_GLOBAL_WINDOW_SIZE);
    m_label_id.push_back(IDC_HTTP_KBPS);
    m_label_id.push_back(IDC_HTTP_SERVER_STATUS);
    m_label_id.push_back(IDC_IP);
    m_label_id.push_back(IDC_LIST);
    m_label_id.push_back(IDC_LOCATION);
    m_label_id.push_back(IDC_LOST_RATE);
    m_label_id.push_back(IDC_MEMORY_POOL_LEFT_SIZE);
    m_label_id.push_back(IDC_P2P_KBPS);
    m_label_id.push_back(IDC_RE_RATE);
    m_label_id.push_back(IDC_RECEIVE);
    m_label_id.push_back(IDC_RESET);
    m_label_id.push_back(IDC_REST_TIME);
    m_label_id.push_back(IDC_RID_LIST);
    m_label_id.push_back(IDC_SAVE_RATE);
    m_label_id.push_back(IDC_SEND);
    m_label_id.push_back(IDC_SPEED_LIMIT);
    m_label_id.push_back(IDC_STATIC15);
    m_label_id.push_back(IDC_STATIC16);
    m_label_id.push_back(IDC_STATIC19);
    m_label_id.push_back(IDC_STATIC20);
    m_label_id.push_back(IDC_STATIC24);
    m_label_id.push_back(IDC_STATIC25);
    m_label_id.push_back(IDC_STATIC26);
    m_label_id.push_back(IDC_STATIC27);
    m_label_id.push_back(IDC_STATIC28);
    m_label_id.push_back(IDC_STATIC29);
    m_label_id.push_back(IDC_STATIC30);
    m_label_id.push_back(IDC_STATIC31);
    m_label_id.push_back(IDC_STATIC3333);
    m_label_id.push_back(IDC_STATIC3434);
    m_label_id.push_back(IDC_STATIC3535);
    m_label_id.push_back(IDC_STATIC3636);
    m_label_id.push_back(IDC_STATIC3737);
    m_label_id.push_back(IDC_STATIC8);
    m_label_id.push_back(IDC_SWITCH_H);
    m_label_id.push_back(IDC_SWITCH_HP);
    m_label_id.push_back(IDC_SWITCH_HTTP);
    m_label_id.push_back(IDC_SWITCH_P);
    m_label_id.push_back(IDC_SWITCH_P2P);
    m_label_id.push_back(IDC_SWITCH_STATE);
    m_label_id.push_back(IDC_TOTAL_HTTP);
    m_label_id.push_back(IDC_TOTAL_P2P);
    m_label_id.push_back(IDC_STATIC3);
    m_label_id.push_back(IDC_STATIC5);
    m_label_id.push_back(IDC_STATIC6);
    m_label_id.push_back(IDC_STATIC4);
    m_label_id.push_back(IDC_STATIC7);
    m_label_id.push_back(IDC_STATIC14);
    m_label_id.push_back(IDC_STATIC13);
    m_label_id.push_back(IDC_STATIC1);
    m_label_id.push_back(IDC_STATIC2);
    m_label_id.push_back(IDC_STATIC22);
    m_label_id.push_back(IDC_STATIC12);
    m_label_id.push_back(IDC_STATIC23);
    m_label_id.push_back(IDC_STATIC9);
    m_label_id.push_back(IDC_STATIC21);
    m_label_id.push_back(IDC_STATIC32);
    m_label_id.push_back(IDC_STATIC10);
    m_label_id.push_back(IDC_STATIC11);
    m_label_id.push_back(IDC_LIST);
    m_label_id.push_back(IDC_STATIC40);
    m_label_id.push_back(IDC_HTTP_CURRENT);
    m_label_id.push_back(IDC_STATIC39);
    m_label_id.push_back(IDC_P2P_CURRENT);
    m_label_id.push_back(IDC_STATIC38);
    m_label_id.push_back(IDC_STATIC41);
    m_label_id.push_back(IDC_TOTAL_SN);
    m_label_id.push_back(IDC_STATIC43);
    m_label_id.push_back(IDC_SN_STATE);
    LabelInfo li;
    CWnd *pWnd;
    CRect rect;

    for (int i = 0; i < m_label_id.size(); ++i)
    {
        li.labelID = m_label_id[i];

        pWnd = GetDlgItem(m_label_id[i]);
        pWnd->GetWindowRect(&rect);
        ScreenToClient(&rect);
        li.left_old = rect.left;
        li.right_old = rect.right;
        li.top_old = rect.top;
        li.bottom_old = rect.bottom;

        m_label_info.push_back(li);
    }
}

void CP2PDlg::Display(CString rid)
{
    for (int i = 0; i < m_statistics_datas[m_second]->GetVodDownloadDriverStatistics().size(); ++i)
    {
        char buf[MAX_PATH] = "";
        std::string tmp_rid = m_statistics_datas[m_second]->GetVodDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo().ResourceID.to_string();
        sprintf(buf, "%s", tmp_rid.c_str());
        
        if (rid.CompareNoCase(buf) != 0)
        {
            continue;
        }
        statistic::DOWNLOADDRIVER_STATISTIC_INFO dd_info = m_statistics_datas[m_second]->GetVodDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo();
        wchar_t *filename = (wchar_t*)(dd_info.FileName);
        char ansiname[MAX_PATH] = "";
        WideCharToMultiByte(CP_ACP, 0, filename, -1, ansiname, MAX_PATH, NULL, NULL);

        if (strlen(ansiname) == 0)
        {
            m_filename.Format("%s", "视频名称未知");
        }
        else
        {
            m_filename.Format("%s", ansiname);
        }

        m_sn_state=dd_info.sn_state;
        int http_downloader_index = 0;
        while (http_downloader_index < dd_info.HttpDownloaderCount && dd_info.HttpDownloaders[http_downloader_index].IsPause == 1)
        {
            ++http_downloader_index;
        }
        if (http_downloader_index >= dd_info.HttpDownloaderCount)
        {
            http_downloader_index = 0;
        }

        m_http_server_status = dd_info.HttpDownloaders[http_downloader_index].LastHttpStatusCode;

        char *ip = (char*)(dd_info.HttpDownloaders[http_downloader_index].Url);

        m_strIP.Format("%s", ip);
        int pos = m_strIP.Find('//', 0);
        m_strIP = m_strIP.Mid(pos+1);

        pos = m_strIP.Find('//', 0);
        m_strIP = m_strIP.Mid(pos+1);

        pos = m_strIP.Find('//', 0);
        m_strIP = m_strIP.Mid(0, pos);

        m_rest_time = dd_info.t;
        m_bandwidth = dd_info.b;
        m_speed_limit = dd_info.speed_limit;

        if (dd_info.http_state == 2 || (dd_info.http_state == 3 && dd_info.p2p_state == 0))
        {
            if (dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.TotalDownloadBytes / 1024 != 0)
            {
                m_total_http = dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.TotalDownloadBytes / 1024;
            }
        }

        string loc_result;

        loc_result = m_locater.getIpAddr(string(m_strIP.GetBuffer()));
        m_strLocation.Format("%s", loc_result.c_str());

        m_http_kbps = dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.NowDownloadSpeed / 1024;
        m_p2p_peer_kbps = 0;
        m_datarate = dd_info.DataRate;
        m_switch_state.Format("%d%d%d%d", dd_info.http_state, dd_info.p2p_state,
            dd_info.timer_using_state, dd_info.timer_state);

        if (dd_info.http_state == 2)
        {
            // HTTP ON
            m_switch_http.SetIcon(h_on);
        }
        else
        {
            // HTTP OFF
            m_switch_http.SetIcon(h_off);
        }

        if (dd_info.p2p_state == 2)
        {
            // P2P ON
            m_switch_p2p.SetIcon(h_on);
        }
        else
        {
            // P2P OFF
            m_switch_p2p.SetIcon(h_off);
        }

        UpdateData(FALSE);
    }

    BOOL success = FALSE;
    statistic::P2PDOWNLOADER_STATISTIC_INFO p2p_info;
    for (int i = 0; i < m_statistics_datas[m_second]->GetP2PDownloaderStatistics().size(); ++i)
    {
        char buf[MAX_PATH] = "";
        std::string tmprid = m_statistics_datas[m_second]->GetP2PDownloaderStatistics()[i]->GetDownloaderStatisticsInfo().ResourceID.to_string();
        sprintf(buf, "%s", tmprid.c_str());

        if (rid.CompareNoCase(buf) == 0)
        {
            p2p_info = m_statistics_datas[m_second]->GetP2PDownloaderStatistics()[i]->GetDownloaderStatisticsInfo();
            success = TRUE;
            break;
        }
    }
    if (TRUE == success)
    {
        m_filelen = p2p_info.FileLength;

        m_Send = p2p_info.TotalRequestSubPieceCount;
        m_Receive = p2p_info.TotalRecievedSubPieceCount;
        int total_receive = p2p_info.TotalUnusedSubPieceCount;

        m_NonConsistentSize = p2p_info.NonConsistentSize;

        int total_requestint_count = 0;
        for (int i=0; i<m_last_pc_count; i++)
        {
            total_requestint_count += p2p_info.P2PConnections[i].Requesting_Count;
        }

        m_lost_rate.Format("%.2f%%", (m_Send - total_requestint_count - total_receive)* 100.0 / (m_Send - total_requestint_count + 0.000001));
        m_re_rate.Format("%.2f%%", (total_receive - m_Receive) * 100.0 / (total_receive+0.0000001));

        m_p2p_kbps = p2p_info.SpeedInfo.NowDownloadSpeed / 1024;
        m_p2p_peer_kbps = 0;
        m_p2p_sn_kbps = 0;

        full_block_peer_count = p2p_info.FullBlockPeerCount;
        peer_count = p2p_info.PeerCount;
        ip_pool_peer_count = p2p_info.IpPoolPeerCount;

        m_peer_connect_count.Format("%d - %d - %d", p2p_info.FullBlockPeerCount,
            p2p_info.PeerCount, p2p_info.IpPoolPeerCount);

        m_total_p2p = p2p_info.TotalP2PPeerDataBytesWithoutRedundance / 1024;

        m_strConnectKick.Format("%d - %d", p2p_info.ConnectCount, p2p_info.KickCount);

        m_GlobalWindowSize = m_si.GlobalWindowSize;

        m_GlobalRequestSendCount = m_si.GlobalRequestSendCount;

        m_MemoryPoolLeftSize = m_si.MemoryPoolLeftSize;

        m_empty_subpiece_distance = p2p_info.empty_subpiece_distance;

        m_last_pc_count = p2p_info.PeerCount;
        for (int i=0; i<m_last_pc_count; i++)
        {
            m_last_pc[i] = p2p_info.P2PConnections[i];
        }

        ShowReport();
    }
    else
    {
        m_total_p2p = 0;            // P2P总下载
        m_total_sn = 0;
        m_p2p_kbps = 0;                // P2P速度
        m_p2p_peer_kbps = 0;
        m_p2p_sn_kbps = 0;
        m_peer_connect_count = "";    // 连接
        m_peerlist.DeleteAllItems();

        m_Send = 0;                    // 总发送
        m_Receive = 0;                // 总收到
        m_NonConsistentSize = 0;    // 距离
        m_re_rate = "";                // 冗余率
        m_lost_rate = "";            // 丢包率
    }
    
}

void CP2PDlg::ShowStatisticLog()
{
    if (m_is_online == TRUE)
        return;

    CString strLastGuid;

    // 保存上次选择
    int index = m_ridlist.GetCurSel();
    if (index >= 0)
    {
        m_ridlist.GetText(index, strLastGuid);
    }

    int count = m_ridlist.GetCount();
    m_ridlist.ResetContent();
    ClearContent();

    // 读共享内存并添加
    if (1 == import_file_type)
    {
        for (int i = 0; i < m_statistics_datas[m_second]->GetVodDownloadDriverStatistics().size(); ++i)
        {
            //DOWNLOADDRIVER_STATISTIC_INFO dd_info(m_statistics_datas[m_second]->GetVodDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo());

            char buf[MAX_PATH] = "";
            std::string rid = m_statistics_datas[m_second]->GetVodDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo().ResourceID.to_string();
            sprintf(buf, "%d_%s", i, rid.c_str());
            m_ridlist.InsertString(m_ridlist.GetCount(), buf);


            // 如果和上次选择相同，默认选中
            if (strLastGuid.CompareNoCase(buf) == 0)
            {
                for(int i=0; i<m_ridlist.GetCount(); i++)
                {
                    CString strtmp;
                    m_ridlist.GetText(i, strtmp);

                    if (strLastGuid.CompareNoCase(strtmp) == 0)
                    {
                        m_ridlist.SetCurSel(i);
                        break;
                    }
                }
            }
        }
    }

    if(count < m_ridlist.GetCount())
    {
        count = m_ridlist.GetCount();

        CString temp, max;
        int cur = 0;
        m_ridlist.GetText(0, temp);
        max = temp;

        for(int i = 1; i < count; ++i)
        {
            m_ridlist.GetText(i, temp);
            if(max < temp)
            {
                max = temp;
                cur = i;
            }
        }
        m_ridlist.SetCurSel(cur);
    }

    // 如果列表中只有一项，默认选中
    if (m_ridlist.GetCount() == 1)
    {
        m_ridlist.SetCurSel(0);
    }
    else if (m_ridlist.GetCount() == 0)
    {
        MonitorFillZero();
    }

    // 如果存在选中，则 持续 更新数据
    index = m_ridlist.GetCurSel();
    if (index >= 0)
    {
        // 更新数据
        OnLbnSelchangeRidList();
    }

    if (!m_is_pause && !m_is_online)
    {
        ++m_second;
        if (m_second >= m_statistics_datas.size())
        {
            m_second = m_statistics_datas.size() - 1;
            MonitorFillZero();
        }
    }
}


void CP2PDlg::CreateCsvFromGZ(CString filename)
{
    FILE *fpcsv;
    if ((fpcsv = fopen(filename, "w")) != NULL)
    {
        fprintf(fpcsv, "%s,", "文件名");
        fprintf(fpcsv, "%s,", "文件长度");
        fprintf(fpcsv, "%s,", "码流");
        fprintf(fpcsv, "%s,", "CDN IP");
        fprintf(fpcsv, "%s,", "当前时间");
        fprintf(fpcsv, "%s,", "总发送");
        fprintf(fpcsv, "%s,", "总收到");
        fprintf(fpcsv, "%s,", "丢包率");
        fprintf(fpcsv, "%s,", "冗余率");
        fprintf(fpcsv, "%s,", "http总下载");
        fprintf(fpcsv, "%s,", "p2p总下载");
        fprintf(fpcsv, "%s,", "剩余时间(s)");
        fprintf(fpcsv, "%s,", "http速度");
        fprintf(fpcsv, "%s,", "p2p速度");
        fprintf(fpcsv, "%s,", "总下载速度");
        fprintf(fpcsv, "%s,", "状态机");
        fprintf(fpcsv, "%s,", "连接数");
        fprintf(fpcsv, "%s,", "满资源的节点数");
        fprintf(fpcsv, "%s,", "连接节点数");
        fprintf(fpcsv, "%s\n", "IPPool");

        char this_rid[MAX_PATH];
        int last_pc_count = 0;
        for (int i = 0; i < m_statistics_datas.size(); ++i)
        {
            if (m_statistics_datas[i]->GetVodDownloadDriverStatistics().size() == 0)
            {
                continue;
            }

            statistic::DOWNLOADDRIVER_STATISTIC_INFO dd_info;
            CString tmp_ip;
            int http_downloader_index;
            if (i == 0)
            {
                int j;
                for (j = 0; j < m_statistics_datas[i]->GetVodDownloadDriverStatistics().size(); ++j)
                {
                    dd_info = m_statistics_datas[i]->GetVodDownloadDriverStatistics()[j]->GetDownloadDriverStatisticsInfo();
                    std::string rid = dd_info.ResourceID.to_string();
                    sprintf(this_rid, "%s", rid.c_str());
                    http_downloader_index = 0;
                    while (http_downloader_index < dd_info.HttpDownloaderCount && dd_info.HttpDownloaders[http_downloader_index].IsPause == 1)
                    {
                        ++http_downloader_index;
                    }
                    if (http_downloader_index >= dd_info.HttpDownloaderCount)
                    {
                        http_downloader_index = 0;
                    }
                    char *ip = (char*)(dd_info.HttpDownloaders[http_downloader_index].Url);

                    tmp_ip.Format("%s", ip);
                    int pos = tmp_ip.Find('//', 0);
                    tmp_ip = tmp_ip.Mid(pos+1);

                    pos = tmp_ip.Find('//', 0);
                    tmp_ip = tmp_ip.Mid(pos+1);

                    pos = tmp_ip.Find('//', 0);
                    tmp_ip = tmp_ip.Mid(0, pos);
                    if (tmp_ip != "ppsucai.pptv.com" && tmp_ip.Find("synacast") == -1)
                    {
                        break;
                    }
                }
                if (j == m_statistics_datas[i]->GetVodDownloadDriverStatistics().size())
                {
                    continue;
                }
            }
            else
            {
                int j;
                for (j = 0; j < m_statistics_datas[i]->GetVodDownloadDriverStatistics().size(); ++j)
                {
                    char tmp_rid[MAX_PATH];
                    dd_info = m_statistics_datas[i]->GetVodDownloadDriverStatistics()[j]->GetDownloadDriverStatisticsInfo();
                    std::string rid = dd_info.ResourceID.to_string();
                    sprintf(tmp_rid, "%s", rid.c_str());
                    if (strlen(tmp_rid) != 0 && strlen(this_rid) == 0)
                    {
                        strcat(this_rid, tmp_rid);
                    }
                    else
                    {
                        if (strcmp(this_rid, tmp_rid) == 0)
                        {
                            break;
                        }
                    }
                }
                if (j == m_statistics_datas[i]->GetVodDownloadDriverStatistics().size())
                {
                    continue;
                }
            }

            time_t now = time(0);
            char str_now[MAX_PATH]; 

            wchar_t *filename = (wchar_t*)(dd_info.FileName);
            char ansiname[MAX_PATH] = "";
            WideCharToMultiByte(CP_ACP, 0, filename, -1, ansiname, MAX_PATH, NULL, NULL);
            CString tmp_filename;
            if (strlen(ansiname) == 0)
            {
                tmp_filename.Format("%s", "视频名称未知");
            }
            else
            {
                tmp_filename.Format("%s", ansiname);
            }

            fprintf(fpcsv, "%s", tmp_filename); //文件名
            fprintf(fpcsv, ",%d", dd_info.FileLength);  //文件长度
            fprintf(fpcsv, ",%d", dd_info.DataRate);    //码流

            http_downloader_index = 0;
            while (http_downloader_index < dd_info.HttpDownloaderCount && dd_info.HttpDownloaders[http_downloader_index].IsPause == 1)
            {
                ++http_downloader_index;
            }
            if (http_downloader_index >= dd_info.HttpDownloaderCount)
            {
                http_downloader_index = 0;
            }

            if (i != 0)
            {
                char *ip = (char*)(dd_info.HttpDownloaders[http_downloader_index].Url);

                tmp_ip.Format("%s", ip);
                int pos = tmp_ip.Find('//', 0);
                tmp_ip = tmp_ip.Mid(pos+1);

                pos = tmp_ip.Find('//', 0);
                tmp_ip = tmp_ip.Mid(pos+1);

                pos = tmp_ip.Find('//', 0);
                tmp_ip = tmp_ip.Mid(0, pos);
            }

            if (dd_info.http_state == 3 && dd_info.p2p_state != 0)
            {
                fprintf(fpcsv, ",");    //CDN IP
            }
            else
            {
                fprintf(fpcsv, ",%s", tmp_ip);    //CDN IP
            }

            strftime(str_now, sizeof(str_now), "%Y/%m/%d %X",localtime(&now)); 
            fprintf(fpcsv, ",%s", str_now);    //当前时间

            statistic::P2PDOWNLOADER_STATISTIC_INFO p2p_info;
            for (int j = 0; j < m_statistics_datas[i]->GetP2PDownloaderStatistics().size(); ++j)
            {
                char tmp_rid[MAX_PATH];
                std::string rid = m_statistics_datas[i]->GetP2PDownloaderStatistics()[j]->GetDownloaderStatisticsInfo().ResourceID.to_string();
                sprintf(tmp_rid, "%s", rid.c_str());
                if (strcmp(this_rid, tmp_rid) == 0)
                {
                    p2p_info = m_statistics_datas[i]->GetP2PDownloaderStatistics()[j]->GetDownloaderStatisticsInfo();
                    break;
                }
            }
            fprintf(fpcsv, ",%d", p2p_info.TotalRequestSubPieceCount);        //总发送
            fprintf(fpcsv, ",%d", p2p_info.TotalRecievedSubPieceCount);    //总收到

            int total_requestint_count = 0;
            for (int i=0; i<last_pc_count; i++)
            {
                total_requestint_count += p2p_info.P2PConnections[i].Requesting_Count;
            }

            CString tmp_lost_rate;
            tmp_lost_rate.Format("%.2f%%", (p2p_info.TotalRequestSubPieceCount - total_requestint_count - p2p_info.TotalUnusedSubPieceCount)* 100.0
                / (p2p_info.TotalRequestSubPieceCount - total_requestint_count + 0.000001));
            fprintf(fpcsv, ",%s", tmp_lost_rate);//丢包率
            CString tmp_re_rate;
            tmp_re_rate.Format("%.2f%%", (p2p_info.TotalUnusedSubPieceCount - p2p_info.TotalRecievedSubPieceCount) * 100.0 /
                (p2p_info.TotalUnusedSubPieceCount + 0.0000001));
            fprintf(fpcsv, ",%s", tmp_re_rate);    //冗余率

            fprintf(fpcsv, ",%d", dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.TotalDownloadBytes / 1024);//http总下载
            fprintf(fpcsv, ",%d", p2p_info.TotalP2PPeerDataBytesWithoutRedundance / 1024); //p2p总下载

            fprintf(fpcsv, ",%d", dd_info.t / 1000);//剩余时间
            fprintf(fpcsv, ",%d", dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.NowDownloadSpeed / 1024); //http速度
            fprintf(fpcsv, ",%d", p2p_info.SpeedInfo.NowDownloadSpeed / 1024);  //p2p速度
            fprintf(fpcsv, ",%d", dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.NowDownloadSpeed / 1024
                + p2p_info.SpeedInfo.NowDownloadSpeed / 1024);  //总下载速度

            CString tmp_switch_state;
            tmp_switch_state.Format("%d%d%d%d", dd_info.http_state, dd_info.p2p_state,
                dd_info.timer_using_state, dd_info.timer_state);
            fprintf(fpcsv, ",%s", tmp_switch_state);//状态机

            CString tmp_peer_connect_count;
            tmp_peer_connect_count.Format("%d - %d - %d", p2p_info.FullBlockPeerCount,
                p2p_info.PeerCount, p2p_info.IpPoolPeerCount);
            fprintf(fpcsv, ",%s", tmp_peer_connect_count);    //连接数
            fprintf(fpcsv, ",%d", p2p_info.FullBlockPeerCount);   //满资源数
            fprintf(fpcsv, ",%d", p2p_info.PeerCount);              //连接上的peer数
            fprintf(fpcsv, ",%d\n", p2p_info.IpPoolPeerCount);      //备选peer数
        }

        fclose(fpcsv);
    }
}