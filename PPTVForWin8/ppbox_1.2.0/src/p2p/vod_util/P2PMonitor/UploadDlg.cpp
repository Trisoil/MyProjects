// UploadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "P2PMonitor.h"
#include "UploadDlg.h"
#include "P2PMonitorDlg.h"

#include "DataStruct.h"
#include "Common.h"


// CUploadDlg 对话框

IMPLEMENT_DYNAMIC(CUploadDlg, CDialog)

CUploadDlg::CUploadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUploadDlg::IDD, pParent)
    , m_speed_limit(0)
    , m_upload_count(0)
    , m_list_update(TRUE)
    , m_sort(0)
	, m_is_created(FALSE)
    , m_upload_speed(0)
    , m_upload_speed_limit2(0)
{

}

CUploadDlg::~CUploadDlg()
{
}

BOOL CUploadDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_upload_list.InsertColumn(0, "IP");
    m_upload_list.InsertColumn(1, "Location");
    m_upload_list.InsertColumn(2, "Port");
    m_upload_list.InsertColumn(3, "Up");
    m_upload_list.InsertColumn(4, "Download Connect Count");
    m_upload_list.InsertColumn(5, "Upload Connect Count");
    m_upload_list.InsertColumn(6, "Mine Upload Speed");
    m_upload_list.InsertColumn(7, "Mine Max Upload Speed");
    m_upload_list.InsertColumn(8, "Rest Playable Time");
    m_upload_list.InsertColumn(9, "Lost Rate");
    m_upload_list.InsertColumn(10, "Redundancy Rate");

    m_upload_list.SetColumnWidth(0, 100);
    m_upload_list.SetColumnWidth(1, 150);
    m_upload_list.SetColumnWidth(2, 100);
    m_upload_list.SetColumnWidth(3, 100);
    m_upload_list.SetColumnWidth(4, 100);
    m_upload_list.SetColumnWidth(5, 100);
    m_upload_list.SetColumnWidth(6, 100);
    m_upload_list.SetColumnWidth(7, 100);
    m_upload_list.SetColumnWidth(8, 100);
    m_upload_list.SetColumnWidth(9, 100);
    m_upload_list.SetColumnWidth(10, 100);

	GetClientRect(&m_rect);
	m_up_ori_h = m_rect.Height();
	m_up_ori_w = m_rect.Width();

    return TRUE;
}

void CUploadDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_SPEED_LIMIT, m_speed_limit);
    DDX_Text(pDX, IDC_UPLOAD_COUNT, m_upload_count);
    DDX_Control(pDX, IDC_UPLOAD_LIST, m_upload_list);
    DDX_Text(pDX, IDC_EDIT_UPLOAD_SPEED, m_upload_speed);
    DDX_Text(pDX, IDC_SPEED_LIMIT2, m_upload_speed_limit2);
}


BEGIN_MESSAGE_MAP(CUploadDlg, CDialog)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_UPLOAD_LIST, &CUploadDlg::OnLvnColumnclickUploadList)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CUploadDlg 消息处理程序
void CUploadDlg::OnShareMemoryTimer()
{
    // 读共享内存并添加
    int size = sizeof(m_upload_info);
    if (((CP2PMonitorDlg*)GetParent()->GetParent())->ReadShareMemory("UPLOAD_" + m_strProcessID, sizeof(m_upload_info), &m_upload_info))
    {
        m_speed_limit = m_upload_info.speed_limit;
        m_upload_count = m_upload_info.peer_upload_count;
        m_upload_speed = m_upload_info.upload_speed;
        m_upload_speed_limit2 = m_upload_info.actual_speed_limit;

        UpdateData(FALSE);

        if (m_list_update)
        {
            for (int i=0; i<m_upload_info.peer_upload_count; i++)
            {
                m_last_[i] = m_upload_info.peer_upload_info[i];
            }
        }

        ShowReport();
    }
}

void CUploadDlg::SetProcessID(CString processid)
{
    m_strProcessID = processid;
}

void CUploadDlg::ShowReport()
{
    for(int i=0; i<m_upload_count-1; i++)
    {
        for(int j=i+1; j<m_upload_count; j++)
        {
            bool is_swap = false;

            if (m_sort_col == 3)
            {
                // upload_speed
                int a = m_last_[i].upload_speed;
                int b = m_last_[j].upload_speed;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 4)
            {
                // download connected count
                int a = m_last_[i].peer_info.download_connected_count_;
                int b = m_last_[j].peer_info.download_connected_count_;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 5)
            {
                // upload connected count
                int a = m_last_[i].peer_info.upload_connected_count_;
                int b = m_last_[j].peer_info.upload_connected_count_;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 6)
            {
                // upload speed
                int a = m_last_[i].peer_info.mine_upload_speed_;
                int b = m_last_[j].peer_info.mine_upload_speed_;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 7)
            {
                // max upload speed
                int a = m_last_[i].peer_info.max_upload_speed_;
                int b = m_last_[j].peer_info.max_upload_speed_;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 8)
            {
                // rest playable time
                int a = m_last_[i].peer_info.rest_playable_time_;
                int b = m_last_[j].peer_info.rest_playable_time_;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 9)
            {
                // lost rate
                int a = m_last_[i].peer_info.lost_rate_;
                int b = m_last_[j].peer_info.lost_rate_;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (m_sort_col == 10)
            {
                // redundancy rate
                int a = m_last_[i].peer_info.redundancy_rate_;
                int b = m_last_[j].peer_info.redundancy_rate_;
                if (m_sort * (a - b) > 0)
                {
                    is_swap = true;
                }
            }

            if (is_swap)
            {
                PEER_UPLOAD_INFO tmp = m_last_[i];
                m_last_[i] = m_last_[j];
                m_last_[j] = tmp;
            }
        }
    }

    // 列表清空
    m_upload_list.DeleteAllItems();

    for (int i=0; i<m_upload_count; i++)
    {
        // 添加历史记录
        char strIP[MAX_PATH];

        Value2IP(m_last_[i].ip, strIP);
        m_upload_list.InsertItem(i, strIP);

        CString str;

        string loc_result;
        loc_result = m_locater.getIpAddr(string(strIP));
        str.Format("%s", loc_result.c_str());
        m_upload_list.SetItem(i, 1, LVIF_TEXT, str, 0, 0, 0, 0);

        str.Format("%d", m_last_[i].port);
        m_upload_list.SetItem(i, 2, LVIF_TEXT, str, 0, 0, 0, 0);

        str.Format("%d", m_last_[i].upload_speed);
        m_upload_list.SetItem(i, 3, LVIF_TEXT, str, 0, 0, 0, 0);

        str.Format("%d", m_last_[i].peer_info.download_connected_count_);
        m_upload_list.SetItem(i, 4, LVIF_TEXT, str, 0, 0, 0, 0);

        str.Format("%d", m_last_[i].peer_info.upload_connected_count_);
        m_upload_list.SetItem(i, 5, LVIF_TEXT, str, 0, 0, 0, 0);

        str.Format("%d", m_last_[i].peer_info.mine_upload_speed_);
        m_upload_list.SetItem(i, 6, LVIF_TEXT, str, 0, 0, 0, 0);

        str.Format("%d", m_last_[i].peer_info.max_upload_speed_);
        m_upload_list.SetItem(i, 7, LVIF_TEXT, str, 0, 0, 0, 0);

        str.Format("%d", m_last_[i].peer_info.rest_playable_time_);
        m_upload_list.SetItem(i, 8, LVIF_TEXT, str, 0, 0, 0, 0);

        str.Format("%d", m_last_[i].peer_info.lost_rate_);
        m_upload_list.SetItem(i, 9, LVIF_TEXT, str, 0, 0, 0, 0);

        str.Format("%d", m_last_[i].peer_info.redundancy_rate_);
        m_upload_list.SetItem(i, 10, LVIF_TEXT, str, 0, 0, 0, 0);
    }
}

void CUploadDlg::OnLvnColumnclickUploadList(NMHDR *pNMHDR, LRESULT *pResult)
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

void CUploadDlg::SetListReportState(BOOL is_list_update)
{
    m_list_update = is_list_update;
}
void CUploadDlg::OnSize(UINT nType, int cx, int cy)
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

void CUploadDlg::ChangeLabelSize(int cx, int cy, int w, int h)
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

void CUploadDlg::AddLabelInfo()
{
	m_label_id.push_back(IDC_SPEED_LIMIT);
	m_label_id.push_back(IDC_STATIC1);
	m_label_id.push_back(IDC_STATIC2);
	m_label_id.push_back(IDC_UPLOAD_COUNT);
	m_label_id.push_back(IDC_UPLOAD_LIST);
    m_label_id.push_back(IDC_STATIC_UPLOAD_SPEED);
    m_label_id.push_back(IDC_EDIT_UPLOAD_SPEED);
    m_label_id.push_back(IDC_STATIC4);
    m_label_id.push_back(IDC_SPEED_LIMIT2);
	
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
