// Detection.cpp : implementation file
//

#include "stdafx.h"
#include "P2PMonitor.h"
#include "Detection.h"
#include <fstream>
#include "boost/thread/thread.hpp"
#include "boost/bind.hpp"
#include <string>



// Detection dialog

IMPLEMENT_DYNAMIC(Detection, CDialog)

boost::mutex mu;

Detection::Detection(CWnd* pParent /*=NULL*/)
	: CDialog(Detection::IDD, pParent)
    , m_progress_pos_(0)
    , has_peer_list(false)
    , has_tracker_list(false)
    , m_switch_3000_seconds(0)
    , m_switch_2300_seconds(0)
    , m_switch_3200_seconds(0)
    , m_switch_2200_seconds(0)
    , m_switch_3000_http_total_download(0)
    , m_switch_2300_http_total_download(0)
    , m_switch_3200_p2p_total_download(0)
    , m_switch_2200_http_total_download(0)
    , m_switch_2200_p2p_total_download(0)
    , m_3200_total_connected_peers(0)
    , m_2200_total_connected_peers(0)
    , m_2300_total_connected_peers(0)
    , m_3200_max_p2p_speed(0)
    , m_2200_max_p2p_speed(0)
{
    m_p2p_monitor_dlg = new CP2PMonitorDlg();
    m_p2p_monitor_dlg->Create(IDD_P2PMONITOR_DIALOG);
    m_p2p_monitor_dlg->ShowWindow(FALSE);

//     m_simple_dlg = new CSimpleDlg();
//     m_simple_dlg->Create(IDD_SIMPLE_DIALOG);
//     m_simple_dlg->ShowWindow(FALSE);

    //m_progress_control.SetRange32(0, 100);
    //m_progress_control.SetPos(m_progress_pos_);

}

Detection::~Detection()
{
}

void Detection::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS1, m_progress_control);
}


BEGIN_MESSAGE_MAP(Detection, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_DET_START, &Detection::OnBnClickedButtonDetStart)
END_MESSAGE_MAP()


// Detection message handlers

void Detection::OnBnClickedButtonDetStart()
{
    // TODO: Add your control notification handler code here
    // 首先结束掉所有PPLive进程
    //m_p2p_monitor_dlg->OnBnClickedKillAll();

    memset(&ShExecInfo, 0, sizeof(ShExecInfo));
    ShExecInfo.cbSize =  sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = _T("cmd");
    ShExecInfo.lpDirectory = _T(" C:\ ");
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;

    m_progress_control.SetRange32(0, 159);

    std::ofstream fout("text.txt");
    fout << "开始检测，检测时间：";
    time_t t = time(0);
    char tmp[64];
    strftime(tmp, 64, "%X", localtime(&t));
    for(int i = 0; i < strlen(tmp); ++i)
    {
        fout << tmp[i];
    }
    fout << std::endl;
    fout.close();

    // 保存当前路径
    char current_dir[512];
    GetCurrentDirectory(512, current_dir);

//     m_p2p_monitor_dlg->OnBnClickedRunPpap();
//     m_p2p_monitor_dlg->OnBnClickedRunPplive();

    ::MessageBox(this->m_hWnd, "请在PPTV客户端中双击一个节目，双击完了之后点确定按钮", "Title", MB_OK);
    SetCurrentDirectory(current_dir);

    m_p2p_monitor_dlg->m_p2p_dlg->SetSaving(TRUE);
    m_p2p_monitor_dlg->m_p2p_dlg->SetStart();

    IniTrachkerList();

    boost::thread work_thread(boost::bind(&Detection::WorkFunc, this));
    boost::thread draw_thread(boost::bind(&Detection::DrawProcessControl, this));
}

void Detection::WorkFunc()
{
    CheckIP138();
    CheckIP();
    TestTinydrag();
    Ping();
    GetLocalIP();
    Test9KAPPPAC();
    Test9K();
    Test8888();
    boost::thread work_thread(boost::bind(&Detection::TestTracker, this));
    for (int i = 0; i < 5 * 60; ++i)
    {
        if (i % 3 == 0)
        {
            ++m_progress_pos_;
            if (i % 60 == 0)
            {
                Test9K();
            }
        }
        Sleep(1000);
    }
    Analyze();
    TestBs();
    Test9K();
    WriteTrackerResult();
    Sleep(1000);
    MessageBox("测试完毕!");
}

void Detection::CheckIP138()
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    fout << "1. Check ip138" << std::endl;

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query("www.ip138.com", "http");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator end;

    // Try each endpoint until we successfully establish a connection.
    boost::asio::ip::tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
      socket.close();
      socket.connect(*endpoint_iterator++, error);
    }
    if (error)
    {
        fout << "域名解析失败!" << std::endl;
    }

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << "/ip2city.asp" << " HTTP/1.0\r\n";
    request_stream << "Host: " << "www.ip138.com" << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Send the request.
    boost::asio::write(socket, request);

    // Read the response status line.
    boost::asio::streambuf response;
    boost::asio::read(socket, response, boost::asio::transfer_all(), error);

    std::istream response_stream(&response);
    std::string response_string;
    while (std::getline(response_stream, response_string))
    {
        if (response_string.find("您的IP地址是") != std::string::npos)
        {
            fout << response_string.substr(response_string.find("您"),
                response_string.find("</center>") - response_string.find("您") - 1) << std::endl;
        }
    }
    fout.close();
    ++m_progress_pos_;
}

void Detection::CheckIP()
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    fout << "\n**********************************************************************\n";
    fout << "2. Checkip" << std::endl;

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query("jump.g1d.net", "http");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator end;

    // Try each endpoint until we successfully establish a connection.
    boost::asio::ip::tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
      socket.close();
      socket.connect(*endpoint_iterator++, error);
    }
    if (error)
    {
        fout << "域名解析失败!" << std::endl;
    }

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << "/checkip.jsp" << " HTTP/1.0\r\n";
    request_stream << "Host: " << "jump.g1d.net" << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Send the request.
    boost::asio::write(socket, request);

    // Read the response status line.
    boost::asio::streambuf response;
    boost::asio::read(socket, response, boost::asio::transfer_all(), error);

    std::istream response_stream(&response);
    std::string response_string;
    while (std::getline(response_stream, response_string))
    {
        if (response_string.find("用户来源IP") != std::string::npos || response_string.find("来源IP服务的ID:") != std::string::npos)
        {
            fout << response_string.substr(0, response_string.find('<')) << std::endl;
        }
    }
    fout.close();

    ++m_progress_pos_;
}

void Detection::TestTinydrag()
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    fout << "\n**********************************************************************\n";
    fout << "3. Open Tinydrag Test Url" << std::endl;

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query("tinydrag.synacast.com", "http");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator end;

    // Try each endpoint until we successfully establish a connection.
    boost::asio::ip::tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
      socket.close();
      socket.connect(*endpoint_iterator++, error);
    }
    if (error)
    {
        fout << "域名解析失败!" << std::endl;
    }

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << "/0/B%2B%D5%EC%CC%BD%28%B8%DF%C7%E5%29_V1.mp40drag" << " HTTP/1.0\r\n";
    request_stream << "Host: " << "tinydrag.synacast.com" << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Send the request.
    boost::asio::write(socket, request);

    // Read the response status line.
    boost::asio::streambuf response;
    boost::asio::read(socket, response, boost::asio::transfer_all(), error);

    std::istream response_stream(&response);
    std::string response_string;
    bool successful = false;
    while (std::getline(response_stream, response_string))
    {
        if (response_string.find("tinydrag") != std::string::npos)
        {
            successful = true;
            break;
        }
    }

    if (successful)
    {
        fout << "成功取得tinydrag" << std::endl;
    }
    else
    {
        fout << "取不到tinydrag" << std::endl;
    }

    fout.close();

    ++m_progress_pos_;
}

void Detection::Ping()
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    fout << "\n**********************************************************************\n";
    fout << "4. Ping" << std::endl;
    fout.close();

    ShExecInfo.lpParameters = " /C ping www.pptv.com >> text.txt";
    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    ++m_progress_pos_;

    ShExecInfo.lpParameters = " /C ping tinydrag.synacast.com >> text.txt";
    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    ++m_progress_pos_;

    ShExecInfo.lpParameters = " /C ping tinydrag.pptv.com >> text.txt";
    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    fout << std::endl << std::endl;
    fout.close();

    ++m_progress_pos_;
}

void Detection::GetLocalIP()
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    fout << "\n**********************************************************************\n";
    fout << "5. Ipconfig" << std::endl;
    fout.close();

    ShExecInfo.lpParameters = " /C ipconfig >> text.txt";
    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    fout << std::endl << std::endl;
    fout.close();

    ++m_progress_pos_;
}

void Detection::Test9KAPPPAC()
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    fout << "\n**********************************************************************\n";
    fout << "6. Test 9000 Port" << std::endl;

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query("localhost", "9000");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator end;

    // Try each endpoint until we successfully establish a connection.
    boost::asio::ip::tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
      socket.close();
      socket.connect(*endpoint_iterator++, error);
    }
    if (error)
    {
        fout << "域名解析失败!" << std::endl;
        return;
    }

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);

    request_stream << "GET " << "/application.pac" << " HTTP/1.1\r\n";
    request_stream << "Host: " << "localhost:9000" << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Send the request.
    boost::asio::write(socket, request);

    // Read the response status line.
    boost::asio::streambuf response;
    boost::asio::read(socket, response, boost::asio::transfer_all(), error);
    std::istream response_stream(&response);
    std::string response_string;

    bool successful = false;
    while (std::getline(response_stream, response_string))
    {
        if (response_string.find("FindProxyForURL") != std::string::npos)
        {
            successful = true;
            break;
        }
    }

    if (successful)
    {
        fout << "peer在监听9000端口" << std::endl;
    }
    else
    {
        fout << "peer没有监听9000端口" << std::endl;
    }

    ++m_progress_pos_;
}

void Detection::Test9K()
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    fout << "\n**********************************************************************\n";
    fout << "7. Test 9000 Port" << std::endl;
    fout.close();

    ShExecInfo.lpParameters = " /C netstat -ano|findstr 9000 >> text.txt";
    ShExecInfo.nShow = SW_HIDE;
    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    std::ifstream fin("text.txt");
    std::string str;
    std::set<CString> commands;
    while (getline(fin, str))
    {
        if (str.find("9000") != std::string::npos)
        {
            int index = str.length() - 1;
            while(index >=0 && str[index] != ' ')
                --index;
            if (index > 0)
            {
                CString pid = (CString)(str.substr(index + 1)).c_str();
                if (pid[0] == '0')
                {
                    continue;
                }
                CString get_process_name_by_pid;
                get_process_name_by_pid.Format("%s%s%s", " /C tasklist|findstr ", pid, " >> text.txt");

                if (commands.find(get_process_name_by_pid) == commands.end())
                {
                    commands.insert(get_process_name_by_pid);
                }
            }
        }
    }

    for (std::set<CString>::iterator iter = commands.begin(); iter != commands.end(); ++iter)
    {
        ShExecInfo.lpParameters = (LPCTSTR)(*iter);
        ShellExecuteEx(&ShExecInfo);
        WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
    }

    ++m_progress_pos_;
}

void Detection::Test8888()
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    fout << "\n**********************************************************************\n";
    fout << "8. Test 8888 Port" << std::endl;
    fout.close();

    ShExecInfo.lpParameters = " /C netstat -ano|findstr 8888 >> text.txt";
    ShExecInfo.nShow = SW_HIDE;
    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    std::ifstream fin("text.txt");
    std::string str;
    std::set<CString> commands;
    while (getline(fin, str))
    {
        if (str.find("8888") != std::string::npos)
        {
            int index = str.length() - 1;
            while(index >=0 && str[index] != ' ')
                --index;
            if (index > 0)
            {
                CString pid = (CString)(str.substr(index + 1)).c_str();
                if (pid[0] == '0')
                {
                    continue;
                }
                CString get_process_name_by_pid;
                get_process_name_by_pid.Format("%s%s%s", " /C tasklist|findstr ", pid, " >> text.txt");
                if (commands.find(get_process_name_by_pid) == commands.end())
                {
                    commands.insert(get_process_name_by_pid);
                }
            }
        }
    }

    for (std::set<CString>::iterator iter = commands.begin(); iter != commands.end(); ++iter)
    {
        ShExecInfo.lpParameters = (LPCTSTR)(*iter);
        ShellExecuteEx(&ShExecInfo);
        WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
    }

    ++m_progress_pos_;
}

void Detection::DrawProcessControl()
{
    int progress_pos = m_progress_pos_;
    while(true)
    {
        if (progress_pos != m_progress_pos_)
        {
            m_progress_control.SetPos(m_progress_pos_);
            progress_pos = m_progress_pos_;
        }
        if (m_progress_pos_ >= 159)
        {
            break;
        }
        else
        {
            Sleep(1000);
        }
    }
}

void Detection::Analyze()
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    fout << "\n**********************************************************************\n";
    fout << "9. 分析下载速度" << std::endl;

    CStdioFile f;
    if (f.Open(m_p2p_monitor_dlg->m_p2p_dlg->getCSVFilePath(), CFile::modeRead))
    {
        CString line;
        while (f.ReadString(line))
        {
            ReadFile(line);
        }
        f.Close();
    }

    bool pinged_3000_cdn_ip = false;

    for (int i = 0; i < m_datas.size(); ++i)
    {
        if (m_datas[i].m_switch_state == "3000")
        {
            ++m_switch_3000_seconds;
            m_switch_3000_http_total_download += m_datas[i].m_http_kbps;
            m_3000_cdn_ips.insert(m_datas[i].m_strIP);
        }
        else if (m_datas[i].m_switch_state == "2300")
        {
            ++m_switch_2300_seconds;
            m_switch_2300_http_total_download += m_datas[i].m_http_kbps;
            m_2300_cdn_ips.insert(m_datas[i].m_strIP);
        }
        else if (m_datas[i].m_switch_state == "3200")
        {
            ++m_switch_3200_seconds;
            m_switch_3200_p2p_total_download += m_datas[i].m_p2p_kbps;
            m_3200_total_connected_peers += m_datas[i].m_connect_peer;
            if (m_3200_max_p2p_speed < m_datas[i].m_p2p_kbps)
            {
                m_3200_max_p2p_speed = m_datas[i].m_p2p_kbps;
            }
        }
        else if (m_datas[i].m_switch_state == "2200")
        {
            ++m_switch_2200_seconds;
            m_switch_2200_http_total_download += m_datas[i].m_http_kbps;
            m_switch_2200_p2p_total_download += m_datas[i].m_p2p_kbps;
            m_2200_cdn_ips.insert(m_datas[i].m_strIP);
            m_2200_total_connected_peers += m_datas[i].m_connect_peer;
            if (m_2200_max_p2p_speed < m_datas[i].m_p2p_kbps)
            {
                m_2200_max_p2p_speed = m_datas[i].m_p2p_kbps;
            }
        }
    }

    fout << "状态机为3000的总时间为: " << m_switch_3000_seconds << "秒\n";
    if (m_switch_3000_seconds > 0)
    {
        fout << "状态机为3000时http的平均速度为: " << m_switch_3000_http_total_download / m_switch_3000_seconds
        << "KB/s\n";
    }

    fout << "状态机为2300的总时间为: " << m_switch_2300_seconds << "秒\n";
    if (m_switch_2300_seconds > 0)
    {
        fout << "状态机为2300时http的平均速度为: " << m_switch_2300_http_total_download / m_switch_2300_seconds
            << "KB/s\n";
        fout << "状态机为2300时p2p平均节点数为: " << m_2300_total_connected_peers / m_switch_2300_seconds << std::endl;
    }

    fout << "状态机为3200的总时间为: " << m_switch_3200_seconds << "秒\n";
    if (m_switch_3200_seconds > 0)
    {
        fout << "状态机为3200时p2p的平均速度为: " << m_switch_3200_p2p_total_download / m_switch_3200_seconds
            << "KB/s\n";
        fout << "状态机为3200时p2p的最大速度为: " << m_3200_max_p2p_speed << std::endl;
        fout << "状态机为3200时p2p平均节点数为: " << m_3200_total_connected_peers / m_switch_3200_seconds << std::endl;
    }

    fout << "状态机为2200的总时间为: " << m_switch_2200_seconds << "秒\n";
    if (m_switch_2200_seconds > 0)
    {
        fout << "状态机为2200时p2p的平均速度为: " << m_switch_2200_p2p_total_download / m_switch_2200_seconds
            << "KB/s\n";
        fout << "状态机为2200时p2p的最大速度为: " << m_2200_max_p2p_speed << std::endl;
        fout << "状态机为2200时p2p平均节点数为: " << m_2200_total_connected_peers / m_switch_2200_seconds << std::endl;
        fout << "状态机为2200时http的平均速度为: " << m_switch_2200_http_total_download / m_switch_2200_seconds;
    }
    fout.close();

    if (m_switch_3000_seconds > 10 && m_switch_3000_http_total_download / m_switch_3000_seconds < 100)
    {
        PingSomeIP(0, m_3000_cdn_ips);
    }
    else
    {
        m_progress_pos_ += 5;
    }

    if (m_switch_2300_seconds > 10 && m_switch_2300_http_total_download / m_switch_2300_seconds < 100)
    {
        PingSomeIP(1, m_2300_cdn_ips);
    }
    else
    {
        m_progress_pos_ += 5;
    }

    if (m_switch_2200_seconds > 10 && m_switch_2200_http_total_download / m_switch_2200_seconds < 50)
    {
        PingSomeIP(2, m_2200_cdn_ips);
    }
    else
    {
        m_progress_pos_ += 5;
    }
}

void Detection::ReadFile(const CString& line)
{
    ConcernData con_data;
    int index1 = 0, index2;
    index2 = line.Find(',');
    con_data.m_filename = line.Mid(index1, index2 - index1);//文件名

    if (con_data.m_filename== "文件名")
        return;

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_filelen = atoi(line.Mid(index1, index2 - index1));//文件长度

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_datarate = atoi(line.Mid(index1, index2 - index1));//码流

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_strIP = line.Mid(index1, index2 - index1);//CDN IP

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    //当前时间  跳过

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_Send = atoi(line.Mid(index1, index2 - index1));//总发送

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_Receive = atoi(line.Mid(index1, index2 - index1));//总收到

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_lost_rate = line.Mid(index1, index2 - index1);//丢包率

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_re_rate = line.Mid(index1, index2 - index1);//冗余率	

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_total_http = atoi(line.Mid(index1, index2 - index1));//http总下载

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_total_p2p = atoi(line.Mid(index1, index2 - index1));//p2p总下载

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_rest_time = atoi(line.Mid(index1, index2 - index1));//剩余时间

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_http_kbps = atoi(line.Mid(index1, index2 - index1));//http速度

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_p2p_kbps = atoi(line.Mid(index1, index2 - index1));//p2p速度

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    //总下载速度  跳过

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_switch_state = line.Mid(index1, index2 - index1);//状态机

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_peer_connect_count = line.Mid(index1, index2 - index1);//连接数

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_full_resource_peer = atoi(line.Mid(index1, index2 - index1));  // 满资源peer数

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_connect_peer = atoi(line.Mid(index1, index2 - index1));  // 连接peer数

    index1 = index2 + 1;
    index2 = line.Find(',', index1);
    con_data.m_queryed_peer = atoi(line.Mid(index1, index2 - index1));  // 查询peer数

    m_datas.push_back(con_data);
}

void Detection::TestBs()
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    fout << "\n**********************************************************************\n";
    fout << "10. TestBS" << std::endl;

    std::string bss[3];
    bss[0] = "ppvabs.pplive.com";
    bss[1] = "ppvaindex.pplive.com";
    bss[2] = "60.28.216.149";
    for (int i = 0; i < 3; ++i)
    {
        boost::asio::io_service io_service;

        boost::asio::ip::udp::resolver resolver(io_service);
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), bss[i], "");
        boost::asio::ip::udp::endpoint receiver_endpoint = *resolver.resolve(query);
        receiver_endpoint.port(6400);
        boost::asio::ip::udp::socket socket(io_service);
        socket.open(boost::asio::ip::udp::v4());

        std::string send_buffer = "";
        boost::uint32_t check_sum = 0x80301a7d;
        boost::uint8_t action = 0x14;
        boost::uint32_t transaction_id = 0x00000006;
        boost::uint8_t is_request = 1;
        boost::uint16_t peer_version = 0x0106;
        boost::uint16_t reserved = 0x0000;
        boost::uint32_t guid_1 = 0x36e985d8;
        boost::uint32_t guid_2 = 0x4e4e041d;
        boost::uint32_t guid_3 = 0x8a7db9b7;
        boost::uint32_t guid_4 = 0x911a6af6;
        send_buffer.append((const char *)&check_sum, 4);
        send_buffer.append((const char *)&action, 1);
        send_buffer.append((const char *)&transaction_id, 4);
        send_buffer.append((const char *)&is_request, 1);
        send_buffer.append((const char *)&peer_version, 2);
        send_buffer.append((const char *)&reserved, 2);
        send_buffer.append((const char *)&guid_1, 4);
        send_buffer.append((const char *)&guid_2, 4);
        send_buffer.append((const char *)&guid_3, 4);
        send_buffer.append((const char *)&guid_4, 4);

        socket.send_to(boost::asio::buffer(send_buffer), receiver_endpoint);
        boost::asio::ip::udp::endpoint sender_endpoint;
        boost::asio::deadline_timer timer(io_service);

        socket.async_receive_from(
            boost::asio::buffer(data_, 2000), sender_endpoint,
            boost::bind(&Detection::HandleReceiveFrom, this, 0, 0,
            boost::ref(timer),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

        timer.expires_from_now(boost::posix_time::seconds(5));

        timer.async_wait(boost::bind(&Detection::TimeOutHandler, this, 0, boost::asio::placeholders::error, boost::ref(socket), 0));

        io_service.run();

        if (has_tracker_list)
        {
            m_progress_pos_ = 135;
            break;
        }
    }
    fout.close();
}

void Detection::TestTracker()
{
//     std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
//     fout << "\n**********************************************************************\n";
//     fout << "11. TestTracker" << std::endl;
//     fout.close();

    std::string send_buffer = "";
    boost::uint32_t check_sum = 0x66e3d105;
    boost::uint8_t action = 0x31;
    boost::uint32_t transaction_id = 0x00054afb;
    boost::uint8_t is_request = 1;
    boost::uint16_t peer_version = 0x0106;
    boost::uint16_t reserved = 0x0000;
    boost::uint32_t rid_1 = 0x4c58186d;
    boost::uint32_t rid_2 = 0x45cab895;
    boost::uint32_t rid_3 = 0x47d10c08;
    boost::uint32_t rid_4 = 0xf4354547;
    boost::uint32_t guid_1 = 0xa111fe6e;
    boost::uint32_t guid_2 = 0x4e1472c3;
    boost::uint32_t guid_3 = 0x9298e98b;
    boost::uint32_t guid_4 = 0x825f3f99;
    boost::uint16_t peer_count = 0x0032;
    send_buffer.append((const char *)&check_sum, 4);
    send_buffer.append((const char *)&action, 1);
    send_buffer.append((const char *)&transaction_id, 4);
    send_buffer.append((const char *)&is_request, 1);
    send_buffer.append((const char *)&peer_version, 2);
    send_buffer.append((const char *)&reserved, 2);
    send_buffer.append((const char *)&rid_1, 4);
    send_buffer.append((const char *)&rid_2, 4);
    send_buffer.append((const char *)&rid_3, 4);
    send_buffer.append((const char *)&rid_4, 4);
    send_buffer.append((const char *)&guid_1, 4);
    send_buffer.append((const char *)&guid_2, 4);
    send_buffer.append((const char *)&guid_3, 4);
    send_buffer.append((const char *)&guid_4, 4);
    send_buffer.append((const char *)&peer_count, 2);

    for (int i = 0; i < m_tracker_infos.size(); ++i)
    {
        boost::asio::io_service io_service;

        boost::asio::ip::udp::resolver resolver(io_service);
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), m_tracker_infos[i].ip, "");
        boost::asio::ip::udp::endpoint receiver_endpoint = *resolver.resolve(query);
        receiver_endpoint.port(m_tracker_infos[i].port);
        boost::asio::ip::udp::socket socket(io_service);
        socket.open(boost::asio::ip::udp::v4());

        socket.send_to(boost::asio::buffer(send_buffer), receiver_endpoint);
        boost::asio::ip::udp::endpoint sender_endpoint;
        boost::asio::deadline_timer timer(io_service);

        socket.async_receive_from(
            boost::asio::buffer(data_, 2000), sender_endpoint,
            boost::bind(&Detection::HandleReceiveFrom, this, 1, i,
            boost::ref(timer),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

        timer.expires_from_now(boost::posix_time::seconds(5));

        timer.async_wait(boost::bind(&Detection::TimeOutHandler, this, 1,
            boost::asio::placeholders::error, boost::ref(socket), i));

        io_service.run();
    }
}

void Detection::HandleReceiveFrom(int packet_type, int tracker_index, boost::asio::deadline_timer& timer,
                                  const boost::system::error_code& error, size_t bytes_recvd)
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    if (!error || error == boost::asio::error::message_size)
    {
        if (packet_type == 0)  // bs
        {
            int tracker_count = data_[14] * 256 + data_[13];
            fout << "向bs查询到" << tracker_count << "个tracker" << std::endl;
            fout.close();
            has_tracker_list = true;
            timer.cancel();
        }
        else if (packet_type == 1)  // tracker
        {
            int peer_count = data_[28] * 256 + data_[27];
            //mu.lock();
            std::string result = "";
            result.append("向tracker(");
            result.append(m_tracker_infos[tracker_index].ip);
            result.append(":");
            boost::uint16_t port = m_tracker_infos[tracker_index].port;
            char tmp[10];
            itoa(m_tracker_infos[tracker_index].port, tmp, 10);
            result.append(tmp);
            result.append( ")查询到");
            itoa(peer_count, tmp, 10);
            result.append(tmp);
            result.append("个peer");
//             result += "向tracker(" + m_tracker_infos[tracker_index].ip + ":" + m_tracker_infos[tracker_index].port
//                 + ")查询到" + peer_count + "个peer";
            result_trackers.push_back(result);
//             fout << "向tracker(" << m_tracker_infos[tracker_index].ip << ":" << m_tracker_infos[tracker_index].port
//                 << ")查询到" << peer_count << "个peer" << std::endl;
//             fout.close();
//             mu.unlock();
            timer.cancel();
            has_peer_list = true;
        }
    }
    else if (error != boost::asio::error::operation_aborted)
    {
        if (packet_type == 0)
        {
            fout << error.message() << std::endl;
            fout.close();
        }
        else if (packet_type == 1)
        {
            result_trackers.push_back(error.message());
        }
        
    }
}

void Detection::TimeOutHandler(int packet_type, const boost::system::error_code& error, boost::asio::ip::udp::socket& socket, int tracker_index)
{
    if(!error)//超时
    {
        std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
        if (packet_type == 0 && !has_tracker_list)  // bs
        {
            fout << "bs回包超时" << std::endl;
            fout.close();
            m_progress_pos_ += 3;
        }
        else if (packet_type == 1)  // tracker
        {
            std::string result = "";
            result.append("tracker(");
            result.append(m_tracker_infos[tracker_index].ip);
            result.append(":");
            boost::uint16_t port = m_tracker_infos[tracker_index].port;
            char tmp[10];
            itoa(m_tracker_infos[tracker_index].port, tmp, 10);
            result.append(tmp);
            result.append( ")回包超时");
            result_trackers.push_back(result);
            //fout.close();
            m_progress_pos_ += 1;
        }
        socket.close();
    }
}

void Detection::PingSomeIP(int switch_type, std::set<CString> test_ips)
{
    if (switch_type == 0)  //3000
    {
        std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
        fout << "\t\tPing 3000 cdn ip\n";
        fout.close();
    }
    else if (switch_type == 1)  // 2300
    {
        std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
        fout << "\t\tPing 2300 cdn ip\n";
        fout.close();
    }
    else if (switch_type == 2)  // 2200
    {
        std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
        fout << "\t\tPing 2200 cdn ip\n";
        fout.close();
    }
    for (std::set<CString>::iterator iter = test_ips.begin(); iter != test_ips.end(); ++iter)
    {
        CString ping_command;
        ping_command.Format("%s%s%s", " /C ping ", *iter, " >> text.txt");
        ShExecInfo.lpParameters = (LPCTSTR)ping_command;
        ShellExecuteEx(&ShExecInfo);
        WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
        if (switch_type == 0)  // 3000状态需要telnet
        {
            CString telnet_command;
            telnet_command.Format("%s%s%s", " /C telnet ", *iter, " 80 >> text.txt");
            ShExecInfo.lpParameters = (LPCTSTR)telnet_command;
            ShellExecuteEx(&ShExecInfo);
            WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
        }
    }

    m_progress_pos_ += 5;
}

void Detection::IniTrachkerList()
{
    TrackerInfo ti;
    ti.ip = "122.225.35.103"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "61.155.162.36"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "122.225.35.103"; ti.port = 18008; m_tracker_infos.push_back(ti);
    ti.ip = "61.155.164.53"; ti.port = 18004; m_tracker_infos.push_back(ti);
    ti.ip = "61.155.164.54"; ti.port = 18004; m_tracker_infos.push_back(ti);
    ti.ip = "61.155.164.54"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "188.123.212.28"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "118.123.212.26"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "118.123.201.106"; ti.port = 18008; m_tracker_infos.push_back(ti);
    ti.ip = "118.123.212.12"; ti.port = 18003; m_tracker_infos.push_back(ti);
    ti.ip = "118.123.212.13"; ti.port = 18002; m_tracker_infos.push_back(ti);
    ti.ip = "118.123.201.106"; ti.port = 18004; m_tracker_infos.push_back(ti);
    ti.ip = "125.46.66.147"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "125.46.66.92"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "119.167.214.52"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "119.167.214.53"; ti.port = 18004; m_tracker_infos.push_back(ti);
    ti.ip = "125.46.66.132"; ti.port = 18008; m_tracker_infos.push_back(ti);
    ti.ip = "125.46.66.96"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "221.192.146.142"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "119.167.214.53"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "125.46.66.131"; ti.port = 18004; m_tracker_infos.push_back(ti);
    ti.ip = "132.126.34.230"; ti.port = 18000; m_tracker_infos.push_back(ti);
    ti.ip = "119.167.214.52"; ti.port = 18004; m_tracker_infos.push_back(ti);
}

void Detection::WriteTrackerResult()
{
    std::ofstream fout("text.txt", std::ofstream::ate|std::ofstream::app);
    fout << "\n**********************************************************************\n";
    fout << "11. TestTracker" << std::endl;

    for (int i = 0; i < result_trackers.size(); ++i)
    {
        fout << result_trackers[i] << std::endl;
    }

    fout.close();
}