#ifndef _IPPARSER_H_
#define _IPPARSER_H_

#include <boost/filesystem.hpp>

// 纯真IP类
typedef unsigned int IP_TYPE;

const int  IP_SIZE = 4;
const int  OFFSET_SIZE = 3;
const int  INDEX_RECORD_SIZE = IP_SIZE + OFFSET_SIZE;

using namespace std;

struct location_info
{
    location_info() : area(NULL) , isp(NULL)
    {
    }

    char * area;
    char * isp;
};

class IpLocater{
private:
    enum 
    {
        REDIRECT_MODE_1 = 0x01,
        REDIRECT_MODE_2 = 0x02
    };

    int first_index;
    int last_index;
    bool is_ok;

    char * file_in_memory;
    int file_in_memory_size;
    // 用来模拟文件指针
    int current_seek_pointer;

protected:

public:
    static IpLocater * Instance(const string dbfilename = "qqwry.dat")
    {
        static IpLocater * ip_locater;
        if( NULL == ip_locater )
        {
            ip_locater = new IpLocater(dbfilename);
            if( !ip_locater->is_OK() )
            {
                delete ip_locater;
                ip_locater = NULL;
            }
        }

        return ip_locater;
    }

    IpLocater( const string dbfilename = "IP.dat" ) : file_in_memory(NULL) , file_in_memory_size(0) , current_seek_pointer(0)
    {
        FILE* dbfile;
		
		is_ok = false;
        dbfile = fopen(dbfilename.c_str(), "rb");
        if (dbfile)
        {
            fseek( dbfile , 0 , SEEK_END );
            this->file_in_memory_size =   ftell(dbfile);
            if( this->file_in_memory_size > 0 )
            {
                // 这是为了模拟 0xFF 的结尾符
                this->file_in_memory = new char[this->file_in_memory_size + 1];
                this->file_in_memory[this->file_in_memory_size] = 0xFF;
                fseek( dbfile , 0 , SEEK_SET );
                int readed = 0;
                while( !feof(dbfile) )
                {
                    int ret = fread( this->file_in_memory + readed , 1 , 10240 , dbfile );
                    if( ret > 0 )
                    {
                        readed += ret;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            current_seek_pointer = 0;
            first_index = *(int *)(this->file_in_memory + this->current_seek_pointer);
            current_seek_pointer += 4;
            last_index = *(int *)(this->file_in_memory + this->current_seek_pointer);
            current_seek_pointer += 4;

            is_ok = true;

            fclose( dbfile );
        }
    }

    ~IpLocater()
    {
        if (is_ok)
        {
            // NOTE: IpLocater 这个对象暂时是永远也不释放了，会在程序退出后释放
            delete []file_in_memory;
            is_ok = false;
        }
    }

    int getRecordCount() const
    {
        return (last_index - first_index ) / INDEX_RECORD_SIZE + 1;
    }

    char * readString(const int offset = 0)
    {
        if ( offset )
        {
            this->current_seek_pointer = offset;
        }

        char * result = this->file_in_memory + this->current_seek_pointer;
        char ch = this->file_in_memory[this->current_seek_pointer++];
        while ( ch != 0 && ch != EOF )
        {
            ch = this->file_in_memory[this->current_seek_pointer++];
        }

        return result;
    }

    inline int readInt3(const int offset = 0 )
    {
        if ( offset )
        {
            this->current_seek_pointer = offset;
        }

        int result = 0;
        memcpy( &result , this->file_in_memory + this->current_seek_pointer , 3 );
        this->current_seek_pointer += 3;
        return result;
    }

    char * readAreaAddr(const int offset = 0)
    {
        if ( offset )
        {
            this->current_seek_pointer = offset;
        }

        char b = this->file_in_memory[ this->current_seek_pointer++ ];
        if ( b == REDIRECT_MODE_1 || b == REDIRECT_MODE_2)
        {
            int areaOffset=0;
            areaOffset = this->readInt3();
            if ( areaOffset )
            {
                return readString( areaOffset );
            }
            else
            {
                return "Unkown";
            }
        }
        else
        {
            this->current_seek_pointer--;
            return readString();
        }
    }

    unsigned int readLastIp(const int offset)
    {
        this->current_seek_pointer = offset;
        unsigned int ip = 0;
        ip = *(unsigned int *)(this->file_in_memory + this->current_seek_pointer);
        // note: 这里固定4个字节就算是在64位机器上也是4
        current_seek_pointer += 4;
        return ip;
    }

    location_info readFullAddr(const int offset,int ip = 0 )
    {
        location_info address;

        this->current_seek_pointer = offset+4;

        char ch = this->file_in_memory[this->current_seek_pointer++];
        if ( ch == REDIRECT_MODE_1 )
        {
            int countryOffset = 0;
            countryOffset = this->readInt3();

            this->current_seek_pointer = countryOffset;
            char byte = this->file_in_memory[this->current_seek_pointer++];
            if ( byte == REDIRECT_MODE_2 )
            {
                int p = 0;
                p = this->readInt3();
                address.area = readString(p);
                this->current_seek_pointer = countryOffset+4;
            }
            else
            {
                address.area = readString(countryOffset);
            }

            address.isp = readAreaAddr(); // current position
        }
        else if ( ch == REDIRECT_MODE_2 )
        {
            int p = 0;
            p = this->readInt3();
            address.area = readString(p);
            address.isp = readAreaAddr( offset + 8);
        }
        else
        {
            this->current_seek_pointer --;
            address.area = readString();
            address.isp = readAreaAddr();
        }

        return address;
    }

    int find(unsigned int ip,int left ,int right)
    {
        if ( right-left == 1)
        {
            return left;
        }
        else
        {
            int middle = (left + right) / 2;

            int offset = first_index + middle * INDEX_RECORD_SIZE;
            this->current_seek_pointer = offset;
            unsigned int new_ip = 0;
            new_ip = *(unsigned int *)(this->file_in_memory + this->current_seek_pointer);
            this->current_seek_pointer += 4;

            if ( ip >= new_ip )
            {
                return find(ip,middle,right);
            }
            else
            {
                return find(ip,left,middle);
            }
        }
    }

    location_info getIpAddr( unsigned int ip)
    {
        int index = find(ip,0, getRecordCount() - 1 );
        int index_offset = first_index + index * INDEX_RECORD_SIZE + 4;
        int addr_offset = 0;
        this->current_seek_pointer = index_offset;
        addr_offset = this->readInt3();
        location_info address = readFullAddr( addr_offset,ip );
        return address;
    }

  
    string ip2string( unsigned int ip) const
    {
        ostringstream sstr;
        sstr << ((ip & 0xff000000)>>24) ;
        sstr << "." << ((ip & 0xff0000)>>16);
        sstr << "." << ((ip & 0xff00)>>8);
        sstr << "." << (ip & 0xff);
        return sstr.str();
    }

    unsigned int string2ip(const string ipstr)const
    {
        string str = ipstr;
        unsigned int ip = 0;
        int p = 0;
        p = str.find(".");
        ip += atoi(str.substr(0,p).c_str());
        ip <<= 8;
        str = str.substr(p+1,str.length());
        p = str.find(".");
        ip += atoi(str.substr(0,p).c_str());
        ip <<= 8;
        str = str.substr(p+1,str.length());
        p = str.find(".");
        ip += atoi(str.substr(0,p).c_str());
        ip <<= 8;
        ip += atoi(str.substr(p+1,str.length()).c_str());
        return ip;
    }

    location_info getIpAddr(string ip)
    {
        if (is_ok)
        {
            return getIpAddr( string2ip( ip ) );
        }

        return location_info();
    }
    

    bool is_OK()
    {
        return is_ok;
    }
};

#endif
