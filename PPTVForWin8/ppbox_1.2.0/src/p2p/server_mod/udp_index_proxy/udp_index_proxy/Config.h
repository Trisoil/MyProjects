#pragma once

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <map>
#include <fstream>

using namespace std;

namespace framework
{
    inline wstring s2w(const string& value, const wstring& default_value = L"")
    {
        int nSize = ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)value.c_str(), value.length(), 0, 0);
        if(nSize <= 0) return default_value;

        WCHAR *pwszDst = new WCHAR[nSize+1];
        if( NULL == pwszDst) return default_value;

        MultiByteToWideChar(CP_ACP, 0,(LPCSTR)value.c_str(), value.length(), pwszDst, nSize);
        pwszDst[nSize] = 0;

        if( pwszDst[0] == 0xFEFF)
            memcpy(pwszDst, pwszDst + 1, nSize * sizeof(pwszDst[0]));

        wstring wcharString(pwszDst);
        delete[] pwszDst;

        return wcharString;
    }

    class Config
        : public boost::enable_shared_from_this<Config>
        , public boost::noncopyable
    {
    public:
        typedef boost::shared_ptr<Config> p;
        static Config& Inst() { return *inst_; }
        static p Create() { return p(new Config()); }
    public:
        void Clear() 
        { 
            configs_.clear(); 
        }
        string GetValue(const string& key, const string& default_value = "") const
        {
            ConfigMapType::const_iterator it = configs_.find(key);
            if (it != configs_.end())
                return it->second;
            return default_value;
        }
        void SetValue(const string& key, const string& value)
        {
            configs_[key] = value;
        }
        tstring GetTString(const string& key, const tstring& default_value = TEXT("")) const
        {
            if (Contains(key) == false)
                return default_value;
            string value = GetValue(key);
#ifdef _UNICODE
            return s2w(value);
#else
            return value;
#endif
        }
        int GetInteger(const string& key, int default_value = 0) const
        {
            if (Contains(key) == false)
                return default_value;
            return boost::lexical_cast<int>(GetValue(key));
        }
        double GetDouble(const string& key, double default_value = 0.0) const
        {
            if (Contains(key) == false)
                return default_value;
            return boost::lexical_cast<double>(GetValue(key));
        }
        void SetInteger(const string& key, int value)
        {
            configs_[key] = boost::lexical_cast<string>(value);
        }
        bool Contains(const string& key) const
        {
            return configs_.count(key) != 0;
        }
        string ToString() const
        {
            ostringstream oss;
            oss << "Config:" << endl;
            STL_FOR_EACH_CONST(ConfigMapType, configs_, iter)
            {
                oss << " " << iter->first << " = " << iter->second << endl;
            }
            return oss.str();
        }
    public:
        string operator[] (const string& key) const
        {
            ConfigMapType::const_iterator it = configs_.find(key);
            if (it != configs_.end())
                return it->second;
            return "";
        }
        string& operator[] (const string& key)
        {
            return configs_[key];
        }
    public:
        bool LoadConfig(const tstring& filename);
    private:
        Config() {}
    private:
        static p inst_;
    private:
        typedef map<string, string> ConfigMapType;
        ConfigMapType configs_;
    };
}
