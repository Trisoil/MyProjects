// LiveP2PDlg.cpp : implementation file
//

#include "stdafx.h"
#include "P2PMonitor.h"
#include "LiveP2PDlg.h"
#include "P2PMonitorDlg.h"


// LiveP2PDlg dialog

using namespace p2p_monitor;

IMPLEMENT_DYNAMIC(LiveP2PDlg, CDialog)

LiveP2PDlg::LiveP2PDlg(CWnd* pParent /*=NULL*/)
    : CDialog(LiveP2PDlg::IDD, pParent)
    , m_live_http_speed(0)
    , m_live_p2p_speed(_T(""))
    , m_switch_state(_T(""))
    , m_receive(_T(""))
    , m_re_rate(_T(""))
    , m_lost_rate(_T(""))
    , m_ip(_T(""))
    , m_save_rate(_T(""))
    , m_strLocation(_T(""))
    , m_http_server_status(0)
    , m_total_http(0)
    , m_total_p2p(0)
    , m_live_p2p_subpiece_speed(0)
    , m_unic_request(_T(""))
    , m_unic_receive(_T(""))
    , m_peer_connect_count(_T(""))
    , m_is_saving(FALSE)
    , connect_peer(0)
    , ippool(0)
    , m_datarate(0)
    , m_cache_size(0)
    , m_cache_interval(_T(""))
    , m_playing_position(0)
    , m_left_capacity(0)
    , m_rest_time(0)
    , m_cache_start(0)
    , m_cache_end(0)
    , m_data_rate_level_(0)
    , m_live_point_(0)
    , m_jump_times_(0)
    , m_checksum_failed_times_(0)
    , m_is_2300_resttime_enouth_(0)
    , m_import_file_type(0)
    , m_second(0)
    , m_is_created(FALSE)
    , m_is_pause(TRUE)
    , m_udpserver_download_bytes(0)
    , m_pms_status(0)
    , m_unique_id(0)
    , m_mem_unique_id(0)
    , m_udp_server_speed(_T(""))
    , m_live_pause(0)
    , m_live_replay(0)
    , m_sort_col(-1)
    , m_sort(0)
    , m_list_update(TRUE)
    , peer_count(0)
    , rate_index(0)
    , upload_speed_(0)
    , upload_connection_count_(0)
    , upload_speed_limit_(0)
    , slide_lost_rate_(_T(""))
    , slide_re_rate_(_T(""))
    , missing_existing_subpiece_count_(_T(""))
    , send_subpiece_count_(_T(""))
    , second_index_(0)
    , m_line_number(0)
    , last_line_number_(0)
{
    m_label_info.resize(0);

    memset(m_unique_requests, 0, sizeof(m_unique_requests));
    memset(m_receives, 0, sizeof(m_receives));
    memset(m_unique_receives, 0, sizeof(m_unique_receives));
    memset(m_send_subpiece_count, 0, sizeof(m_send_subpiece_count));
}

LiveP2PDlg::~LiveP2PDlg()
{
}

void LiveP2PDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_HTTP_SPEED, m_live_http_speed);
    DDX_Text(pDX, IDC_P2P_SPEED, m_live_p2p_speed);
    DDX_Text(pDX, IDC_EDIT_SWITCH, m_switch_state);
    DDX_Text(pDX, IDC_TOTAL_RECV, m_receive);
    DDX_Text(pDX, IDC_RETRY_RATE, m_re_rate);
    DDX_Text(pDX, IDC_LOST_RATE, m_lost_rate);
    DDX_Text(pDX, IDC_EDIT_CDNIP, m_ip);
    DDX_Text(pDX, IDC_EDIT_BAND_SAVE, m_save_rate);
    DDX_Text(pDX, IDC_EDIT_CDN_LOCATION, m_strLocation);
    DDX_Text(pDX, IDC_EDIT_CDN_STATUS, m_http_server_status);
    DDX_Text(pDX, IDC_LIVE_TOTAL_HTTP, m_total_http);
    DDX_Text(pDX, IDC_LIVE_TOTAL_P2P, m_total_p2p);
    //DDX_Text(pDX, IDC_EDIT12, m_live_p2p_subpiece_speed);
    DDX_Control(pDX, IDC_LIST1, m_peerlist);
    DDX_Text(pDX, IDC_REQUEST, m_unic_request);
    DDX_Text(pDX, IDC_RECEIVE, m_unic_receive);
    DDX_Text(pDX, IDC_EDIT_LIVE_CONNECT, m_peer_connect_count);
    DDX_Text(pDX, IDC_EDIT_DATA_RATE, m_datarate);
    DDX_Text(pDX, IDC_CACHE_SIZE, m_cache_size);
    DDX_Text(pDX, IDC_CACHE_INTERVAL, m_cache_interval);
    DDX_Text(pDX, IDC_PLAYING_POSITION, m_playing_position);
    DDX_Text(pDX, IDC_LEFT_CAPACITY, m_left_capacity);
    DDX_Text(pDX, IDC_EDIT8, m_rest_time);
    DDX_Control(pDX, IDC_RID_LIST, m_ridlist);
    DDX_Text(pDX, IDC_EDIT9, m_data_rate_level_);
    DDX_Text(pDX, IDC_EDIT10, m_live_point_);
    DDX_Text(pDX, IDC_EDIT_JUMP_TIMES, m_jump_times_);
    DDX_Text(pDX, IDC_EDIT_CHECKSUM_FAILED_TIMES, m_checksum_failed_times_);
    //DDX_Text(pDX, IDC_EDIT_CHANNLE_ID, m_channel_id);
    DDX_Text(pDX, IDC_EDIT_UDPSERVER_DOWNLOAD, m_udpserver_download_bytes);
    DDX_Text(pDX, IDC_EDIT_PMS_STATUS, m_pms_status);
    DDX_Text(pDX, IDC_EDIT_UNIQUE_ID, m_mem_unique_id);
    DDX_Text(pDX, IDC_EDIT_UDPSERVER_SPEED, m_udp_server_speed);
    DDX_Text(pDX, IDC_EDIT_PAUSE, m_live_pause);
    DDX_Text(pDX, IDC_EDIT_REPLAY, m_live_replay);
    DDX_Text(pDX, IDC_EDIT_UPLOAD_SPEED2, upload_speed_);
    DDX_Text(pDX, IDC_EDIT_UPLOAD_CONNECTION_COUNT2, upload_connection_count_);
    DDX_Text(pDX, IDC_EDIT_UPLOAD_LIMIT_SPEED2, upload_speed_limit_);
    DDX_Text(pDX, IDC_EDIT_SLIDE_LOST_RATE, slide_lost_rate_);
    DDX_Text(pDX, IDC_EDIT_SLIDE_RE_RATE, slide_re_rate_);
    DDX_Text(pDX, IDC_EDIT_MISSING_EXIST_SUBPIECE_COUNT, missing_existing_subpiece_count_);
    DDX_Text(pDX, IDC_EDIT_SEND, send_subpiece_count_);
}

BEGIN_MESSAGE_MAP(LiveP2PDlg, CDialog)
    ON_LBN_SELCHANGE(IDC_RID_LIST, &LiveP2PDlg::OnLbnSelchangeRidList)
    ON_WM_SIZE()
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &LiveP2PDlg::OnLvnColumnclickList1)
END_MESSAGE_MAP()


// LiveP2PDlg message handlers

BOOL LiveP2PDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    m_peerlist.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);

    GetClientRect(&m_rect);
    m_live_p2p_ori_h = m_rect.Height();
    m_live_p2p_ori_w = m_rect.Width();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void LiveP2PDlg::OnShareMemoryTimer()
{
    if (FALSE == m_is_online && 0 == m_import_file_type)
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
        
        if (TRUE == m_is_online)
        {
            m_ridlist.ResetContent();
            ClearContent();
            if (((CP2PMonitorDlg*)GetParent()->GetParent())->ReadShareMemory("PPVIDEO_" + m_strProcessID, sizeof(m_si), &m_si))
            {
                LIVE_DOWNLOADDRIVER_STATISTIC_INFO_EX m_live_download_info_ex = LIVE_DOWNLOADDRIVER_STATISTIC_INFO_EX(100);
                LIVE_DOWNLOADDRIVER_STATISTIC_INFO& m_live_download_info = m_live_download_info_ex.Ref();

                for (int i=0; i<LIVEDOWNLOADER_MAX_COUNT; i++)
                {
                    if (m_si.LiveDownloadDriverIDs[i] != 0)
                    {
                        CString str;
                        str.Format("%d", m_si.LiveDownloadDriverIDs[i]);
                        if (((CP2PMonitorDlg*)GetParent()->GetParent())->ReadShareMemory("LIVEDOWNLOADDRIVER_" + m_strProcessID + "_" + str, m_live_download_info_ex.Size(), &m_live_download_info))
                        {
                            char buf[MAX_PATH] = "";
                            GUID guid = m_live_download_info.ResourceID;
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

                            UpdateData(FALSE);
                        }
                    }
                }
            }

            int size = sizeof(m_upload_info);
            if (((CP2PMonitorDlg*)GetParent()->GetParent())->ReadShareMemory("UPLOAD_" + m_strProcessID, sizeof(m_upload_info), &m_upload_info))
            {
                upload_connection_count_ = m_upload_info.peer_upload_count;
                upload_speed_ = m_upload_info.upload_speed;
                upload_speed_limit_ = m_upload_info.actual_speed_limit;
                m_send_subpiece_count[second_index_] = m_upload_info.upload_subpiece_count;
                if (++second_index_ >= 20)
                {
                    second_index_ = 0;
                }
                UpdateData(FALSE);
            }
        }
        else if (1 == m_import_file_type && !m_is_pause && m_second < m_statistics_datas.size())
        {
            m_ridlist.ResetContent();
            ClearContent();
            for (int i = 0; i < m_statistics_datas[m_second]->GetLiveDownloadDriverStatistics().size(); ++i)
            {
                //DOWNLOADDRIVER_STATISTIC_INFO dd_info(m_statistics_datas[m_second]->GetVodDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo());

                char buf[MAX_PATH] = "";
                std::string rid = m_statistics_datas[m_second]->GetLiveDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo().ResourceID.to_string();
                sprintf(buf, "%d_%s", i, rid.c_str());
                m_ridlist.InsertString(m_ridlist.GetCount(), buf);


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
        else
        {
            return;
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

void LiveP2PDlg::SetProcessID(CString processid)
{
    m_strProcessID = processid;
}

void LiveP2PDlg::ShowReport()
{
    for(int i=0; i<peer_count-1; i++)
    {
        for(int j=i+1; j<peer_count; j++)
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
                // LastLivePieceID
                int a = m_last_pc[i].LastLivePieceID;
                int b = m_last_pc[j].LastLivePieceID;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 4)
            {
                // AssignedSubPieceCount
                int a = m_last_pc[i].AssignedSubPieceCount;
                int b = m_last_pc[j].AssignedSubPieceCount;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 5)
            {
                // WindowSize
                int a = m_last_pc[i].WindowSize;
                int b = m_last_pc[j].WindowSize;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 6)
            {
                // Total Request Count
                int a = m_last_pc[i].Sent_Count;
                int b = m_last_pc[j].Sent_Count;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 7)
            {
                // Requesting Count
                int a = m_last_pc[i].Requesting_Count;
                int b = m_last_pc[j].Requesting_Count;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 8)
            {
                // Total Received Count
                int a = m_last_pc[i].Received_Count;
                int b = m_last_pc[j].Received_Count;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 9)
            {
                // RTT_Average
                int a = m_last_pc[i].RTT_Average;
                int b = m_last_pc[j].RTT_Average;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 10)
            {
                // RTT_Max
                int a = m_last_pc[i].RTT_Max;
                int b = m_last_pc[j].RTT_Max;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 11)
            {
                // AverageDeltaTime
                int a = m_last_pc[i].AverageDeltaTime;
                int b = m_last_pc[j].AverageDeltaTime;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 12)
            {
                // NowDownloadSpeed
                int a = m_last_pc[i].SpeedInfo.NowDownloadSpeed;
                int b = m_last_pc[j].SpeedInfo.NowDownloadSpeed;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }


            if (m_sort_col == 13)
            {
                // lost rate
                float a = (m_last_pc[i].Sent_Count - m_last_pc[i].Requesting_Count - m_last_pc[i].Received_Count) * 1.0 / (m_last_pc[i].Sent_Count - m_last_pc[i].Requesting_Count + 0.000001);
                float b = (m_last_pc[j].Sent_Count - m_last_pc[j].Requesting_Count - m_last_pc[j].Received_Count) * 1.0 / (m_last_pc[j].Sent_Count - m_last_pc[j].Requesting_Count + 0.000001);

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 14)
            {
                // ConnectType
                int a = m_last_pc[i].ConnectType;
                int b = m_last_pc[j].ConnectType;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 15)
            {
                // Download Connected Count
                int a = m_last_pc[i].RealTimePeerInfo.download_connected_count_;
                int b = m_last_pc[j].RealTimePeerInfo.download_connected_count_;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 16)
            {
                // Upload Connected Count
                int a = m_last_pc[i].RealTimePeerInfo.upload_connected_count_;
                int b = m_last_pc[j].RealTimePeerInfo.upload_connected_count_;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 17)
            {
                // Upload Speed
                int a = m_last_pc[i].RealTimePeerInfo.mine_upload_speed_;
                int b = m_last_pc[j].RealTimePeerInfo.mine_upload_speed_;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 18)
            {
                // Max Upload Speed
                int a = m_last_pc[i].RealTimePeerInfo.max_upload_speed_;
                int b = m_last_pc[j].RealTimePeerInfo.max_upload_speed_;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 19)
            {
                // Rest Playable Time
                int a = m_last_pc[i].RealTimePeerInfo.rest_playable_time_;
                int b = m_last_pc[j].RealTimePeerInfo.rest_playable_time_;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 20)
            {
                // Peer Nat Type
                int a = m_last_pc[i].PeerInfo.PeerNatType;
                int b = m_last_pc[j].PeerInfo.PeerNatType;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 21)
            {
                // Rest Playable Time
                int a = m_last_pc[i].RealTimePeerInfo.lost_rate_;
                int b = m_last_pc[j].RealTimePeerInfo.lost_rate_;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 22)
            {
                // Rest Playable Time
                int a = m_last_pc[i].RealTimePeerInfo.redundancy_rate_;
                int b = m_last_pc[j].RealTimePeerInfo.redundancy_rate_;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 23)
            {
                // Actual Assigned SubPiece Count
                int a = m_last_pc[i].ActualAssignedSubPieceCount;
                int b = m_last_pc[j].ActualAssignedSubPieceCount;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 24)
            {
                // Request SubPiece Count
                int a = m_last_pc[i].RequestSubPieceCount;
                int b = m_last_pc[j].RequestSubPieceCount;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 25)
            {
                // Supply SubPiece Count
                int a = m_last_pc[i].SupplySubPieceCount;
                int b = m_last_pc[j].SupplySubPieceCount;

                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 26)
            {
                // Time of No Response
                int a = m_last_pc[i].TimeOfNoResponse;
                int b = m_last_pc[j].TimeOfNoResponse;

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
        m_sel_ip.Format("%s%s", m_peerlist.GetItemText(sel, 0), m_peerlist.GetItemText(sel, 1));
    }

    m_peerlist.DeleteAllItems();

    for(int i = 0; i< peer_count; i++)
    {
        P2P_CONNECTION_INFO pcinfo = m_last_pc[i];
        CString str;
        char strIP[MAX_PATH];
        char strDetectIP[MAX_PATH];

        // DetectIP
        Value2IP(pcinfo.PeerInfo.DetectIP, strDetectIP);
        m_peerlist.InsertItem(i, strDetectIP);

        // IP
        Value2IP(pcinfo.PeerInfo.IP, strIP);
        m_peerlist.SetItem(i, 1, LVIF_TEXT, strIP, 0, 0, 0, 0);
        str.Format("%s%s", strDetectIP, strIP);
        if (str.Compare(m_sel_ip) == 0)
        {
            m_peerlist.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
        }

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

        // LAST PIECE ID
        str.Format("%d - %d", pcinfo.FirstLiveBlockID, pcinfo.LastLivePieceID);
        m_peerlist.SetItem(i, 3, LVIF_TEXT, str, 0, 0, 0, 0);

        // ASSIGNED COUNT
        str.Format("%d", pcinfo.AssignedSubPieceCount);
        m_peerlist.SetItem(i, 4, LVIF_TEXT, str, 0, 0, 0, 0);

        // WindowSize
        str.Format("%d", pcinfo.WindowSize);
        m_peerlist.SetItem(i, 5, LVIF_TEXT, str, 0, 0, 0, 0);

        // Total Request Count
        str.Format("%d", pcinfo.Sent_Count);
        m_peerlist.SetItem(i, 6, LVIF_TEXT, str, 0, 0, 0, 0);

        // Requesting_Count
        str.Format("%d", pcinfo.Requesting_Count);
        m_peerlist.SetItem(i, 7, LVIF_TEXT, str, 0, 0, 0, 0);

        // Total Received Count
        str.Format("%d", pcinfo.Received_Count);
        m_peerlist.SetItem(i, 8, LVIF_TEXT, str, 0, 0, 0, 0);

        // RTT_AVERAGE
        str.Format("%d", pcinfo.RTT_Average);
        m_peerlist.SetItem(i, 9, LVIF_TEXT, str, 0, 0, 0, 0);

        // RTT_MAX
        str.Format("%d", pcinfo.RTT_Max);
        m_peerlist.SetItem(i, 10, LVIF_TEXT, str, 0, 0, 0, 0);

        // AVG_DELTA_TIME
        str.Format("%d", pcinfo.AverageDeltaTime);
        m_peerlist.SetItem(i, 11, LVIF_TEXT, str, 0, 0, 0, 0);

        // SPEED
        str.Format("%d", pcinfo.SpeedInfo.NowDownloadSpeed);
        m_peerlist.SetItem(i, 12, LVIF_TEXT, str, 0, 0, 0, 0);

        // LOST RATE
        double rate = (pcinfo.Sent_Count - pcinfo.Requesting_Count - pcinfo.Received_Count) * 100.0 / (pcinfo.Sent_Count - pcinfo.Requesting_Count + 0.000001);
        str.Format("%.2f%%", rate);
        m_peerlist.SetItem(i, 13, LVIF_TEXT, str, 0, 0, 0, 0);

        // CONNECT TYPE
        str.Format("%d", pcinfo.ConnectType);
        m_peerlist.SetItem(i, 14, LVIF_TEXT, str, 0, 0, 0, 0);

        // DOWNLOAD CONNECTED COUNT
        str.Format("%d", pcinfo.RealTimePeerInfo.download_connected_count_);
        m_peerlist.SetItem(i, 15, LVIF_TEXT, str, 0, 0, 0, 0);

        // UPLOAD CONNECTED COUNT
        str.Format("%d", pcinfo.RealTimePeerInfo.upload_connected_count_);
        m_peerlist.SetItem(i, 16, LVIF_TEXT, str, 0, 0, 0, 0);

        // UPLOAD SPEED
        str.Format("%d", pcinfo.RealTimePeerInfo.mine_upload_speed_);
        m_peerlist.SetItem(i, 17, LVIF_TEXT, str, 0, 0, 0, 0);

        // MAX UPLOAD SPEED
        str.Format("%d", pcinfo.RealTimePeerInfo.max_upload_speed_);
        m_peerlist.SetItem(i, 18, LVIF_TEXT, str, 0, 0, 0, 0);

        // REST PLAYABLE TIME
        str.Format("%d", pcinfo.RealTimePeerInfo.rest_playable_time_);
        m_peerlist.SetItem(i, 19, LVIF_TEXT, str, 0, 0, 0, 0);

        // NAT TYPE
        switch (pcinfo.PeerInfo.PeerNatType)
        {
        case 255:
            str.Format("%s", "ERROR");
            break;
        case 0:
            str.Format("%s", "FULLCONENAT");
            break;
        case 1:
            str.Format("%s", "IP_RESTRICTEDNAT");
            break;
        case 2:
            str.Format("%s", "IP_PORT_RESTRICTEDNAT");
            break;
        case 3:
            str.Format("%s", "SYMNAT");
            break;
        case 4:
            str.Format("%s", "PUBLIC");
            break;
        }

        m_peerlist.SetItem(i, 20, LVIF_TEXT, str, 0, 0, 0, 0);

        // LOST RATE
        str.Format("%d", pcinfo.RealTimePeerInfo.lost_rate_);
        m_peerlist.SetItem(i, 21, LVIF_TEXT, str, 0, 0, 0, 0);

        // RE RATE
        str.Format("%d", pcinfo.RealTimePeerInfo.redundancy_rate_);
        m_peerlist.SetItem(i, 22, LVIF_TEXT, str, 0, 0, 0, 0);

        // Actual Assigned SubPiece Count
        str.Format("%d", pcinfo.ActualAssignedSubPieceCount);
        m_peerlist.SetItem(i, 23, LVIF_TEXT, str, 0, 0, 0, 0);

        // Request SubPiece Count
        str.Format("%d", pcinfo.RequestSubPieceCount);
        m_peerlist.SetItem(i, 24, LVIF_TEXT, str, 0, 0, 0, 0);

        // Supply SubPiece Count
        str.Format("%d", pcinfo.SupplySubPieceCount);
        m_peerlist.SetItem(i, 25, LVIF_TEXT, str, 0, 0, 0, 0);

        // Time of No Response
        str.Format("%d", pcinfo.TimeOfNoResponse);
        m_peerlist.SetItem(i, 26, LVIF_TEXT, str, 0, 0, 0, 0);
    }

    UpdateData(FALSE);
}

void LiveP2PDlg::InitListControl()
{
    m_peerlist.InsertColumn(0, "DetectIP");
    m_peerlist.InsertColumn(1, "IP");
    m_peerlist.InsertColumn(2, "Location");
    m_peerlist.InsertColumn(3, "FIRST - LAST BLOCK ID");
    m_peerlist.InsertColumn(4, "ASSIGNED COUNT");
    m_peerlist.InsertColumn(5, "Window Size");
    m_peerlist.InsertColumn(6, "TOTAL REQUEST COUNT");
    m_peerlist.InsertColumn(7, "REQUESTING COUNT");
    m_peerlist.InsertColumn(8, "TOTAL RECEIVED COUNT");
    m_peerlist.InsertColumn(9, "RTT AVG");
    m_peerlist.InsertColumn(10, "RTT MAX");
    m_peerlist.InsertColumn(11, "AVG DELTA");
    m_peerlist.InsertColumn(12, "SPEED");
    m_peerlist.InsertColumn(13, "LOST RATE");
    m_peerlist.InsertColumn(14, "CONNECT TYPE");
    m_peerlist.InsertColumn(15, "DOWNLOAD CONNECTED COUNT");
    m_peerlist.InsertColumn(16, "UPLOAD CONNECTED COUNT");
    m_peerlist.InsertColumn(17, "UPLOAD SPEED");
    m_peerlist.InsertColumn(18, "MAX UPLOAD SPEED");
    m_peerlist.InsertColumn(19, "REST PLAYABLE TIME");
    m_peerlist.InsertColumn(20, "NAT TYPE");
    m_peerlist.InsertColumn(21, "LOST RATE");
    m_peerlist.InsertColumn(22, "REDUNDANCY RATE");
    m_peerlist.InsertColumn(23, "ACTUAL ASSIGNED SUBPIECE COUNT");
    m_peerlist.InsertColumn(24, "REQUEST SUBPIECE COUNT");
    m_peerlist.InsertColumn(25, "SUPPLY SUBPIECE COUNT");
    m_peerlist.InsertColumn(26, "TIME OF NO RESPONSE");

    m_peerlist.SetColumnWidth(0, 50);
    m_peerlist.SetColumnWidth(1, 100);
    m_peerlist.SetColumnWidth(2, 100);
    m_peerlist.SetColumnWidth(3, 150);
    m_peerlist.SetColumnWidth(4, 75);
    m_peerlist.SetColumnWidth(5, 80);
    m_peerlist.SetColumnWidth(6, 70);
    m_peerlist.SetColumnWidth(7, 80);
    m_peerlist.SetColumnWidth(8, 70);
    m_peerlist.SetColumnWidth(9, 70);
    m_peerlist.SetColumnWidth(10, 70);
    m_peerlist.SetColumnWidth(11, 70);
    m_peerlist.SetColumnWidth(12, 70);
    m_peerlist.SetColumnWidth(13, 70);
    m_peerlist.SetColumnWidth(14, 70);
    m_peerlist.SetColumnWidth(15, 70);
    m_peerlist.SetColumnWidth(16, 70);
    m_peerlist.SetColumnWidth(17, 70);
    m_peerlist.SetColumnWidth(18, 70);
    m_peerlist.SetColumnWidth(19, 70);
    m_peerlist.SetColumnWidth(20, 70);
    m_peerlist.SetColumnWidth(21, 70);
    m_peerlist.SetColumnWidth(22, 70);
    m_peerlist.SetColumnWidth(23, 70);
    m_peerlist.SetColumnWidth(24, 70);
    m_peerlist.SetColumnWidth(25, 70);
    m_peerlist.SetColumnWidth(26, 70);
}

void LiveP2PDlg::CreateCSVFileName()
{
    time_t t = time(0);
    char tmp[64];
    strftime(tmp, 64, "%X", localtime(&t));

    for (int i=0; i<strlen(tmp); i++)
    {
        if (tmp[i] == ':')
            tmp[i] = '_';
    }
    csvFileName = "\\PeerMonitor_live_p2p.";
    for (int i = 0; i < 8; ++i)
    {
        csvFileName += tmp[i];
    }
    csvFileName = csvFileName + ".csv";
}

void LiveP2PDlg::SetStart()
{
    m_is_start = true;
}

void LiveP2PDlg::SetSaving(BOOL is_saving)
{
    m_is_saving = is_saving;
}

void LiveP2PDlg::Export()
{
    CString CurrentPath;
    GetModuleFileName(NULL, CurrentPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
    CurrentPath.ReleaseBuffer();
    int pos = CurrentPath.ReverseFind('\\');
    CurrentPath = CurrentPath.Left(pos);
    
    LPITEMIDLIST pidl = NULL;
    SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &pidl);
    if (pidl)
    {
        if (m_is_start)
        {
            CreateCSVFileName();
        }

        //FilePathName += csvFileName;
        FilePathName.Format("%s%s",CurrentPath, csvFileName.c_str());
    }

    FILE *fpcsv;   

    if ((fpcsv = fopen(FilePathName.GetBuffer(), "a+")) != NULL)
    {
        if(m_is_start)
        {
            m_is_start = false;
#if !_USER_VERSION_
            fprintf(fpcsv, "%s,", "当前时间");
            fprintf(fpcsv, "%s,", "http总下载");
            fprintf(fpcsv, "%s,", "p2p总下载");
            fprintf(fpcsv, "%s,", "UdpServer总下载");
            fprintf(fpcsv, "%s,", "节约带宽比");
            fprintf(fpcsv, "%s,", "CDN IP");
            fprintf(fpcsv, "%s,", "CDN地域");
            fprintf(fpcsv, "%s,", "CDN状态");
            fprintf(fpcsv, "%s,", "总收到");
            fprintf(fpcsv, "%s,", "无重复请求");
            fprintf(fpcsv, "%s,", "无重复收到");
            fprintf(fpcsv, "%s,", "发送");
            fprintf(fpcsv, "%s,", "丢包率");
            fprintf(fpcsv, "%s,", "冗余率");
            fprintf(fpcsv, "%s,", "http速度");
            fprintf(fpcsv, "%s,", "p2p总速度");
            fprintf(fpcsv, "%s,", "p2p速度");
            fprintf(fpcsv, "%s,", "状态机");
            fprintf(fpcsv, "%s,", "连接数");
            fprintf(fpcsv, "%s,", "连接节点数");
            fprintf(fpcsv, "%s,", "IPPool");
            fprintf(fpcsv, "%s,", "缓存大小");
            fprintf(fpcsv, "%s,", "缓存区间");
            fprintf(fpcsv, "%s,", "播放点");
            fprintf(fpcsv, "%s,", "剩余时间");
            fprintf(fpcsv, "%s,", "码流率");
            fprintf(fpcsv, "%s,", "内存池剩余");
            fprintf(fpcsv, "%s,", "上传速度");
            fprintf(fpcsv, "%s,", "上传连接数");
            fprintf(fpcsv, "%s,", "上传限速");
            fprintf(fpcsv, "%s,", "PMS");
            fprintf(fpcsv, "%s,", "播放器ID");
            fprintf(fpcsv, "%s,", "暂停");
            fprintf(fpcsv, "%s,", "回拖");
            fprintf(fpcsv, "%s,", "码流率等级");
            fprintf(fpcsv, "%s,", "滑动丢包率");
            fprintf(fpcsv, "%s,", "滑动冗余率");
            fprintf(fpcsv, "%s,", "UdpServer速度");
            fprintf(fpcsv, "%s,", "跳跃次数");
            fprintf(fpcsv, "%s,", "校验失败");
            fprintf(fpcsv, "%s,", "缺-有");
            fprintf(fpcsv, "%s,", "直播点");
            fprintf(fpcsv, "%s,", "DetectIP");
            fprintf(fpcsv, "%s,", "IP");
            fprintf(fpcsv, "%s,", "Location");
            fprintf(fpcsv, "%s,", "FIRST BLOCK ID");
            fprintf(fpcsv, "%s,", "LAST BLOCK ID");
            fprintf(fpcsv, "%s,", "ASSIGNED COUNT");
            fprintf(fpcsv, "%s,", "Window Size");
            fprintf(fpcsv, "%s,", "TOTAL REQUEST COUNT");
            fprintf(fpcsv, "%s,", "REQUESTING COUNT");
            fprintf(fpcsv, "%s,", "TOTAL RECEIVED COUNT");
            fprintf(fpcsv, "%s,", "RTT AVG");
            fprintf(fpcsv, "%s,", "RTT MAX");
            fprintf(fpcsv, "%s,", "AVG DELTA");
            fprintf(fpcsv, "%s,", "SPEED");
            fprintf(fpcsv, "%s,", "LOST RATE");
            fprintf(fpcsv, "%s,", "CONNECT TYPE");
            fprintf(fpcsv, "%s,", "DOWNLOAD CONNECTED COUNT");
            fprintf(fpcsv, "%s,", "UPLOAD CONNECTED COUNT");
            fprintf(fpcsv, "%s,", "UPLOAD SPEED");
            fprintf(fpcsv, "%s,", "MAX UPLOAD SPEED");
            fprintf(fpcsv, "%s,", "REST PLAYABLE TIME");
            fprintf(fpcsv, "%s,", "NAT TYPE");
            fprintf(fpcsv, "%s,", "LOST RATE");
            fprintf(fpcsv, "%s,", "REDUNDANCY RATE");
            fprintf(fpcsv, "%s,", "ACTUAL ASSIGNED SUBPIECE COUNT");
            fprintf(fpcsv, "%s,", "REQUEST SUBPIECE COUNT");
            fprintf(fpcsv, "%s,", "SUPPLY SUBPIECE COUNT");
            fprintf(fpcsv, "%s\n", "TIME OF NO RESPONSE");

#endif
        }

        time_t now = time(0);
        char str_now[MAX_PATH]; 

        if(m_ip == "ppsucai.pptv.com")
        {
            fclose(fpcsv);
            return;
        }

        strftime(str_now, sizeof(str_now), "%Y/%m/%d %X",localtime(&now)); 
        fprintf(fpcsv, "%s", str_now);                //当前时间 

        fprintf(fpcsv, ",%d", m_total_http);         //http总下载
        fprintf(fpcsv, ",%d", m_total_p2p);         //p2p总下载
        fprintf(fpcsv, ",%d", m_udp_server_speed);         //UdpServer总下载
        fprintf(fpcsv, ",%s", m_save_rate);            //节约带宽比
        fprintf(fpcsv, ",%s", m_ip);                //CDN IP
        fprintf(fpcsv, ",%s", m_strLocation);        //CDN 地域
        fprintf(fpcsv, ",%d", m_http_server_status);//CDN 状态

        fprintf(fpcsv, ",%s", m_receive);            //总收到

        fprintf(fpcsv, ",%s", m_unic_request);        //无重复请求
        fprintf(fpcsv, ",%s", m_unic_receive);        //无重复收到
        fprintf(fpcsv, ",%s", send_subpiece_count_);  // 发送
        fprintf(fpcsv, ",%s", m_lost_rate);            //丢包率

        fprintf(fpcsv, ",%s", m_re_rate);            //冗余率

        fprintf(fpcsv, ",%d", m_live_http_speed);         //http总速度
        fprintf(fpcsv, ",%s", m_live_p2p_speed);          //P2P总速度
        fprintf(fpcsv, ",%d", m_live_p2p_subpiece_speed); //p2p速度

        fprintf(fpcsv, ",%s", m_switch_state);       //状态机
        fprintf(fpcsv, ",%s", m_peer_connect_count); //连接数
        fprintf(fpcsv, ",%d", connect_peer);        //连接上的peer
        fprintf(fpcsv, ",%d", ippool);                  //备选peer
        fprintf(fpcsv, ",%d", m_cache_size);        // 缓存区间
        fprintf(fpcsv, ",%s", m_cache_interval);         // 缓存区间
        fprintf(fpcsv, ",%d", m_playing_position);  // 播放点
        fprintf(fpcsv, ",%d", m_rest_time);         // 剩余时间
        fprintf(fpcsv, ",%d", m_datarate);          // 码流率
        fprintf(fpcsv, ",%d", m_left_capacity);     // 内存池剩余
        fprintf(fpcsv, ",%d", upload_connection_count_);     // 上传速度
        fprintf(fpcsv, ",%d", upload_speed_);     // 上传连接数
        fprintf(fpcsv, ",%d", upload_speed_limit_);     // 上传限速
        fprintf(fpcsv, ",%d", m_pms_status);     // PMS
        fprintf(fpcsv, ",%d", m_mem_unique_id);     // 播放器ID
        fprintf(fpcsv, ",%d", m_live_pause);     // 暂停
        fprintf(fpcsv, ",%d", m_live_replay);     // 回拖
        fprintf(fpcsv, ",%d", m_data_rate_level_);     // 码流率等级
        fprintf(fpcsv, ",%s", slide_lost_rate_);     // 滑动丢包率
        fprintf(fpcsv, ",%s", slide_re_rate_);     // 滑动冗余率
        fprintf(fpcsv, ",%s", m_udp_server_speed);     // UdpServer速度
        fprintf(fpcsv, ",%d", m_jump_times_);     // 跳跃次数
        fprintf(fpcsv, ",%d", m_checksum_failed_times_);     // 校验失败
        fprintf(fpcsv, ",%s", missing_existing_subpiece_count_);     // 缺-有
        fprintf(fpcsv, ",%d", m_live_point_);     // 直播点


        for (int i = 0; i < connect_peer; ++i)
        {
            char strIP[MAX_PATH];
            Value2IP(m_last_pc[i].PeerInfo.DetectIP, strIP);
            fprintf(fpcsv, ",%s", strIP);       //detect ip

            Value2IP(m_last_pc[i].PeerInfo.IP, strIP);
            fprintf(fpcsv, ",%s", strIP);       //ip

            // 地域
            if (m_last_pc[i].PeerInfo.DetectIP == 0)
            {
                Value2IP(m_last_pc[i].PeerInfo.IP, strIP);
            }
            else
            {
                Value2IP(m_last_pc[i].PeerInfo.DetectIP, strIP);
            }

            string loc_result;
            loc_result = m_locater.getIpAddr(string(strIP));
            fprintf(fpcsv, ",%s", loc_result.c_str());       //location

            fprintf(fpcsv, ",%d", m_last_pc[i].FirstLiveBlockID);       //FirstLiveBlockID
            fprintf(fpcsv, ",%d", m_last_pc[i].LastLivePieceID);       //LastLivePieceID
            fprintf(fpcsv, ",%d", m_last_pc[i].AssignedSubPieceCount);       //AssignedSubPieceCount
            fprintf(fpcsv, ",%d", m_last_pc[i].WindowSize);       //WindowSize
            fprintf(fpcsv, ",%d", m_last_pc[i].Sent_Count);       //Sent_Count
            fprintf(fpcsv, ",%d", m_last_pc[i].Requesting_Count);       //Requesting_Count
            fprintf(fpcsv, ",%d", m_last_pc[i].Received_Count);       //Received_Count
            fprintf(fpcsv, ",%d", m_last_pc[i].RTT_Average);       //RTT_Average
            fprintf(fpcsv, ",%d", m_last_pc[i].RTT_Max);       //RTT_Max
            fprintf(fpcsv, ",%d", m_last_pc[i].AverageDeltaTime);       //AverageDeltaTime
            fprintf(fpcsv, ",%d", m_last_pc[i].SpeedInfo.NowDownloadSpeed);       //NowDownloadSpeed

            double rate = (m_last_pc[i].Sent_Count - m_last_pc[i].Requesting_Count - m_last_pc[i].Received_Count) * 100.0
                / (m_last_pc[i].Sent_Count - m_last_pc[i].Requesting_Count + 0.000001);
            fprintf(fpcsv, ",%.2f%%", rate);       //rate

            fprintf(fpcsv, ",%d", m_last_pc[i].ConnectType);       //ConnectType
            fprintf(fpcsv, ",%d", m_last_pc[i].RealTimePeerInfo.download_connected_count_);       //download_connected_count_
            fprintf(fpcsv, ",%d", m_last_pc[i].RealTimePeerInfo.upload_connected_count_);       //upload_connected_count_
            fprintf(fpcsv, ",%d", m_last_pc[i].RealTimePeerInfo.mine_upload_speed_);       //mine_upload_speed_
            fprintf(fpcsv, ",%d", m_last_pc[i].RealTimePeerInfo.max_upload_speed_);       //max_upload_speed_
            fprintf(fpcsv, ",%d", m_last_pc[i].RealTimePeerInfo.rest_playable_time_);       //rest_playable_time_
            fprintf(fpcsv, ",%d", m_last_pc[i].PeerInfo.PeerNatType);       //PeerNatType
            fprintf(fpcsv, ",%d", m_last_pc[i].RealTimePeerInfo.lost_rate_);       //lost_rate_
            fprintf(fpcsv, ",%d", m_last_pc[i].RealTimePeerInfo.redundancy_rate_);       //redundancy_rate_
            fprintf(fpcsv, ",%d", m_last_pc[i].ActualAssignedSubPieceCount);       //ActualAssignedSubPieceCount
            fprintf(fpcsv, ",%d", m_last_pc[i].RequestSubPieceCount);       //RequestSubPieceCount
            fprintf(fpcsv, ",%d", m_last_pc[i].SupplySubPieceCount);       //SupplySubPieceCount
            fprintf(fpcsv, ",%d", m_last_pc[i].TimeOfNoResponse);       //TimeOfNoResponse
        }
        fprintf(fpcsv, "\n");

        fclose(fpcsv);
    }
}

void LiveP2PDlg::ClearContent()
{
    m_live_http_speed = 0;
    m_live_p2p_speed = "";
    m_switch_state = "";
    connect_peer = 0;
    ippool = 0;
    m_receive = "";
    m_re_rate = "";
    m_lost_rate = "";
    m_ip = "";
    m_save_rate = "";
    m_strLocation = "";
    m_http_server_status = 0;
    m_total_http = 0;
    m_total_p2p = 0;
    m_live_p2p_subpiece_speed = 0;
    m_unic_request = "";
    m_unic_receive = "";
    m_peer_connect_count = "";
    m_total_p2p = 0;
    m_save_rate = "";
    m_datarate = 0;
    m_cache_size = 0;
    m_cache_interval = "";
    m_playing_position = 0;
    m_left_capacity = 0;
    m_rest_time = 0;

    UpdateData(TRUE);
}

void LiveP2PDlg::MonitorFillZero()
{
    m_live_http_speed = 0;
    m_live_p2p_speed = "";
    m_switch_state = "";
    peer_count = 0;
    connect_peer = 0;
    ippool = 0;
    m_receive = "";
    m_re_rate = "";
    m_lost_rate = "";
    m_ip = "";
    m_save_rate = "";
    m_strLocation = "";
    m_http_server_status = 0;
    m_total_http = 0;
    m_total_p2p = 0;
    m_live_p2p_subpiece_speed = 0;
    m_unic_request = "";
    m_unic_receive = "";
    m_peer_connect_count = "";
    m_total_p2p = 0;
    m_save_rate = "";
    m_datarate = 0;
    m_cache_size = 0;
    m_cache_interval = "";
    m_playing_position = 0;
    m_left_capacity = 0;
    m_rest_time = 0;
    m_peerlist.DeleteAllItems();
    m_ridlist.ResetContent();

    memset(m_unique_requests, 0, sizeof(m_unique_requests));
    memset(m_receives, 0, sizeof(m_receives));
    memset(m_unique_receives, 0, sizeof(m_unique_receives));
    memset(m_send_subpiece_count, 0, sizeof(m_send_subpiece_count));

    UpdateData(FALSE);
}

void LiveP2PDlg::OnLbnSelchangeRidList()
{
    // TODO: Add your control notification handler code here

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

        // int pos = rid.Find('_');

        // rid = rid.Mid(pos+1);

        if (TRUE == m_is_online)
        {
            LIVE_DOWNLOADDRIVER_STATISTIC_INFO_EX m_live_download_info_ex = LIVE_DOWNLOADDRIVER_STATISTIC_INFO_EX(100);
            LIVE_DOWNLOADDRIVER_STATISTIC_INFO& m_live_download_info = m_live_download_info_ex.Ref();

            for (int i=0; i<LIVEDOWNLOADER_MAX_COUNT; i++)
            {
                if (m_si.LiveDownloadDriverIDs[i] != 0)
                {
                    CString str;
                    str.Format("%d", m_si.LiveDownloadDriverIDs[i]);
                    if (((CP2PMonitorDlg*)GetParent()->GetParent())->ReadShareMemory("LIVEDOWNLOADDRIVER_" + m_strProcessID + "_" + str, m_live_download_info_ex.Size(), &m_live_download_info))
                    {
                        char buf[MAX_PATH] = "";
                        sprintf(buf, "%d_%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X", i, m_live_download_info.ResourceID.Data1, m_live_download_info.ResourceID.Data2, m_live_download_info.ResourceID.Data3, 
                            m_live_download_info.ResourceID.Data4[0], m_live_download_info.ResourceID.Data4[1], 
                            m_live_download_info.ResourceID.Data4[2], m_live_download_info.ResourceID.Data4[3], 
                            m_live_download_info.ResourceID.Data4[4], m_live_download_info.ResourceID.Data4[5],
                            m_live_download_info.ResourceID.Data4[6], m_live_download_info.ResourceID.Data4[7]);

                        if (rid.CompareNoCase(buf) != 0)
                        {
                            continue;
                        }

                        m_live_http_speed = m_live_download_info.LiveHttpSpeedInfo.NowDownloadSpeed / 1024;

                        m_live_p2p_speed.Format("%d - %d - %d - %d", m_live_download_info.LiveP2PSpeedInfo.NowDownloadSpeed / 1024,
                            m_live_download_info.LiveP2PSubPieceSpeedInfo.NowDownloadSpeed / 1024,
                            m_live_download_info.P2PPeerSpeedInSecond / 1024,
                            m_live_download_info.LiveP2PSubPieceSpeedInfo.MinuteDownloadSpeed / 1024);

                        m_udp_server_speed.Format("%d - %d", m_live_download_info.UdpServerSpeedInfo.NowDownloadSpeed / 1024,
                            m_live_download_info.P2PUdpServerSpeedInSecond / 1024);
                        m_switch_state.Format("%d%d", m_live_download_info.http_state, m_live_download_info.p2p_state);

                        int total_receive = m_live_download_info.TotalUnusedSubPieceCount;
                        int total_requestint_count = 0;
                        for (int i = 0; i< m_live_download_info.PeerCount; i++)
                        {
                            total_requestint_count += m_live_download_info.P2PConnections[i].Requesting_Count;
                        }

                        m_unique_requests[rate_index] = m_live_download_info.TotalRequestSubPieceCount;
                        m_receives[rate_index] = m_live_download_info.TotalUnusedSubPieceCount;
                        m_unique_receives[rate_index] = m_live_download_info.TotalRecievedSubPieceCount;

                        m_lost_rate.Format("%.2f%%", (m_live_download_info.TotalRequestSubPieceCount - total_requestint_count - m_live_download_info.TotalUnusedSubPieceCount)* 100.0
                            / (m_live_download_info.TotalRequestSubPieceCount - total_requestint_count + 0.000001));
                        m_re_rate.Format("%.2f%%", (total_receive - m_live_download_info.TotalRecievedSubPieceCount) * 100.0 / (total_receive+0.0000001));

                        boost::uint32_t total_unique_request_subpiece_count_in_period = m_unique_requests[rate_index] - m_unique_requests[(rate_index + 1) % 20];
                        boost::uint32_t total_unique_receive_subpiece_count_in_period = m_unique_receives[rate_index] - m_unique_receives[(rate_index + 1) % 20];
                        boost::uint32_t total_receive_subpiece_count_in_period = m_receives[rate_index] - m_receives[(rate_index + 1) % 20];

                        UINT request_subpiece_count_in_second = rate_index == 0 ?
                            m_unique_requests[rate_index] - m_unique_requests[19] : m_unique_requests[rate_index] - m_unique_requests[rate_index - 1];

                        UINT unique_receive_subpiece_count_in_second = rate_index == 0 ?
                            m_unique_receives[rate_index] - m_unique_receives[19] : m_unique_receives[rate_index] - m_unique_receives[rate_index - 1];

                        UINT reveive_subpiece_count_in_second = rate_index == 0 ?
                            m_receives[rate_index] - m_receives[19] : m_receives[rate_index] - m_receives[rate_index - 1];

                        m_receive.Format("%d - %d", m_live_download_info.TotalUnusedSubPieceCount,
                            reveive_subpiece_count_in_second);
                        m_unic_request.Format("%d - %d", m_live_download_info.TotalRequestSubPieceCount,
                            request_subpiece_count_in_second);
                        m_unic_receive.Format("%d - %d", m_live_download_info.TotalRecievedSubPieceCount,
                            unique_receive_subpiece_count_in_second);

                        boost::uint8_t tmp_index = second_index_ == 0 ? 19 : second_index_ - 1;
                        UINT send_subpiece_count_in_second = tmp_index == 0 ?
                            m_send_subpiece_count[tmp_index] - m_send_subpiece_count[19] :
                            m_send_subpiece_count[tmp_index] - m_send_subpiece_count[tmp_index - 1];

                        send_subpiece_count_.Format("%d - %d", m_send_subpiece_count[tmp_index],
                            send_subpiece_count_in_second);

                        if (++rate_index >= 20)
                        {
                            rate_index = 0;
                        }

                        if (total_unique_request_subpiece_count_in_period < total_requestint_count + total_receive_subpiece_count_in_period)
                        {
                            slide_lost_rate_.Format("%s", "0.00%");
                        }
                        else
                        {
                            slide_lost_rate_.Format("%.2f%%", (total_unique_request_subpiece_count_in_period - total_requestint_count - total_receive_subpiece_count_in_period)* 100.0
                                / (total_unique_request_subpiece_count_in_period - total_requestint_count + 0.000001));
                        }

                        slide_re_rate_.Format("%.2f%%", (total_receive_subpiece_count_in_period - total_unique_receive_subpiece_count_in_period) * 100.0
                            / (total_receive_subpiece_count_in_period+0.0000001));

                        char *ip = (char*)(m_live_download_info.OriginalUrl);
                        m_ip.Format("%s", ip);
                        int pos = m_ip.Find('//', 0);
                        m_ip = m_ip.Mid(pos+1);

                        pos = m_ip.Find('//', 0);
                        m_ip = m_ip.Mid(pos+1);

                        pos = m_ip.Find(':', 0);
                        m_ip = m_ip.Mid(0, pos);

                        string loc_result;

                        loc_result = m_locater.getIpAddr(string(m_ip.GetBuffer()));
                        m_strLocation.Format("%s", loc_result.c_str());

                        m_http_server_status = m_live_download_info.LastHttpStatusCode;

                        m_total_http = m_live_download_info.LiveHttpSpeedInfo.TotalDownloadBytes / 1024;
                        m_total_p2p = m_live_download_info.TotalP2PDataBytes / 1024;
                        m_udpserver_download_bytes = m_live_download_info.TotalUdpServerDataBytes / 1024;

                        m_live_p2p_subpiece_speed = m_live_download_info.LiveP2PSubPieceSpeedInfo.NowDownloadSpeed / 1024;

                        m_save_rate.Format("%.2f%%", m_total_p2p * 100.0 / (m_total_p2p + m_total_http + m_udpserver_download_bytes));

                        peer_count = m_live_download_info_ex.Ref().PeerCount;
                        for (int i = 0; i < peer_count; ++i)
                        {
                            m_last_pc[i] = m_live_download_info_ex.Ref().P2PConnections[i];
                        }

                        connect_peer = m_live_download_info_ex.Ref().PeerCount;
                        ippool = m_live_download_info_ex.Ref().IpPoolPeerCount;

                        m_peer_connect_count.Format("%d - %d", connect_peer, ippool);

                        m_datarate = m_live_download_info.DataRate;

                        m_cache_size = m_live_download_info.CacheSize;
                        
                        m_cache_start = m_live_download_info.CacheFirstPieceID;
                        m_cache_end = m_live_download_info.CacheLastPieceID;

                        m_cache_interval.Format("%d - %d", m_live_download_info.CacheFirstPieceID, m_live_download_info.CacheLastPieceID);

                        m_playing_position = m_live_download_info.PlayingPosition;

                        m_left_capacity = m_live_download_info.LeftCapacity;

                        m_rest_time = m_live_download_info.RestPlayTime;

                        m_live_point_ = m_live_download_info.LivePointBlockId;

                        m_data_rate_level_ = m_live_download_info.DataRateLevel;

                        m_jump_times_ = m_live_download_info.JumpTimes;

                        m_checksum_failed_times_ = m_live_download_info.NumOfChecksumPieces;

                        m_pms_status = m_live_download_info.PmsStatus;

                        m_mem_unique_id = m_live_download_info.UniqueID;

                        m_live_pause = m_live_download_info.IsPaused;

                        m_live_replay = m_live_download_info.IsReplay;

                        missing_existing_subpiece_count_.Format("%d - %d", m_live_download_info.MissingSubPieceCountOfFirstBlock,
                            m_live_download_info.ExistSubPieceCountOfFirstBlock);

                    }
                }
            }
        }
        else if (1 == m_import_file_type)
        {
            Display(rid);
        }

        ShowReport();
        UpdateData(FALSE);

        if (m_is_saving)
        {
            Export();
        }
    }
}

void LiveP2PDlg::Display(CString rid)
{
    for (int i = 0; i < m_statistics_datas[m_second]->GetLiveDownloadDriverStatistics().size(); ++i)
    {
        char buf[MAX_PATH] = "";
        std::string tmp_rid = m_statistics_datas[m_second]->GetLiveDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo().ResourceID.to_string();
        sprintf(buf, "%d_%s", i, tmp_rid.c_str());

        if (rid.CompareNoCase(buf) != 0)
        {
            continue;
        }

        statistic::LIVE_DOWNLOADDRIVER_STATISTIC_INFO m_live_download_info =
            m_statistics_datas[m_second]->GetLiveDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo();

        m_live_http_speed = m_live_download_info.LiveHttpSpeedInfo.NowDownloadSpeed / 1024;

        m_live_p2p_speed.Format("%d - %d - %d - %d", m_live_download_info.LiveP2PSpeedInfo.NowDownloadSpeed / 1024,
            m_live_download_info.LiveP2PSubPieceSpeedInfo.NowDownloadSpeed / 1024,
            m_live_download_info.P2PPeerSpeedInSecond / 1024,
            m_live_download_info.LiveP2PSubPieceSpeedInfo.MinuteDownloadSpeed / 1024);

        m_udp_server_speed.Format("%d - %d", m_live_download_info.UdpServerSpeedInfo.NowDownloadSpeed / 1024,
            m_live_download_info.P2PUdpServerSpeedInSecond / 1024);
        m_switch_state.Format("%d%d", m_live_download_info.http_state, m_live_download_info.p2p_state);

        int total_receive = m_live_download_info.TotalUnusedSubPieceCount;
        int total_requestint_count = 0;
        for (int i = 0; i< m_live_download_info.PeerCount; i++)
        {
            total_requestint_count += m_live_download_info.P2PConnections[i].Requesting_Count;
        }

        m_unique_requests[rate_index] = m_live_download_info.TotalRequestSubPieceCount;
        m_receives[rate_index] = m_live_download_info.TotalUnusedSubPieceCount;
        m_unique_receives[rate_index] = m_live_download_info.TotalRecievedSubPieceCount;

        m_lost_rate.Format("%.2f%%", (m_live_download_info.TotalRequestSubPieceCount - total_requestint_count - m_live_download_info.TotalUnusedSubPieceCount)* 100.0
            / (m_live_download_info.TotalRequestSubPieceCount - total_requestint_count + 0.000001));
        m_re_rate.Format("%.2f%%", (total_receive - m_live_download_info.TotalRecievedSubPieceCount) * 100.0 / (total_receive+0.0000001));

        boost::uint32_t total_unique_request_subpiece_count_in_period = m_unique_requests[rate_index] - m_unique_requests[(rate_index + 1) % 20];
        boost::uint32_t total_unique_receive_subpiece_count_in_period = m_unique_receives[rate_index] - m_unique_receives[(rate_index + 1) % 20];
        boost::uint32_t total_receive_subpiece_count_in_period = m_receives[rate_index] - m_receives[(rate_index + 1) % 20];

        UINT request_subpiece_count_in_second = rate_index == 0 ?
            m_unique_requests[rate_index] - m_unique_requests[19] : m_unique_requests[rate_index] - m_unique_requests[rate_index - 1];

        UINT unique_receive_subpiece_count_in_second = rate_index == 0 ?
            m_unique_receives[rate_index] - m_unique_receives[19] : m_unique_receives[rate_index] - m_unique_receives[rate_index - 1];

        UINT reveive_subpiece_count_in_second = rate_index == 0 ?
            m_receives[rate_index] - m_receives[19] : m_receives[rate_index] - m_receives[rate_index - 1];

        m_receive.Format("%d - %d", m_live_download_info.TotalUnusedSubPieceCount,
            reveive_subpiece_count_in_second);
        m_unic_request.Format("%d - %d", m_live_download_info.TotalRequestSubPieceCount,
            request_subpiece_count_in_second);
        m_unic_receive.Format("%d - %d", m_live_download_info.TotalRecievedSubPieceCount,
            unique_receive_subpiece_count_in_second);

        boost::uint8_t tmp_index = second_index_ == 0 ? 19 : second_index_ - 1;
        UINT send_subpiece_count_in_second = tmp_index == 0 ?
            m_send_subpiece_count[tmp_index] - m_send_subpiece_count[19] :
        m_send_subpiece_count[tmp_index] - m_send_subpiece_count[tmp_index - 1];

        send_subpiece_count_.Format("%d - %d", m_send_subpiece_count[tmp_index],
            send_subpiece_count_in_second);

        if (++rate_index >= 20)
        {
            rate_index = 0;
        }

        if (total_unique_request_subpiece_count_in_period < total_requestint_count + total_receive_subpiece_count_in_period)
        {
            slide_lost_rate_.Format("%s", "0.00%");
        }
        else
        {
            slide_lost_rate_.Format("%.2f%%", (total_unique_request_subpiece_count_in_period - total_requestint_count - total_receive_subpiece_count_in_period)* 100.0
                / (total_unique_request_subpiece_count_in_period - total_requestint_count + 0.000001));
        }

        slide_re_rate_.Format("%.2f%%", (total_receive_subpiece_count_in_period - total_unique_receive_subpiece_count_in_period) * 100.0
            / (total_receive_subpiece_count_in_period+0.0000001));char *ip = (char*)(m_live_download_info.OriginalUrl);
        m_ip.Format("%s", ip);
        int pos = m_ip.Find('//', 0);
        m_ip = m_ip.Mid(pos+1);

        pos = m_ip.Find('//', 0);
        m_ip = m_ip.Mid(pos+1);

        pos = m_ip.Find(':', 0);
        m_ip = m_ip.Mid(0, pos);

        string loc_result;

        loc_result = m_locater.getIpAddr(string(m_ip.GetBuffer()));
        m_strLocation.Format("%s", loc_result.c_str());

        m_http_server_status = m_live_download_info.LastHttpStatusCode;

        m_total_http = m_live_download_info.LiveHttpSpeedInfo.TotalDownloadBytes / 1024;
        m_total_p2p = m_live_download_info.TotalP2PDataBytes / 1024;
        m_udpserver_download_bytes = m_live_download_info.TotalUdpServerDataBytes / 1024;

        m_live_p2p_subpiece_speed = m_live_download_info.LiveP2PSubPieceSpeedInfo.NowDownloadSpeed / 1024;

        m_save_rate.Format("%.2f%%", m_total_p2p * 100.0 / (m_total_p2p + m_total_http + m_udpserver_download_bytes));

        peer_count = m_live_download_info.PeerCount;
        for (int i = 0; i < peer_count; ++i)
        {
            m_last_pc[i] = m_live_download_info.P2PConnections[i];
        }

        connect_peer = m_live_download_info.PeerCount;
        //peer_count = connect_peer;
        ippool = m_live_download_info.IpPoolPeerCount;

        m_peer_connect_count.Format("%d - %d", connect_peer, ippool);

        m_datarate = m_live_download_info.DataRate;

        m_cache_size = m_live_download_info.CacheSize;

        m_cache_start = m_live_download_info.CacheFirstBlockId;
        m_cache_end = m_live_download_info.CacheLastBlockId;

        m_cache_interval.Format("%d - %d", m_live_download_info.CacheFirstBlockId, m_live_download_info.CacheLastBlockId);

        m_playing_position = m_live_download_info.PlayingPosition;

        m_left_capacity = m_live_download_info.LeftCapacity;

        m_rest_time = m_live_download_info.RestPlayTime;

        m_live_point_ = m_live_download_info.LivePointBlockId;

        m_data_rate_level_ = m_live_download_info.DataRateLevel;

        m_jump_times_ = m_live_download_info.JumpTimes;

        m_checksum_failed_times_ = m_live_download_info.NumOfChecksumFailedPieces;

        m_pms_status = m_live_download_info.PmsStatus;

        m_mem_unique_id = m_live_download_info.UniqueID;

        m_live_pause = m_live_download_info.IsPaused;

        m_live_replay = m_live_download_info.IsReplay;

        missing_existing_subpiece_count_.Format("%d - %d", m_live_download_info.MissingSubPieceCountOfFirstBlock,
            m_live_download_info.ExistSubPieceCountOfFirstBlock);
    }
}

void LiveP2PDlg::OnSize(UINT nType, int cx, int cy)
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

void LiveP2PDlg::ChangeLabelSize(int cx, int cy, int w, int h)
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

void LiveP2PDlg::AddLabelInfo()
{
    m_label_id.push_back(IDC_STATIC29);
    m_label_id.push_back(IDC_RID_LIST);
    m_label_id.push_back(IDC_STATIC4);
    m_label_id.push_back(IDC_STATIC5);
    m_label_id.push_back(IDC_LIVE_TOTAL_HTTP);
    m_label_id.push_back(IDC_STATIC6);
    m_label_id.push_back(IDC_LIVE_TOTAL_P2P);
    m_label_id.push_back(IDC_STATIC13);
    m_label_id.push_back(IDC_EDIT_CDNIP);
    m_label_id.push_back(IDC_STATIC14);
    m_label_id.push_back(IDC_EDIT_BAND_SAVE);
    m_label_id.push_back(IDC_STATIC15);
    m_label_id.push_back(IDC_EDIT_CDN_LOCATION);
    m_label_id.push_back(IDC_STATIC16);
    m_label_id.push_back(IDC_EDIT_CDN_STATUS);
    m_label_id.push_back(IDC_STATIC8);
    m_label_id.push_back(IDC_STATIC18);
    m_label_id.push_back(IDC_REQUEST);
    m_label_id.push_back(IDC_STATIC10);
    m_label_id.push_back(IDC_TOTAL_RECV);
    m_label_id.push_back(IDC_STATIC19);
    m_label_id.push_back(IDC_RECEIVE);
    m_label_id.push_back(IDC_STATIC12);
    m_label_id.push_back(IDC_LOST_RATE);
    m_label_id.push_back(IDC_STATIC11);
    m_label_id.push_back(IDC_RETRY_RATE);
    m_label_id.push_back(IDC_STATIC22);
    m_label_id.push_back(IDC_EDIT_DATA_RATE);
    m_label_id.push_back(IDC_STATIC31);
    m_label_id.push_back(IDC_EDIT9);
    m_label_id.push_back(IDC_STATIC7);
    m_label_id.push_back(IDC_STATIC_HTTP);
    m_label_id.push_back(IDC_HTTP_SPEED);
    m_label_id.push_back(IDC_STATIC_P2P);
    m_label_id.push_back(IDC_P2P_SPEED);
    m_label_id.push_back(IDC_STATIC21);
    m_label_id.push_back(IDC_EDIT_LIVE_CONNECT);
    m_label_id.push_back(IDC_STATIC38);
    m_label_id.push_back(IDC_STATIC39);
    m_label_id.push_back(IDC_EDIT_JUMP_TIMES);
    m_label_id.push_back(IDC_STATIC40);
    m_label_id.push_back(IDC_EDIT_CHECKSUM_FAILED_TIMES);
    m_label_id.push_back(IDC_STATIC28);
    m_label_id.push_back(IDC_EDIT8);
    m_label_id.push_back(IDC_STATIC3);
    m_label_id.push_back(IDC_EDIT_SWITCH);
    m_label_id.push_back(IDC_STATIC26);
    m_label_id.push_back(IDC_STATIC23);
    m_label_id.push_back(IDC_CACHE_SIZE);
    m_label_id.push_back(IDC_STATIC24);
    m_label_id.push_back(IDC_CACHE_INTERVAL);
    m_label_id.push_back(IDC_STATIC25);
    m_label_id.push_back(IDC_PLAYING_POSITION);
    m_label_id.push_back(IDC_STATIC32);
    m_label_id.push_back(IDC_EDIT10);
    m_label_id.push_back(IDC_STATIC27);
    m_label_id.push_back(IDC_LEFT_CAPACITY);
    m_label_id.push_back(IDC_LIST1);
    m_label_id.push_back(IDC_STATIC47);
    m_label_id.push_back(IDC_EDIT_UDPSERVER_DOWNLOAD);
    m_label_id.push_back(IDC_STATIC48);
    m_label_id.push_back(IDC_EDIT_PMS_STATUS);
    m_label_id.push_back(IDC_STATIC49);
    m_label_id.push_back(IDC_EDIT_UNIQUE_ID);
    m_label_id.push_back(IDC_STATIC5050);
    m_label_id.push_back(IDC_EDIT_PAUSE);
    m_label_id.push_back(IDC_STATIC51);
    m_label_id.push_back(IDC_EDIT_REPLAY);
    m_label_id.push_back(IDC_STATIC50);
    m_label_id.push_back(IDC_EDIT_UDPSERVER_SPEED);
    m_label_id.push_back(IDC_STATIT_UPLOAD_GROUP);
    m_label_id.push_back(IDC_STATIC_UPLOAD_SPEED2);
    m_label_id.push_back(IDC_EDIT_UPLOAD_SPEED2);
    m_label_id.push_back(IDC_STATIC_UPLOAD_CONNECTION_COUNT2);
    m_label_id.push_back(IDC_EDIT_UPLOAD_CONNECTION_COUNT2);
    m_label_id.push_back(IDC_STATIC_UPLOAD_LIMIT_SPEED2);
    m_label_id.push_back(IDC_EDIT_UPLOAD_LIMIT_SPEED2);
    m_label_id.push_back(IDC_STATIC_SLIDE_LOST_RATE);
    m_label_id.push_back(IDC_EDIT_SLIDE_LOST_RATE);
    m_label_id.push_back(IDC_STATIC_SLIDE_RE_RATE);
    m_label_id.push_back(IDC_EDIT_SLIDE_RE_RATE);
    m_label_id.push_back(IDC_STATIC_MISSING_EXIST_SUBPIECE_COUNT_OF_FIRST_BLOCK);
    m_label_id.push_back(IDC_EDIT_MISSING_EXIST_SUBPIECE_COUNT);
    m_label_id.push_back(IDC_STATIC_SEND);
    m_label_id.push_back(IDC_EDIT_SEND);
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

void LiveP2PDlg::ShowStatisticLog()
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

    if (1 == m_import_file_type)
    {
        for (int i = 0; i < m_statistics_datas[m_second]->GetLiveDownloadDriverStatistics().size(); ++i)
        {
            //DOWNLOADDRIVER_STATISTIC_INFO dd_info(m_statistics_datas[m_second]->GetVodDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo());

            char buf[MAX_PATH] = "";
            std::string rid = m_statistics_datas[m_second]->GetLiveDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo().ResourceID.to_string();
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

void LiveP2PDlg::OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult)
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

void LiveP2PDlg::SetListReportState(BOOL is_list_update)
{
    m_list_update = is_list_update;
}

void LiveP2PDlg::ReadFile()
{
    int index1 = 0, index2;
    index2 = m_import_file[m_line_number].Find(',');
    // 当前时间，跳过

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_total_http = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//http总下载

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_total_p2p = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//p2p总下载

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_udpserver_download_bytes = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//UdpServer总下载

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_save_rate = m_import_file[m_line_number].Mid(index1, index2 - index1);//节约带宽比

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_ip = m_import_file[m_line_number].Mid(index1, index2 - index1);//CDN IP

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_strLocation = m_import_file[m_line_number].Mid(index1, index2 - index1);//CDN地域

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_http_server_status = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//CDN状态

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_receive = m_import_file[m_line_number].Mid(index1, index2 - index1);//总收到

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_unic_request = m_import_file[m_line_number].Mid(index1, index2 - index1);//无重复请求

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_unic_receive = m_import_file[m_line_number].Mid(index1, index2 - index1);//无重复收到

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    send_subpiece_count_ = m_import_file[m_line_number].Mid(index1, index2 - index1);//发送

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_lost_rate = m_import_file[m_line_number].Mid(index1, index2 - index1);//丢包率

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_re_rate = m_import_file[m_line_number].Mid(index1, index2 - index1);//冗余率

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_live_http_speed = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//http总速度

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_live_p2p_speed = m_import_file[m_line_number].Mid(index1, index2 - index1);//p2p总速度

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_live_p2p_subpiece_speed = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//p2p速度

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_switch_state = m_import_file[m_line_number].Mid(index1, index2 - index1);//状态机

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_peer_connect_count = m_import_file[m_line_number].Mid(index1, index2 - index1);//连接数

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    connect_peer = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//连接节点数
    peer_count = connect_peer;

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    ippool = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//IPPool

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_cache_size = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//缓存大小

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_cache_interval = m_import_file[m_line_number].Mid(index1, index2 - index1);//缓存区间

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_playing_position = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//播放点

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_rest_time = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//剩余时间

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_datarate = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//码流率

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_left_capacity = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//内存池剩余

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    upload_connection_count_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//上传速度

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    upload_speed_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//上传连接数

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    upload_speed_limit_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//上传限速

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_pms_status = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//PMS

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_mem_unique_id = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//播放器ID

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_live_pause = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//暂停

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_live_replay = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//回拖

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_data_rate_level_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//码流率等级

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    slide_lost_rate_  = m_import_file[m_line_number].Mid(index1, index2 - index1);//滑动丢包率

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    slide_re_rate_ = m_import_file[m_line_number].Mid(index1, index2 - index1);//滑动冗余率

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_udp_server_speed = m_import_file[m_line_number].Mid(index1, index2 - index1);//UdpServer速度

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_jump_times_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//跳跃次数

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_checksum_failed_times_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//校验失败

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    missing_existing_subpiece_count_ = m_import_file[m_line_number].Mid(index1, index2 - index1);//缺-有

    index1 = index2 + 1;
    index2 = m_import_file[m_line_number].Find(',', index1);
    m_live_point_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));//直播点

    UpdateData(FALSE);

    CreatePeerList(index1, index2);
}

void LiveP2PDlg::CreatePeerList(int index1, int index2)
{
    for (int i = 0; i < peer_count; ++i)
    {
        P2P_CONNECTION_INFO info;

        string str;
        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        str = m_import_file[m_line_number].Mid(index1, index2 - index1);  // detect ip
        info.PeerInfo.DetectIP = IP2Value(str);

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        str = m_import_file[m_line_number].Mid(index1, index2 - index1);  // ip
        info.PeerInfo.IP = IP2Value(str);

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);  // location 

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.FirstLiveBlockID = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // FirstLiveBlockID

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.LastLivePieceID = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // LastLivePieceID

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.AssignedSubPieceCount = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // AssignedSubPieceCount

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.WindowSize = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // WindowSize

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.Sent_Count = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // Sent_Count

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.Requesting_Count = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // Requesting_Count

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.Received_Count = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // Received_Count

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.RTT_Average = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // RTT_Average

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.RTT_Max = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // RTT_Max

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.AverageDeltaTime = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // AverageDeltaTime

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.SpeedInfo.NowDownloadSpeed = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // SpeedInfo.NowDownloadSpeed

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1); // lost rate

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.ConnectType = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // ConnectType

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.RealTimePeerInfo.download_connected_count_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // RealTimePeerInfo.download_connected_count_

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.RealTimePeerInfo.upload_connected_count_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // RealTimePeerInfo.upload_connected_count_

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.RealTimePeerInfo.mine_upload_speed_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // RealTimePeerInfo.mine_upload_speed_

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.RealTimePeerInfo.max_upload_speed_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // RealTimePeerInfo.max_upload_speed_

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.RealTimePeerInfo.rest_playable_time_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // RealTimePeerInfo.rest_playable_time_

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.PeerInfo.PeerNatType = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // PeerInfo.PeerNatType

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.RealTimePeerInfo.lost_rate_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // RealTimePeerInfo.lost_rate_

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.RealTimePeerInfo.redundancy_rate_ = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // RealTimePeerInfo.redundancy_rate_

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.ActualAssignedSubPieceCount = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // ActualAssignedSubPieceCount

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.RequestSubPieceCount = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // RequestSubPieceCount

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.SupplySubPieceCount = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // SupplySubPieceCount

        index1 = index2 + 1;
        index2 = m_import_file[m_line_number].Find(',', index1);
        info.TimeOfNoResponse = atoi(m_import_file[m_line_number].Mid(index1, index2 - index1));  // TimeOfNoResponse

        m_last_pc[i] = info;
    }

    ShowReport();
}
