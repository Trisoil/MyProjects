// livemedia_gui_testDlg.h : header file
//

#pragma once

#define  ON_UPDATEDATA  (WM_USER+1)

// Clivemedia_gui_testDlg dialog
class Clivemedia_gui_testDlg : public CDialog , public protocol::IUdpServerListener
{
// Construction
public:
	Clivemedia_gui_testDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LIVEMEDIA_GUI_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg HRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CString m_strDebugMessage;

	void addDebugMessage( const CString & m );
	afx_msg void OnBnClickedConnect();
	CString m_strAddr;

	boost::asio::io_service * io_service_;
	boost::thread * event_loop_thread_;

	protocol::UdpServer * udp_server_;
	framework::string::Uuid my_peer_id_;
	framework::string::Uuid channel_id_;

    protocol::CandidatePeerInfo my_candidate_info_;
    protocol::PEER_DOWNLOAD_INFO my_download_info_;

	virtual void OnUdpRecv(protocol::Packet const & packet);

	CString m_strStartTime;
	CString m_strUnknownTime;
	afx_msg void OnBnClickedRequestAnnounce();

    boost::uint32_t m_dwLatestTimeID;
	afx_msg void OnBnClickedRequestPiece();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	bool m_bAutoSendRequest;
	DWORD m_dwSendRequestTimeCount;
};
