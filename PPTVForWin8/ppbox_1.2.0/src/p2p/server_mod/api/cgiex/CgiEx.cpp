#include "CgiEx.h"
#include <stdlib.h>
#include <string.h>

using namespace std;

CgiEx::CgiEx(int bNeedInit)
{

}
CgiEx::CgiEx(const char* logFile, int bNeedInit )
{

}

bool CgiEx::Run(const std::string& loadtemplate)
{
    //先不管loadtemplate这个参数了。
    map<std::string,std::string> params;
    if (0 != GetParams(params))
    {
        return false;
    }
    return DealInput(params);
};

int CgiEx::GetParams(map<std::string,std::string>& params)
{
    params.clear();
    char *query_string = getenv("QUERY_STRING");
    if (NULL == query_string)
    {
        return 0;
    }
    char * str = (char *)malloc(strlen(query_string)+1);
    char * index;
    memcpy(str, query_string, strlen(query_string)+1);

    while(NULL != (index = strchr(str, '&' )))
    {
        std::string value(str,index-str);
        strncpy(str, index + 1, strlen(str)-(index-str) + 1);
        if (NULL != (index=strchr((char*)value.c_str(),'=')))
        {
            params[std::string(value.c_str(),index - value.c_str())] = std::string(index+1);
        }
    }
    if (NULL != (index=strchr(str,'=')))
    {
        params[std::string(str,index - str)] = std::string(index+1);
    }
    if (NULL != str)
        delete str;
    str = NULL;
    return 0;
}