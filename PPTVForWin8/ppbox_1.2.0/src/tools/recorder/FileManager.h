#ifndef _RECORDER_FILE_MANAGER_H_
#define _RECORDER_FILE_MANAGER_H_

//#include "tools/recorder/TsFile.h"
//#include "tools/recorder/M3uFile.h"
#include <fstream>

namespace server_mod
{
    namespace live
    {           
        namespace live_recorder
        {
        
            //负责写文件，创建路径 . 创建m3u文件
            //配置项需要 : 文件夹创建的数据 m3u文件个数
            class FileManager
            {
            public:
                FileManager(std::string& ,std::string& ,long&);
                virtual ~FileManager();
                
                //提供给 Channel While循环使用
                bool Destory();
                bool changeFile();  //临时文件切转成流文件
                bool write(const char*,int);

            private:
                //获取下一个文件名   param: true 需要自加 false 不需要自加
                std::string next_file_name(bool = true);
                std::string get_writing_name();

                //获取当前索引值，在程序启动时调用
                void getIndex();
                
                

                //创建写ts文件的目录
                void CreateDir();
                
                //写m3u文件
                void updateM3u();
                void WriteM3uBegin();
                void WriteM3uEnd();

                //获取文件头的整形值
                long getFileIndex(const char*);

            private:
                //当前File的索引
                long m_iFileIndex;
                
                // m3u列表个数
                long m_iM3uFileSize;

                //根目录路径
                std::string m_strRootDir;
                
                //临时文件名
                std::string m_strTempFileName;
                
                //临时文件已写入大小
                unsigned long m_iWriteSize;
                
                //频道名称
                std::string m_channelName;

                //写ts文件
                std::fstream m_tsFile; 

                //写m3u8文件
                std::fstream m_m3uFile;
            };
        }
    }
}
#endif
