#ifndef LOG_MONITOR
#define LOG_MONITOR


struct piece_info
{
    int piece_index;
    int subpiece_index;
};

struct downloader
{
    std::list<piece_info> require_list;
    std::list<piece_info> complete_list;
    int max_block_index, require_no, complete_no;
//         int require_subpiece[100][2050];
//         int complete_subpiece[100][2050];
//         int http_download_subpiece[100][2050];
    int** require_subpiece;
    int** complete_subpiece;
    int** http_download_subpiece;
};

class DownloaderDriver
{
public:
    std::map<std::string, downloader> d_list;
    int head_length;
};

// LogMonitor dialog

class LogMonitor : public CDialog
{
    DECLARE_DYNAMIC(LogMonitor)

public:
    LogMonitor(CWnd* pParent = NULL);   // standard constructor
    virtual ~LogMonitor();

// Dialog Data
    enum { IDD = IDD_DIALOG_LOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    CString list1;
    afx_msg void OnBnClickedButtonLogOpen();
    virtual BOOL OnInitDialog();
    LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
private:
    void ReadThread();
    void split(const std::string & log_string, std::string& dd_name, std::string& is_org, std::string& is_p2p,
        std::string& d_name, std::string& piece_index, std::string& subpiece_index);
    void AddPieceInfo(downloader d, int piece_index, int subpiece_index, bool is_require, bool is_subpiece,
        bool is_http_subpiece);
    void AddDInfo(boost::shared_ptr<DownloaderDriver> dd, std::string is_org, std::string is_p2p, std::string d_name,
        int piece_index, int subpiece_index,
        bool is_require, bool is_subpiece, bool is_http_subpiece);
    void AddDDInfo(std::string dd_name, std::string is_org, std::string is_p2p, std::string d_name,
        int piece_index, int subpiece_index, bool is_require, bool is_subpiece, bool is_http_subpiece);
    void Paint();
private:
    CString filename_;
    std::map<std::string, boost::shared_ptr<DownloaderDriver> > dd_list;
public:
    afx_msg void OnBnClickedButtonLogReset();
};

#endif