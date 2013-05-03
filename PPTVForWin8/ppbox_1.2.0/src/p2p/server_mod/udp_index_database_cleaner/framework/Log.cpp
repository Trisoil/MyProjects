#include "stdafx.h"
#include "framework/Log.h"
#include "framework/io/Path.h"
#include "framework/mswin/module.h"
#include <boost/filesystem/operations.hpp>

namespace framework
{
    //////////////////////////////////////////////////////////////////////////
    // LogWriter

    LogWriter::LogWriter()
        : log_level_(__ERROR)
    {
        is_running_ = false;
        log_type_set_.insert("");
    }

    // public
    void LogWriter::Start()
    {
        if (true == is_running_)
            return ;
        // post
        Log::IOS().post(
            boost::bind(&LogWriter::OnStart, shared_from_this())
            );
        is_running_ = true;
    }

    void LogWriter::Stop()
    {
        if (false == is_running_)
            return ;
        // post
        Log::IOS().post(
            boost::bind(&LogWriter::OnStop, shared_from_this())
            );
        is_running_ = false;
    }

    void LogWriter::SetLogLevel(u_int level)
    {
        log_level_ = level;
    }

    void LogWriter::SetLogTypeOn(const string& type)
    {
        log_type_set_.insert(type);
    }

    void LogWriter::SetLogTypeOff(const string& type)
    {
        log_type_set_.erase(type);
    }

    void LogWriter::WriteLogT(u_int level, const string& type, const tstring& text)
    {
#ifdef UNICODE
        WriteLog(level, type, framework::w2b(text));
#else
        WriteLog(level, type, text);
#endif
    }

    void LogWriter::WriteLog(u_int level, const string& type, const string& text)
    {
        const size_t MAX_LOG_HEADER_SIZE = 1024;
        static char log_header[MAX_LOG_HEADER_SIZE + 1];

        if (IsRunning() == false)
        {
            return;
        }

        if (CanLog(type, level) == true)
        {
            // current time
            SYSTEMTIME time;
            ::GetLocalTime(&time);

            // log level
            string level_string = GetLogLevelString(level);

            // log header
            int outputCount = _snprintf(log_header, MAX_LOG_HEADER_SIZE, 
                "%04d-%02d-%02d %02d:%02d:%02d.%03d %5s %s ",
                time.wYear,time.wMonth,time.wDay,
                time.wHour,time.wMinute,time.wSecond, time.wMilliseconds,
                level_string.c_str(), type.c_str()
                );

            // assert
            assert(outputCount < MAX_LOG_HEADER_SIZE);

            // post
            string msg = string(log_header) + text + "\n";
            Log::IOS().post(
                boost::bind(&LogWriter::OnOutputLog, shared_from_this(), msg)
                );
        }
    }

    void LogWriter::WriteLine(const string& text)
    {
        if (false == is_running_)
            return ;

        Write(text + "\n");
    }

    void LogWriter::WriteLineT(const tstring& text)
    {
        if (false == is_running_)
            return ;
        WriteT(text + _T("\n"));
    }

    void LogWriter::Write(const string& text)
    {
        if (false == is_running_)
            return ;
        Log::IOS().post( boost::bind(&LogWriter::OnOutputLog, shared_from_this(), text) );
    }

    void LogWriter::WriteT(const tstring& text)
    {
        if (false == is_running_)
            return ;
#ifdef UNICODE
        Write(framework::w2b(text));
#else
        Write(text);
#endif
    }

    void LogWriter::ClearLog()
    {
        Log::IOS().post(
            boost::bind(&LogWriter::OnClearLog, shared_from_this())
            );
    }

    // protected

    tstring LogWriter::BuildDefaultLogName(const tstring& ext) // default '.log'
    {
        framework::io::Path logFilePath;
        framework::mswin::Module module;
        module.AttachAddress(&Log::Run);

        tstring moduleName = module.GetFileName();
        if (moduleName.empty())
        {
            moduleName = _T("NoName");
        }

        module.BuildLocalFilePath(logFilePath, moduleName.c_str());
        tstring realPathName = logFilePath.GetString() + ext;

        return realPathName;
    }

    tstring LogWriter::BuildLogName(const tstring& file_name, const tstring& ext)
    {
        framework::io::Path logFilePath;
        framework::mswin::Module module;
        module.AttachAddress(&Log::Run);

        module.BuildLocalFilePath(logFilePath, (file_name + ext).c_str());
        return logFilePath.GetString();
    }

    string LogWriter::GetLogLevelString(u_int level)
    {
        string s;
        switch (level)
        {
        case __INFO:
            s = "INFO ";
            break;
        case __EVENT:
            s = "EVENT";
            break;
        case __WARN:
            s = "WARN ";
            break;
        case __ERROR:
            s = "ERROR";
            break;
        case __DEBUG:
            s = "DEBUG";
            break;
        default:
            s = "     ";
            break;
        }
        return s;
    }

    bool LogWriter::CanLog(const string& type, u_int level)
    {
        if (level != 0 && level < log_level_)
            return false;
        if (log_type_set_.find(type) == log_type_set_.end())
            return false;
        return true;
    }

    void LogWriter::OnStart()
    {
        // Default Impl. Do nothing.
    }

    void LogWriter::OnStop()
    {
        // Default Impl. Do nothing.
    }

    void LogWriter::OnClearLog()
    {
        // Default Impl. Do nothing.
    }

    //////////////////////////////////////////////////////////////////////////
    // FileLogWriter

    FileLogWriter::FileLogWriter(const tstring& file_name, const tstring& ext)
    {
        file_name_ = BuildLogName(file_name, ext);
    }

    FileLogWriter::FileLogWriter(const tstring& ext)
    {
        file_name_ = BuildDefaultLogName(ext);
    }

    FileLogWriter::p FileLogWriter::Create(const tstring& file_name, const tstring& ext)
    {
        return FileLogWriter::p(new FileLogWriter(file_name, ext));
    }

    FileLogWriter::p FileLogWriter::Create(const tstring& ext)
    {
        return FileLogWriter::p(new FileLogWriter(ext));
    }

    void FileLogWriter::OnOutputLog(const string& text)
    {
        if (fout_)
        {
            fout_ << text;
            fout_.flush();
        }
    }

    void FileLogWriter::OnStart()
    {
        framework::io::Path file_path;
        file_path.Assign(file_name_.c_str());
        file_path.RemoveFileSpec();
        CreateDirectoryRecursive(file_path.GetString());

        if (fout_) fout_.close();
        fout_.clear();
        fout_.open(file_name_.c_str(), ios_base::out | ios_base::trunc, _SH_DENYNO);
        if (!fout_)
        {
            cerr << "Fail to Open Log file." << endl;
            fout_.close();
            fout_.clear();
        }
    }

    void FileLogWriter::OnStop()
    {
        fout_.close();
        fout_.clear();
    }

    void FileLogWriter::OnClearLog()
    {
        OnStart();
    }

    bool FileLogWriter::CreateDirectoryRecursive(const tstring& directory)
    {
        framework::io::Path parent_dir;
        parent_dir.Assign(directory.c_str());
        if (CreateDirectory(directory.c_str(), NULL) == FALSE)
        {
            if (GetLastError() == ERROR_ALREADY_EXISTS)
                return true;
            else
            {
                parent_dir.RemoveBackslash();
                parent_dir.RemoveFileSpec();

                CreateDirectoryRecursive(parent_dir.GetString());
                return CreateDirectory(directory.c_str(), NULL);
            }
        }
        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    // ConsoleLogWriter

    ConsoleLogWriter::ConsoleLogWriter()
    {
    }

    ConsoleLogWriter::p ConsoleLogWriter::Create()
    {
        return ConsoleLogWriter::p(new ConsoleLogWriter());
    }

    void ConsoleLogWriter::OnOutputLog(const string& text)
    {
        cout << text;
    }

    //////////////////////////////////////////////////////////////////////////
    // NullLogWriter

    NullLogWriter::NullLogWriter()
    {
    }

    NullLogWriter::p NullLogWriter::Create()
    {
        return NullLogWriter::p(new NullLogWriter());
    }

    void NullLogWriter::OnOutputLog(const string& text)
    {
        // does nothing!
    }

    //////////////////////////////////////////////////////////////////////////
    // Log

    Log::p Log::log_(new Log());

    Log::Log()
        : work_(NULL)
        , thread_(NULL)
        , null_logger_(NullLogWriter::Create())
    {
    }

    void Log::Start()
    {
        if (NULL == work_)
        {
            ios_ = new boost::asio::io_service();
            work_ = new boost::asio::io_service::work(*ios_);
            thread_ = ::CreateThread( NULL, 0, Log::Run, NULL, 0, NULL );
        }
    }

    void Log::Stop()
    {
        if (NULL != work_)
        {
            ios_->post(
                boost::bind( &Log::ClearLogWriters, shared_from_this() )
                );
            delete work_;
            work_ = NULL;
            ios_->stop();
            ::WaitForSingleObject( thread_, 5 * 1000 );
            ::TerminateThread(thread_, 0);
            delete ios_;
            ios_ = NULL;
        }
    }

    DWORD Log::Run(LPVOID pParam)
    {
        Inst().ios_->run();
        return 0;
    }

    bool Log::AddLogWriter(const string& name, LogWriter::p log_writer)
    {
        if (!IsRunning()) return false;

        if (!log_writer) 
        {
            log_writer = null_logger_;
        }
        if (HasLogWriter(name) == false)
        {
            log_writer->Start();
            log_writers_[name] = log_writer;
            return true;
        }
        return false;
    }

    bool Log::AddFileLogWriter(const string& name, const tstring& file_name)
    {
        if (!IsRunning()) return false;
        if (HasLogWriter(name) == false)
        {
            LogWriter::p log_writer = FileLogWriter::Create(file_name, _T(".log"));
            log_writer->Start();
            log_writers_[name] = log_writer;
            return true;
        }
        return false;
    }

    LogWriter::p Log::GetLogWriter(const string& name)
    {
        if (!IsRunning()) return null_logger_;

        // special case
        if (name == DEFAULT_LOG_NAME)
            AddLogWriter(name, FileLogWriter::Create());
        else if (name == CONSOLE_LOG_NAME)
            AddLogWriter(name, ConsoleLogWriter::Create());
        else if (name == STATISTIC_LOG_NAME)
            AddLogWriter(name, FileLogWriter::Create(_T(".statistic")));

        // find
        if (HasLogWriter(name) == false)
        {
            return null_logger_;
        }
        return log_writers_[name];
    }

    bool Log::HasLogWriter(const string& name)
    {
        if (!IsRunning()) return false;
        return log_writers_.find(name) != log_writers_.end();
    }

    void Log::RemoveLogWriter(const string& name)
    {
        if (!IsRunning()) return ;
        LogWriterMap::iterator it = log_writers_.find(name);
        if (it != log_writers_.end())
        {
            it->second->Stop();
            log_writers_.erase(it);
        }
    }

    void Log::ClearLogWriters()
    {
        LogWriterMap::iterator it = log_writers_.begin();
        while (it != log_writers_.end())
        {
            it->second->Stop();
            it = log_writers_.erase(it);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // ¼æÈÝ framework::Log
    void Log::WriteLog(u_int level, const string& type, const string& text)
    {
        if (!IsRunning()) return ;
        GetDefaultLogWriter()->WriteLog(level, type, text);
    }

    void Log::WriteLogT(u_int level, const string& type, const tstring& text)
    {
        if (!IsRunning()) return ;
        GetDefaultLogWriter()->WriteLogT(level, type, text);
    }

    //ÓÃÓÚ¿ØÖÆÃüÁîµÄÆÁÄ»Êä³ö
    void Log::WriteConsoleLine(const string& text)
    {
        if (!IsRunning()) return ;
        GetConsoleLogWriter()->WriteLine(text);
    }
    void Log::WriteConsoleLineT(const tstring& text)
    {
        if (!IsRunning()) return ;
        GetConsoleLogWriter()->WriteLineT(text);
    }
    void Log::WriteConsole(const string& text)
    {
        if (!IsRunning()) return ;
        GetConsoleLogWriter()->Write(text);
    }
    void Log::WriteConsoleT(const tstring& text)
    {
        if (!IsRunning()) return ;
        GetConsoleLogWriter()->WriteT(text);
    }
    void Log::WriteConsoleLog(u_int level, const string& type, const string& text)
    {
        if (!IsRunning()) return ;
        GetConsoleLogWriter()->WriteLog(level, type, text);
    }
    void Log::WriteConsoleLog(const string& text)
    {
        if (!IsRunning()) return ;
        GetConsoleLogWriter()->WriteLog(0, "", text);
    }
    void Log::WriteConsoleLogT(const tstring& text)
    {
        if (!IsRunning()) return ;
        GetConsoleLogWriter()->WriteLogT(0, "", text);
    }

    // Default,Console Log
    void Log::SetLogLevel(u_int level)
    {
        if (!IsRunning()) return ;
        GetDefaultLogWriter()->SetLogLevel(level);
    }
    void Log::SetConsoleLevel(u_int level)
    {
        if (!IsRunning()) return ;
        GetConsoleLogWriter()->SetLogLevel(level);
    }
    void Log::SetLogTypeOn(const string& type)
    {
        if (!IsRunning()) return ;
        GetDefaultLogWriter()->SetLogTypeOn(type);
    }
    void Log::SetConsoleTypeOn(const string& type)
    {
        if (!IsRunning()) return ;
        GetConsoleLogWriter()->SetLogTypeOn(type);
    }
    void Log::SetLogTypeOff(const string& type)
    {
        if (!IsRunning()) return ;
        GetDefaultLogWriter()->SetLogTypeOff(type);
    }
    void Log::SetConsoleTypeOff(const string& type)
    {
        if (!IsRunning()) return ;
        GetConsoleLogWriter()->SetLogTypeOff(type);
    }

}
