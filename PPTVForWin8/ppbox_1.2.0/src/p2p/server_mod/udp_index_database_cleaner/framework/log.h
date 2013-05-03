#pragma once

namespace framework
{
    //////////////////////////////////////////////////////////////////////////
    // log name
    const string DEFAULT_LOG_NAME = "default";
    const string CONSOLE_LOG_NAME = "console";
    const string STATISTIC_LOG_NAME = "statistic";

    /////////////////////////////////////////////////////////////////////////////////////
    // Log
    class LogWriter;
    typedef boost::shared_ptr<LogWriter> LogWriter__p;

    class Log 
        : public boost::noncopyable
        , public boost::enable_shared_from_this<Log>
    {
    public:
        typedef boost::shared_ptr<Log> p;
    public:

        void Start();

        void Stop();

    public:
        static Log& Inst() { return *log_; }
        static boost::asio::io_service& IOS() { return *(log_->ios_); }

    public:

        bool IsRunning() const { return work_ != NULL; }

        bool AddLogWriter(const string& name, LogWriter__p log_writer);
        bool AddFileLogWriter(const string& name, const tstring& file_name);

        // name == 'default', use default file log name
        // name == 'console', use default console log name
        // if no such log writer, return NullLogWriter instead
        LogWriter__p GetLogWriter(const string& name);
        LogWriter__p operator[] (const string& name) { return GetLogWriter(name); }

        bool HasLogWriter(const string& name);
        void RemoveLogWriter(const string& name);
        void ClearLogWriters();

        // Utility
        LogWriter__p GetDefaultLogWriter() { return GetLogWriter(DEFAULT_LOG_NAME); }
        LogWriter__p GetConsoleLogWriter() { return GetLogWriter(CONSOLE_LOG_NAME); }
        LogWriter__p GetStatisticLogWriter() { return GetLogWriter(STATISTIC_LOG_NAME); }

        //////////////////////////////////////////////////////////////////////////
        // 兼容framework::Log
    public:
        void WriteLog(u_int level, const string& type, const string& text);
        void WriteLogT(u_int level, const string& type, const tstring& text);

        //用于控制命令的屏幕输出
        // 输出控制台文本，没有时间戳
        void WriteConsoleLine(const string& text);
        void WriteConsoleLineT(const tstring& text);
        void WriteConsole(const string& text);
        void WriteConsoleT(const tstring& text);
        // 输出控制台日志，带时间戳
        void WriteConsoleLog(u_int level, const string& type, const string& text);
        void WriteConsoleLog(const string& text);
        void WriteConsoleLogT(const tstring& text);

        // Default,Console Log
        void SetLogLevel(u_int level);
        void SetConsoleLevel(u_int level);
        void SetLogTypeOn(const string& type);
        void SetConsoleTypeOn(const string& type);
        void SetLogTypeOff(const string& type);
        void SetConsoleTypeOff(const string& type);

    private:

        Log();

    private:
        typedef map<string, LogWriter__p> LogWriterMap;
        LogWriterMap log_writers_;

        boost::asio::io_service *ios_; 
        boost::asio::io_service::work* work_;
        HANDLE thread_;

        LogWriter__p null_logger_;

        // 访问ios
        friend class LogWriter;

    private:
        static Log::p log_;

    protected:
        static DWORD WINAPI Run(LPVOID pParam);
    };

    /////////////////////////////////////////////////////////////////////////////////////
    // LogWriter
    class LogWriter
        : public boost::noncopyable
        , public boost::enable_shared_from_this<LogWriter>
    {
    public:
        typedef boost::shared_ptr<LogWriter> p;
    public:
        void Start();
        void Stop();
        bool IsRunning() const { return is_running_; }
        void ClearLog();
        void WriteLog(u_int level, const string& type, const string& text);
        void WriteLogT(u_int level, const string& type, const tstring& text);
        void WriteLine(const string& text);
        void WriteLineT(const tstring& text);
        void Write(const string& text);
        void WriteT(const tstring& text);
        void SetLogLevel(u_int level);
        void SetLogTypeOn(const string& type);
        void SetLogTypeOff(const string& type);
    protected:
        static string GetLogLevelString(u_int level);
        static tstring BuildDefaultLogName(const tstring& ext = _T(".log"));
        static tstring BuildLogName(const tstring& file_name, const tstring& ext = _T(""));
        bool CanLog(const string& type, u_int level);

        // 这些操作运行在LogThread中，继承类不用自己Post
        virtual void OnOutputLog(const string& text) = 0;
        virtual void OnStart();
        virtual void OnStop();
        virtual void OnClearLog();
    protected:
        LogWriter();
    private:
        bool is_running_;
        u_int log_level_;
        set<string> log_type_set_;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    // FileLogWriter
    class FileLogWriter
        : public LogWriter
        , public boost::enable_shared_from_this<FileLogWriter>
    {
    public:
        typedef boost::shared_ptr<FileLogWriter> p;
    public:
        static FileLogWriter::p Create(const tstring& file_name, const tstring& ext);
        static FileLogWriter::p Create(const tstring& ext = _T(".log"));
    protected:
        virtual void OnOutputLog(const string& text);
        virtual void OnStart();
        virtual void OnStop();
        virtual void OnClearLog();
    protected:
        static bool CreateDirectoryRecursive(const tstring& directory);
    private:
        FileLogWriter(const tstring& file_name, const tstring& ext);
        FileLogWriter(const tstring& ext);
    private:
        ofstream fout_;
        tstring file_name_;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    // ConsoleLogWriter
    class ConsoleLogWriter
        : public LogWriter
        , public boost::enable_shared_from_this<ConsoleLogWriter>
    {
    public:
        typedef boost::shared_ptr<ConsoleLogWriter> p;
    public:
        static ConsoleLogWriter::p Create();
    protected:
        virtual void OnOutputLog(const string& text);
    private:
        ConsoleLogWriter();
    };

    //////////////////////////////////////////////////////////////////////////
    // NullLogWriter
    class NullLogWriter
        : public LogWriter
        , public boost::enable_shared_from_this<NullLogWriter>
    {
    public:
        typedef boost::shared_ptr<NullLogWriter> p;
    public:
        static NullLogWriter::p Create();
    protected:
        virtual void OnOutputLog(const string& text);
    private:
        NullLogWriter();
    };
}


// Log Level
enum LOG_LEVEL
{
    __DEBUG = 500,
    __INFO = 1000,
    __EVENT = 2000,
    __WARN = 3000,
    __ERROR = 4000
};

#ifdef NEED_LOG
#   define LOG(level, type, message) do{\
    std::ostringstream oss; \
    oss << message; \
    framework::Log::Inst().WriteLog(level, type, oss.str()); } while(false)
#else
#   define LOG(level, type, message)
#endif


#ifdef NEED_LOG
#	ifdef	_UNICODE 
#		define LOGT(level, type, message) do{\
    std::wostringstream oss; \
    oss << message; \
    framework::Log::Inst().WriteLogT(level, type, oss.str()); } while(false)
#	else
#		define LOGT(level, type, message) do{\
    std::ostringstream oss; \
    oss << message; \
    framework::Log::Inst().WriteLogT(level, type, oss.str()); } while(false)
#	endif
#else
#	define LOGT(level, type, message)
#endif

#ifdef _DEBUG
#   define RELEASE_LOG(message) 
#else
#   define RELEASE_LOG(message) do{\
    std::ostringstream oss; \
    oss << message; \
    framework::Log::Inst().WriteLog(0, "", oss.str()); } while(false)
#endif

#ifdef _DEBUG
#   define RELEASE_OUTPUT(message)
#else
#   define RELEASE_OUTPUT(message) do{\
    std::ostringstream oss; \
    oss << message; \
    framework::Log::Inst().GetDefaultLogWriter()->WriteLine(oss.str()); } while (false)
#endif


#define CONSOLE_LOG_EX(level, type, message) do{\
    std::ostringstream oss; \
    oss << message; \
    framework::Log::Inst().WriteConsoleLog(level, type, oss.str()); } while(false)

#define CONSOLE_LOG(message) do{\
    std::ostringstream oss; \
    oss << message; \
    framework::Log::Inst().WriteConsoleLog(oss.str()); } while(false)

#define CONSOLE_OUTPUT(message) do{\
    std::ostringstream oss; \
    oss << message; \
    framework::Log::Inst().WriteConsoleLine(oss.str()); } while(false)

#define CONSOLE_TEXT(message) do { \
    std::ostringstream oss; \
    oss << message; \
    framework::Log::Inst().WriteConsole(oss.str()); } while(false)

#define STATISTIC_LOG(message) do{\
    std::ostringstream oss;\
    oss << message; \
    framework::Log::Inst().GetStatisticLogWriter()->WriteLog(0, "", oss.str()); } while (false)

#define STATISTIC_OUTPUT(message) do {\
    std::ostringstream oss;\
    oss << message; \
    framework::Log::Inst().GetStatisticLogWriter()->WriteLine(oss.str()); } while (false)

