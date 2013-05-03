#include <string.h>
#include "boost/cstdint.hpp"
#include "boost/system/error_code.hpp"
#include "framework/string/Md5.h"
#include "framework/string/Uuid.h"
#include <fstream>
#include <iostream>
#include <sstream>

//#include "framework/string/Uuid.h"
using namespace std;

const int BLOCK_SIZE = 2*1024*1024;
const int K1 = 1024;

std::string GenerateRidInfo(const std::string& filename)
{
    string sinfo;
    FILE *stream = fopen(filename.c_str(), "rb");
    if(NULL == stream)
    {
        return "open file failed";
    }
    char* buffer = new char[BLOCK_SIZE];  
    std::vector<framework::string::Md5::bytes_type> block_md5;
    std::vector<std::string> block_str;
    int readcount = 0;

    framework::string::Uuid hash_val;        
    do 
    {       
        framework::string::Md5 md5;
        md5.init();
        readcount = fread(buffer,1,BLOCK_SIZE,stream);
        if(0 == readcount )
        {
            break;
        }
        md5.update((boost::uint8_t*)buffer,readcount);   
        md5.final();
        hash_val.from_bytes(md5.to_bytes());
        //cout<<"md5:"<<md5.to_string()<<" hash md5:"<<hash_val.to_string()<<endl;
        block_md5.push_back(hash_val.to_bytes());

        //注意：这里需要输出的是hash_val，不是md5.to_string
        block_str.push_back(hash_val.to_string());
       
    }while(readcount == BLOCK_SIZE);

    boost::int64_t filesize = ftell(stream);

    //从这里开始，计算rid，rid是把每个block的md5再做一个md5实现的。
    framework::string::Md5 md5;
    md5.init();

    for(unsigned i=0;i<block_md5.size();++i)
    {
        //注意，这里传入的是block_md5而不是block_str
        md5.update(block_md5[i].data(),block_md5[i].size());
    }
    md5.final(); 

    hash_val.from_bytes(md5.to_bytes());
    delete[] buffer;

    std::ostringstream oss;
    oss << "rid=" << hash_val.to_string()<< "&filelength=" << filesize
        << "&blocknum=" << block_str.size() << "&blocksize=" << BLOCK_SIZE
        << "&blockmd5=";
    for(unsigned i=0;i<block_str.size();++i)
    {       
        oss << (i==0?"":"@") << block_str[i];
    }
    return oss.str();    
}

int main(int argc,char* argv[])
{
    //cout<<"argc:"<<argc<<" argv[1]"<<argv[1]<<endl;
    //GenerateRidInfo(string(argv[1]));
    if(argc != 2)
    {
        cout<<"usage:"<<argv[0]<<" filename"<<endl;
        return 0;
    }
    cout<<"file:"<<argv[1]<<endl<<GenerateRidInfo(string(argv[1]))<<endl;
    return 0;
}