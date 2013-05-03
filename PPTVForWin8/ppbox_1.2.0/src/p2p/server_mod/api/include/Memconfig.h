/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* memconfig.h
* 
* Description: 	读取配置文件到共享内存
*	如:
*	[tracker]
*	groupnum=4
*               
* --------------------
* 2010-09-02, kelvinchen create
* --------------------
******************************************************************************/

#ifndef _MEMCONFIG_H_CK_20110901
#define _MEMCONFIG_H_CK_20110901


#include <fstream>
#include <map>
#include <string>
#include <boost/system/config.hpp>
#ifndef BOOST_WINDOWS_API
#include <boost/interprocess/shared_memory_object.hpp>
#else
#include <boost/interprocess/windows_shared_memory.hpp>
#endif

#include <boost/interprocess/mapped_region.hpp>
#include <boost/shared_ptr.hpp>
#pragma pack(1)

#define MAX_HEAD_COUNT	        50  //配置中的section个数
#define MAX_CONTENT_COUNT	    500 //配置中每个section内name=value的最大个数
#define MAX_LINE_COUNT	        200 //每一行的总长度
#define MAX_NAME_COUNT	        50  //Name = value 中name的最大长度
#define MAX_VALUE_COUNT	        150  //Name = value 中value的最大长度
#define MAX_HEAD_VALUE_ONE_COUNT	20 //偏移的总长度

/** 默认共享内存访问标记：shmget第三个参数 */
//#define    SHMCONFIGFLAG 0444
//#define SHMCONFIGFLAG S_IRUSH|S_IWUSR|IPC_CREAT 

/**
* 配置相关shm key定义
* @{
*/
#define PPLIVECGIINICONFIGKEY       "5435"
/** @} */

typedef struct
{
    int     iInitFlag;  //初始化标志:   1 - 已初始化；0 - 未初始化
    int     iHeadCount; //段(section)个数

    //存放section信息，结构为HeadName0x000x00..0x00OffsetFrom0x000x00...0x00Count
    char    chHeadLine[MAX_HEAD_COUNT][MAX_LINE_COUNT];

    //存放具体name=value信息
    char    chContentLine[MAX_CONTENT_COUNT*MAX_HEAD_COUNT][MAX_LINE_COUNT];
}PPliveMemConfig;

#pragma pack()

/**
* 打印配置内容
*
* @param pPPLiveCgiConfig 配置结构PPLiveCgiConfig对象指针
* @return 无
*/
//void PrintfConfig(PPLiveCgiConfig *pPPLiveCgiConfig);

/**
* 基于共享内存的配置访问类
* 
* 典型的section-key-value格式配置访问
*
*/
class CMemConfigFile {
public:
    /**
    * 默认构造函数
    */
    CMemConfigFile();

    /**
    * 构造函数
    *
    * 从共享内存中读取配置
    *
    * @param szFileName 无用~
    * @param shmKey 共享内存key
    **/
    CMemConfigFile(const char* szFileName,const char* shmKey);

    /**
    * 析构函数
    */
    virtual ~CMemConfigFile();

public:
    /**
    * 配置项读取
    *
    * 如果没有找到，则返回空。
    *
    * @param szName section.key格式
    * @return 配置项
    */
    std::string operator[](const char* szName);

    /**
    * 配置项读取
    *
    * 如果没有找到，则返回空。
    *
    * @param szSection section name
    * @param szName key name 
    * @return 配置项
    */
    std::string operator()(const char* szSection, const char* szName);

    /**
    * 打印配置内容
    * 
    * 打印共享内存块中的配置内容
    *
    * @return 无
    */
    int PrintfConfig();

private:
    /**
    * 去掉字符串首尾空格, 并消除注释内容
    *
    * @param szLine 待处理字符串
    * @return 0 - 成功；其他 - 失败
    */
    static int StrimString(char* szLine);

    //int ReadFile(const char* szFileName);

    /**
    * 从配置映射表中获取配置值
    * 
    * 根据 section.key格式从映射表中查找对应配置项，
    * 如果没找到，返回空字符串。
    *
    * @param szSection 目标配置section
    * @param szName 目标配置key
    * @return 配置项
    **/
    std::string GetConfigFromMap(const char* szSection, const char* szName);

    /**
    * 读取配置，构造配置映射表
    *
    * @return 0 - 成功； 其他 - 失败
    **/
    int constructMap();

    /**
    * 根据共享内存中的配置，构造配置映射表
    * 
    * @param szConfigFile 无用~
    * @return 0 - 成功；其他失败
    * @see constructMap()
    */
    int ParseFile(const char* szConfigFile);
private:
    PPliveMemConfig *pPPLiveMemConfig_;     // 位于共享内存的配置内存块
    //key_t m_shmKey;                 // 共享内存key
    std::string shmKey_;
    std::map<std::string, std::string> configMap_;// 配置映射表
    int initflag_;                   // 标记是否初始化？ 0: 未初始化，1： 已初始化

#ifndef BOOST_WINDOWS_API
    typedef boost::shared_ptr<boost::interprocess::shared_memory_object> SharedMemoryPtr;
#else
    typedef boost::shared_ptr<boost::interprocess::windows_shared_memory> SharedMemoryPtr;
#endif

    SharedMemoryPtr shared_memory_;
    boost::shared_ptr<boost::interprocess::mapped_region> mapped_region_;
};

#ifndef BOOST_WINDOWS_API
#define NGINX_ROOT "/usr/local/nginx"
#define PPLIVECGI_INI_FILENAME  NGINX_ROOT"/conf/pplivecgiconfig.ini"
#else
#define PPLIVECGI_INI_FILENAME "D:\\work\\p2p\\build\\msvc-90\\server_mod\\cgi\\Debug\\pplivecgi.txt"
#endif

/**
* PpliveCgiConfig访问类
*  
* 具体用法请参见基类
*
* @see CMemConfigFile
*/
class CPPliveCgiConfigFile
    : public CMemConfigFile
{
public:
    CPPliveCgiConfigFile() : CMemConfigFile(PPLIVECGI_INI_FILENAME,PPLIVECGIINICONFIGKEY) {
    };
};

#endif

