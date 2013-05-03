/* ======================================================================
 *    BConfig.cpp
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    配置文件读取类
 * ======================================================================
 *      Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-27     cnhbdu      创建
 */

#include "Common.h"
#include "BConfig.h"

#include <fstream>
#include <cstdio>
#include <boost/algorithm/string.hpp>

BOOTSTRAP_NAMESPACE_BEGIN
FRAMEWORK_LOGGER_DECLARE_MODULE("Bootstrap");
BConfig::BConfig()
{

}

BConfig::BConfig(const std::string& filename)
{
    OpenConf(filename);
}

BConfig::~BConfig()
{
    CloseConf();
}

bool BConfig::OpenConf(const std::string& filename)
{
    CloseConf();
    if (false == CheckFile(filename))
    {
        return false;
    }
    m_conf_file.open(filename.c_str());
    m_last_pos = m_conf_file.tellg();
    return IsConfOpen();
}

bool BConfig::CheckFile(const string& filename)
{
    bool is_ok = false;

    FILE *fp = fopen(filename.c_str(), "r");
    if (fp)
    {
        unsigned char *buf;
        fseek(fp, 0, SEEK_END);
        size_t filesize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        buf = (unsigned char *)malloc(filesize);
        fread(buf, filesize, 1, fp);
        fclose(fp);

        if (buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF)
        {
            std::cout << "utf-, check failed!" << std::endl;
            LOG4CPLUS_ERROR(Loggers::BootStrap(), "utf-, check failed!");
        }
        else
        {
            is_ok = true;
        }

        free(buf);
    }

    return is_ok;
}

void BConfig::CloseConf()
{
    if (m_conf_file.is_open())
    {
        m_conf_file.close();
    }
}

bool BConfig::ReadNextLine(std::string& line)
{
    if (false == IsConfOpen()) return false;

    while (true)
    {
        if (m_conf_file.bad() || m_conf_file.eof())
        {
            return false;
        }

        m_last_pos = m_conf_file.tellg();
        getline(m_conf_file, line);
        
        line = boost::algorithm::trim_copy(line.substr(0, line.find_first_of('#')));

        if (line.empty())
        {
            continue;
        }

        return true;
    }
}

void BConfig::BackToLastPos()
{
    m_conf_file.seekg(m_last_pos);
}

BPairConfig::BPairConfig() : BConfig()
{

}

BPairConfig::BPairConfig(const std::string& filename) : BConfig(filename)
{

}

BPairConfig::~BPairConfig()
{

}

bool BPairConfig::ReadNextPair(std::string& key_s, std::string& val_s)
{
    std::string line;
    while (ReadNextLine(line))
    {
        std::string::size_type pos = line.find('=');
        if (std::string::npos == pos) 
        {
            BackToLastPos();
            break;
        }

        key_s = boost::algorithm::trim_copy(line.substr(0, pos));
        val_s = boost::algorithm::trim_copy(line.substr(pos+1));

        if (key_s.empty()) continue;

        boost::algorithm::to_lower(key_s);
        boost::algorithm::to_lower(val_s);

        return true;
    }
    return false;
}

BOOTSTRAP_NAMESPACE_END