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
        
            //����д�ļ�������·�� . ����m3u�ļ�
            //��������Ҫ : �ļ��д��������� m3u�ļ�����
            class FileManager
            {
            public:
                FileManager(std::string& ,std::string& ,long&);
                virtual ~FileManager();
                
                //�ṩ�� Channel Whileѭ��ʹ��
                bool Destory();
                bool changeFile();  //��ʱ�ļ���ת�����ļ�
                bool write(const char*,int);

            private:
                //��ȡ��һ���ļ���   param: true ��Ҫ�Լ� false ����Ҫ�Լ�
                std::string next_file_name(bool = true);
                std::string get_writing_name();

                //��ȡ��ǰ����ֵ���ڳ�������ʱ����
                void getIndex();
                
                

                //����дts�ļ���Ŀ¼
                void CreateDir();
                
                //дm3u�ļ�
                void updateM3u();
                void WriteM3uBegin();
                void WriteM3uEnd();

                //��ȡ�ļ�ͷ������ֵ
                long getFileIndex(const char*);

            private:
                //��ǰFile������
                long m_iFileIndex;
                
                // m3u�б����
                long m_iM3uFileSize;

                //��Ŀ¼·��
                std::string m_strRootDir;
                
                //��ʱ�ļ���
                std::string m_strTempFileName;
                
                //��ʱ�ļ���д���С
                unsigned long m_iWriteSize;
                
                //Ƶ������
                std::string m_channelName;

                //дts�ļ�
                std::fstream m_tsFile; 

                //дm3u8�ļ�
                std::fstream m_m3uFile;
            };
        }
    }
}
#endif
