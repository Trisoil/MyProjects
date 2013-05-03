// LogMonitor.cpp : implementation file
//

#include "stdafx.h"
#include "P2PMonitor.h"
#include "LogMonitor.h"
#include "boost/thread/thread.hpp"

// LogMonitor dialog



IMPLEMENT_DYNAMIC(LogMonitor, CDialog)

LogMonitor::LogMonitor(CWnd* pParent /*=NULL*/)
    : CDialog(LogMonitor::IDD, pParent)
    , list1(_T(""))
{

}

LogMonitor::~LogMonitor()
{
}

BOOL LogMonitor::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetTimer(1, 500, 0);
    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

LRESULT LogMonitor::OnTimer(WPARAM wParam, LPARAM lParam)
{
    Paint();
    return 0;
}
void LogMonitor::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_LBString(pDX, IDC_LIST1, list1);
}


BEGIN_MESSAGE_MAP(LogMonitor, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_LOG_OPEN, &LogMonitor::OnBnClickedButtonLogOpen)
    ON_BN_CLICKED(IDC_BUTTON_LOG_RESET, &LogMonitor::OnBnClickedButtonLogReset)
    ON_MESSAGE(WM_TIMER, OnTimer)
END_MESSAGE_MAP()


// LogMonitor message handlers

void LogMonitor::OnBnClickedButtonLogOpen()
{
    // TODO: Add your control notification handler code here
    CFileDialog dlg(TRUE);
    if(dlg.DoModal() == IDOK)
    {
        filename_ = dlg.GetPathName();
        OnBnClickedButtonLogReset();
    }
}

void LogMonitor::OnBnClickedButtonLogReset()
{
    // TODO: Add your control notification handler code here
//     if (t!=null && t.IsAlive)
//         t.Abort();
//     dd_list = new SortedList<string, downloaderdriver>();
//     //dd_list.Clear();
//     t = new Thread(new ThreadStart(this.ThreadRead));
//     t.Start();
//     listBox1.Items.Clear();
    boost::thread read_thread(boost::bind(&LogMonitor::ReadThread, this));
    read_thread.join();
}

void LogMonitor::ReadThread()
{
    std::ifstream fin(filename_);
    std::string log_string;
    int pos = 0;
    std::string dd_name, is_org, is_p2p, d_name, piece_index, subpiece_index;
    while(true)
    {
        if (getline(fin, log_string))
        {
            int j = 6;
            pos = fin.tellg();
            if (log_string.find("DownloadDriver::OnPieceComplete") != std::string::npos)
            {
                split(log_string, dd_name, is_org, is_p2p, d_name, piece_index, subpiece_index);
                AddDDInfo(dd_name, is_org, is_p2p, d_name, atoi(piece_index.c_str()), atoi(subpiece_index.c_str()),
                    false, false, false);
            }
            else if (log_string.find("DownloadDriver::RequestNextPiece") != std::string::npos)
            {
                split(log_string, dd_name, is_org, is_p2p, d_name, piece_index, subpiece_index);
                AddDDInfo(dd_name, is_org, is_p2p, d_name, atoi(piece_index.c_str()), atoi(subpiece_index.c_str()),
                    true, false, false);
            }
            else if (log_string.find("SubPieceRequestManager::OnSubPiece") != std::string::npos)
            {
                split(log_string, dd_name, is_org, is_p2p, d_name, piece_index, subpiece_index);
                AddDDInfo(dd_name, is_org, is_p2p, d_name, atoi(piece_index.c_str()), atoi(subpiece_index.c_str()),
                    false, true, false);
            }
            else if (log_string.find("PeerConnection::RequestSubPiece") != std::string::npos)
            {
                split(log_string, dd_name, is_org, is_p2p, d_name, piece_index, subpiece_index);
                AddDDInfo(dd_name, is_org, is_p2p, d_name, atoi(piece_index.c_str()), atoi(subpiece_index.c_str()),
                    true, true, false);
            }
            else if (log_string.find("OnRecvHttpDataSucced::OnSubPiece") != std::string::npos)
            {
                split(log_string, dd_name, is_org, is_p2p, d_name, piece_index, subpiece_index);
                AddDDInfo(dd_name, is_org, is_p2p, d_name, atoi(piece_index.c_str()), atoi(subpiece_index.c_str()),
                    true, true, true);
            }
            else if (log_string.find("HttpConnection::OnRecvHttpHeaderSucced_HEAD") != std::string::npos)
            {

            }
        }
        else
        {
            fin.seekg(pos);
            Sleep(4000);
            continue;
        }
    }
}

void LogMonitor::split(const std::string & log_string, std::string& dd_name, std::string& is_org, std::string& is_p2p,
           std::string& d_name, std::string& piece_index, std::string& subpiece_index)
{
    int index = -1;
    index = log_string.find(' ');
    if (index != -1)
    {
        index = log_string.find(' ', index + 1);
    }

    if (index != -1)
    {
        index = log_string.find(' ', index + 1);
    }

    int index2 = -1;
    if (index != -1)
    {
        index2 = log_string.find(' ', index + 1);
    }
    // dd_name
    if (index2 != -1)
    {
        dd_name = log_string.substr(index + 1, index2 - index - 1);
    }

    // is_org
    index = log_string.find(' ', index2 + 1);
    if (index != -1)
    {
        is_org = log_string.substr(index2 + 1, index - index2 - 1);
    }

    // is_p2p
    index2 = log_string.find(' ', index + 1);
    if (index != -1)
    {
        is_p2p = log_string.substr(index + 1, index2 - index - 1);
    }

    // d_name
    index = log_string.find(' ', index2 + 1);
    if (index2 != -1)
    {
        d_name = log_string.substr(index2 + 1, index - index2 - 1);
    }

    // piece_index
    index = log_string.find('(');
    index2 = log_string.find('|');
    piece_index = log_string.substr(index + 1, index2 - index - 1);

    // subpiece_index
    index = log_string.find("|p:");
    index2 = log_string.find(')');
    subpiece_index = log_string.substr(index + 3, index2 - index - 3);

}

void LogMonitor::AddPieceInfo(downloader d, int piece_index, int subpiece_index, bool is_require, bool is_subpiece,
                  bool is_http_subpiece)
{
    piece_info add_pi;
    add_pi.piece_index = piece_index;
    add_pi.subpiece_index = subpiece_index;

    if (is_subpiece)
    {
        if (piece_index > d.max_block_index)
            d.max_block_index = piece_index;

        if (is_http_subpiece)
        {
            d.http_download_subpiece[piece_index][subpiece_index]++;
        }
        else if (is_require)
        {
            d.require_subpiece[piece_index][subpiece_index]++;
        }
        else
        {
            if (d.complete_subpiece[piece_index][subpiece_index] == 0)
                d.require_no++;
            else
                d.complete_no++;
            d.complete_subpiece[piece_index][subpiece_index]++;
        }
    }
    else
    {
        if (is_require)
            d.require_list.push_back(add_pi);
        else
            d.complete_list.push_back(add_pi);
    }
}

void LogMonitor::AddDInfo(boost::shared_ptr<DownloaderDriver> dd, std::string is_org, std::string is_p2p, std::string d_name,
              int piece_index, int subpiece_index,
              bool is_require, bool is_subpiece, bool is_http_subpiece)
{
    if (is_org == "1")
    {
        if (dd->d_list.find("Orginal Http") == dd->d_list.end())
        {
            downloader add_d;
            add_d.require_list.clear();
            add_d.complete_list.clear();

            dd->d_list.insert(std::make_pair("Orginal Http", add_d));
            AddPieceInfo(add_d, piece_index, subpiece_index, is_require, is_subpiece, is_http_subpiece);
        } 
        else
        {
            AddPieceInfo(dd->d_list["Orginal Http"], piece_index, subpiece_index, is_require, is_subpiece, is_http_subpiece);
        }
    }
    else if (is_p2p == "1")
    {
        if (dd->d_list.find("P2P") == dd->d_list.end())
        {
            downloader add_d;
            add_d.require_list.clear();
            add_d.complete_list.clear();
            add_d.require_no = 0;
            add_d.complete_no = 0;
            add_d.require_subpiece = new int*[100];
            add_d.complete_subpiece = new int*[100];
            add_d.http_download_subpiece = new int*[100];

            for (int i = 0; i < 100; ++i)
            {
                add_d.require_subpiece[i] = new int[2050];
                add_d.complete_subpiece[i] = new int[2050];
                add_d.http_download_subpiece[i] = new int[2050];
            }

            dd->d_list.insert(std::make_pair("P2P", add_d));

            AddPieceInfo(add_d, piece_index, subpiece_index, is_require, is_subpiece, is_http_subpiece);
        }
        else
        {
            AddPieceInfo(dd->d_list["P2P"], piece_index, subpiece_index, is_require, is_subpiece, is_http_subpiece);
        }
    }
    else
    {
        if (dd->d_list.find(d_name) == dd->d_list.end())
        {
            downloader add_d;
            add_d.require_list.clear();
            add_d.complete_list.clear();
            dd->d_list.insert(std::make_pair(d_name, add_d));

            AddPieceInfo(add_d, piece_index, subpiece_index, is_require, is_subpiece, is_http_subpiece);
        }
        else
        {
            AddPieceInfo(dd->d_list[d_name], piece_index, subpiece_index, is_require, is_subpiece, is_http_subpiece);
        }
    }
}

void LogMonitor::AddDDInfo(std::string dd_name, std::string is_org, std::string is_p2p, std::string d_name,
               int piece_index, int subpiece_index, bool is_require, bool is_subpiece, bool is_http_subpiece)
{
    if (dd_list.find(dd_name) == dd_list.end())
    {
        boost::shared_ptr<DownloaderDriver> add_dd(new DownloaderDriver());
        add_dd->d_list.clear();
        dd_list.insert(std::make_pair(dd_name, add_dd));
        AddDInfo(add_dd, is_org, is_p2p, d_name, piece_index, subpiece_index, is_require, is_subpiece, is_http_subpiece);
    }
    else
    {
        AddDInfo(dd_list[dd_name], is_org, is_p2p, d_name, piece_index, subpiece_index, is_require, is_subpiece, is_http_subpiece);
    }
}

void LogMonitor::Paint()
{

}