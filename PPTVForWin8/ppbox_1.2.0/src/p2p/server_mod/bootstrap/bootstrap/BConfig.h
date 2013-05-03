/* ======================================================================
 *    BConfig.h
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    @class BConfig        ==>        “Sharp配置文件“读取类，读取有效行
 *  @class BPairConfig    ==>        读取配置文件中以“=”分隔的配对值
 * ======================================================================
 *      Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-27     cnhbdu      创建
 */

#ifndef __B_CONFIG_H__
#define __B_CONFIG_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "BHeader.h"

#include <fstream>
#include <string>

BOOTSTRAP_NAMESPACE_BEGIN

class BConfig
{
public:
    BConfig();

    BConfig(const std::string& filename);

    virtual ~BConfig();

    bool OpenConf(const std::string& filename);

    bool IsConfOpen() { return m_conf_file.is_open(); }

    void CloseConf();

    bool ReadNextLine(std::string& line);

    void BackToLastPos();

private:
    bool CheckFile(const string& filename);

private:
    std::ifstream m_conf_file;
    std::ifstream::pos_type m_last_pos;
};

class BPairConfig : public BConfig
{
public:
    BPairConfig();

    BPairConfig(const std::string& filename);

    virtual ~BPairConfig();

    bool ReadNextPair(std::string& key_s, std::string& val_s);

private:
};

BOOTSTRAP_NAMESPACE_END

#endif // __B_CONFIG_H__