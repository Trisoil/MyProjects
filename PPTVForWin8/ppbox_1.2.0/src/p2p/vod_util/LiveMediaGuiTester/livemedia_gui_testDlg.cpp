// livemedia_gui_testDlg.cpp : implementation file
//

#include "stdafx.h"
#include "livemedia_gui_test.h"
#include "livemedia_gui_testDlg.h"
#include "struct/LiveSubpieceInfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define REMOTE_PORT 80

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Clivemedia_gui_testDlg dialog




Clivemedia_gui_testDlg::Clivemedia_gui_testDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Clivemedia_gui_testDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	io_service_ = 0;
	event_loop_thread_ = 0;
	udp_server_ = 0;

	this->my_peer_id_.generate();

	// 
	my_candidate_info_.IP = 0;
	my_candidate_info_.UdpPort= 6666;
	my_candidate_info_.DetectIP = 0;
	my_candidate_info_.DetectUdpPort = 0;
	my_candidate_info_.StunIP = 0;
	my_candidate_info_.StunUdpPort = 0;

	this->my_download_info_.AvgDownload = 0;
	this->my_download_info_.NowDownload = 0;
	this->my_download_info_.AvgUpload = 0;
	this->my_download_info_.NowUpload = 0;
	this->my_download_info_.IsDownloading = 0;
	this->my_download_info_.OnlineTime = 0;

	m_dwLatestTimeID = 0;

	m_dwSendRequestTimeCount = 0;
	m_bAutoSendRequest = false;

	this->channel_id_.from_string( _T("e9301e073cf94732a380b765c8b9573d") );
}

void Clivemedia_gui_testDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DEBUG_MESSAGE, m_strDebugMessage);
	DDX_Text(pDX, IDC_ADDR, m_strAddr);
	DDX_Text(pDX, IDC_START_TIME, m_strStartTime);
	DDX_Text(pDX, IDC_UNKNOWN_TIME, m_strUnknownTime);
}

BEGIN_MESSAGE_MAP(Clivemedia_gui_testDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_MESSAGE(ON_UPDATEDATA, OnUpdateData)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CONNECT, &Clivemedia_gui_testDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_REQUEST_ANNOUNCE, &Clivemedia_gui_testDlg::OnBnClickedRequestAnnounce)
	ON_BN_CLICKED(IDC_REQUEST_PIECE, &Clivemedia_gui_testDlg::OnBnClickedRequestPiece)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// Clivemedia_gui_testDlg message handlers

BOOL Clivemedia_gui_testDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	this->m_strAddr = _T("192.168.30.181");
	this->addDebugMessage( _T("这里显示调试信息:") );

	// 这是测试程序，所有 new 出来的都不用去担心释放了

	// 初始化 BOOST
	this->io_service_ = new boost::asio::io_service();
	this->udp_server_ = new protocol::UdpServer( *this->io_service_ , this );
	srand(::GetTickCount() );
	this->udp_server_->Listen( rand() % 10000 );

    protocol::register_live_peer_packet( *this->udp_server_ );
    protocol::register_peer_packet( *this->udp_server_);
 
	this->udp_server_->Recv( 100 );

	// 启动后台的推线程
	this->event_loop_thread_ = new boost::thread( boost::bind( &boost::asio::io_service::run , this->io_service_ ) );
	if ( 0 == this->event_loop_thread_ ) {
		return FALSE;
	}

	this->m_strStartTime = "0";
	this->m_strUnknownTime = "1";

	this->UpdateData(0);

	this->SetTimer( 0 , 1000  , 0 );
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Clivemedia_gui_testDlg::addDebugMessage( const CString & m )
{
	this->m_strDebugMessage += m;

	this->UpdateData( false );
}

HRESULT Clivemedia_gui_testDlg::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
    CString csOut = (TCHAR*)wParam;
    this->addDebugMessage( csOut );
    return S_OK;
}

void Clivemedia_gui_testDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Clivemedia_gui_testDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Clivemedia_gui_testDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

static int g_tid = 0;

void Clivemedia_gui_testDlg::OnBnClickedConnect()
{
	// TODO: Add your control notification handler code here
	
	// 
	// 发送连接包
	// 

	this->UpdateData( true );

    boost::shared_ptr<protocol::ConnectPacket> packet;
    boost::asio::ip::address addr;
    addr = addr.from_string( (const char *)this->m_strAddr );
    boost::asio::ip::udp::endpoint ep( addr , REMOTE_PORT);

    Guid guid;
    guid.generate();
    packet.reset(new protocol::ConnectPacket(++g_tid,
        this->channel_id_, guid, 0x108,
        0, time(0), 0x108,
        my_candidate_info_, protocol::CONNECT_LIVE_PEER, my_download_info_, ep, 1));

   	this->udp_server_->send_packet( *packet, 0x108);
}

void Clivemedia_gui_testDlg::OnUdpRecv(protocol::Packet const & packet)
{
	std::stringstream oss;
	static int cou = 0;
	oss << cou++ << "Clivemedia_gui_testDlg::OnUdpRecv\n";
	TRACE(oss.str().c_str());

	switch( packet.PacketAction ) {
        case ::protocol::ConnectPacket::Action:
			// 连接包
			{
                const ::protocol::ConnectPacket & c_packet = (const ::protocol::ConnectPacket &)packet;
                SendMessage(ON_UPDATEDATA, (WPARAM)_T("ConnectPacket\r\n"));
			}

			break;
        case ::protocol::LiveRequestAnnouncePacket::Action:
			{
                const ::protocol::LiveRequestAnnouncePacket & c_packet = (const ::protocol::LiveRequestAnnouncePacket &)packet;
                SendMessage(ON_UPDATEDATA, (WPARAM)_T("RequestAnnouncePacket\r\n"));
;			}

			break;

        case ::protocol::LiveAnnouncePacket::Action:
			{
                const ::protocol::LiveAnnouncePacket & c_packet = (const ::protocol::LiveAnnouncePacket&)packet;
                std::map<boost::uint32_t, boost::uint16_t>::const_iterator iter = c_packet.live_announce_map_.subpiece_nos_.begin();
                if (iter != c_packet.live_announce_map_.subpiece_nos_.end())
                {
                     //this->m_dwLatestTimeID = iter->first + iter->second - 1;
                    this->m_dwLatestTimeID = iter->first;
                }
               
                SendMessage(ON_UPDATEDATA, (WPARAM)_T("LiveAnnouncePacket\r\n"));
			}
			break;

        case ::protocol::LiveSubPiecePacket::Action:
			{
                const ::protocol::LiveSubPiecePacket & c_packet = (const ::protocol::LiveSubPiecePacket &)packet;
                SendMessage(ON_UPDATEDATA, (WPARAM)_T("LiveSubpiecePacket\r\n"));
			}
			break;
	}

}

void Clivemedia_gui_testDlg::OnBnClickedRequestAnnounce()
{
	// TODO: Add your control notification handler code here
    boost::shared_ptr<protocol::LiveRequestAnnouncePacket> packet;

	boost::asio::ip::address addr;
	addr = addr.from_string( (const char *)this->m_strAddr );
	boost::asio::ip::udp::endpoint ep( addr , REMOTE_PORT);


    int start_time = ::_atoi64( this->m_strStartTime );
    packet.reset(new protocol::LiveRequestAnnouncePacket( ++g_tid, this->channel_id_, start_time, 1024, ep));
    packet->reserve_ = 0;
    packet->protocol_version_ = 0x108;
    this->UpdateData();
	this->udp_server_->send_packet( *packet, 0x108 );
}

void Clivemedia_gui_testDlg::OnBnClickedRequestPiece()
{
	// TODO: Add your control notification handler code here

    boost::shared_ptr<protocol::LiveRequestSubPiecePacket> packet;

	boost::asio::ip::address addr;
	addr = addr.from_string( (const char *)this->m_strAddr );
	boost::asio::ip::udp::endpoint ep( addr , REMOTE_PORT);
     
    protocol::LiveSubPieceInfo s(this->m_dwLatestTimeID, 0);
    std::vector<protocol::LiveSubPieceInfo> subpieceinfos;
    subpieceinfos.push_back(s);
    
    packet.reset(new protocol::LiveRequestSubPiecePacket(++g_tid, this->channel_id_, 
        subpieceinfos, 30, ep));
    packet->protocol_version_ = 0x108;

	this->udp_server_->send_packet( *packet, 0x108);

	//m_bAutoSendRequest = true;
    m_bAutoSendRequest = false;
}

void Clivemedia_gui_testDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	__super::OnTimer(nIDEvent);

	if( this->m_bAutoSendRequest ) {
        

		boost::asio::ip::address addr;
		addr = addr.from_string( (const char *)this->m_strAddr );
		boost::asio::ip::udp::endpoint ep( addr , REMOTE_PORT);

        std::vector<protocol::LiveSubPieceInfo> subpieceinfos;
		static int count = 0;
		for( size_t i  = 0; i < 100; i++ ) 
        {
            ::protocol::LiveSubPieceInfo s(this->m_dwLatestTimeID + m_dwSendRequestTimeCount, 0 + i);
			subpieceinfos.push_back( s );
		}
        
        boost::shared_ptr<protocol::LiveRequestSubPiecePacket> packet(
            new protocol::LiveRequestSubPiecePacket(++g_tid, this->channel_id_, subpieceinfos, 30, ep));
        packet->protocol_version_ = 0x108;
		m_dwSendRequestTimeCount++;
		this->udp_server_->send_packet( *packet, 0x108);
	}
}
