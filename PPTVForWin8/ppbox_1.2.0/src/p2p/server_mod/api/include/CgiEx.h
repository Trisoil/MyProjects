#ifndef CGIEX_H_CK_20110907
#define CGIEX_H_CK_20110907
#include <map>
#include <string>
#include <stdio.h>

class CgiEx
{
public:
    CgiEx(int bNeedInit = true);

    CgiEx(const char* logFile, int bNeedInit = true);

    static std::string OutputHttpHeader(int cacheTime = -1)
    {
        std::string str;			
        if (cacheTime >= 0)
        {
            char tmp[128];
            sprintf(tmp,"Cache-Control: max-age=%d\r\n",cacheTime);
            str += tmp;
        }			
        str += "Content-Type: text/xml; charset=utf-8\r\n\r\n";
        return str;
    };				

    //传入的参数表示需要加载的模板,无需加载的时候传空
    bool Run(const std::string& loadtemplate="");

public:
    // 对输入的数据进行处理，需重载它对CGI参数进行处理，同时将处理结果通过InsertValue插入输出队列
    virtual bool DealInput (std::map<std::string, std::string> &Params)=0;

private:
    //从环境变量里取到cgi的参数放到map里。
    int GetParams(std::map<std::string,std::string>& params);

};


#endif//CGIEX_H_CK_20110907