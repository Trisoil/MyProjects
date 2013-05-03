#ifndef _GTESTFRAMEWORK_REPORTBASE_H_
#define _GTESTFRAMEWORK_REPORTBASE_H_

#include <string>
#include <stdio.h>
#include "gtest_main/ProjectInfo.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
#   define BOOST_WINDOWS_API
#else
#   define BOOST_POSIX_API 
#endif

#  ifndef BOOST_WINDOWS_API
#    include <iconv.h>
#  else
#    include <windows.h>
#  endif

namespace gtestframe
{
    const char UNICODE_BOM[4] = { char(0xEF), char(0xBB), char(0xBF), char('\0')};
    class ReportBase
    {
    protected:
        ReportBase(char* path):
            path_(path)
        {
        }

        void trans_encode(std::string & dest, std::string const * src)
        {
#ifdef BOOST_WINDOWS_API
            convert( *src, dest );
#else
            dest = *src;
#endif
        }

        void writeXMLHeader( FILE* fp , char* type )
        {
#ifdef BOOST_WINDOWS_API
            fprintf( fp , UNICODE_BOM );
#endif
            fprintf( fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" );
            fprintf( fp, "<tests type=\"%s\" version=\"%s\">\n" , type, gtestframe::ProjectInfo::GetInstance()->pro_info().pro_version.c_str() );
        }

        void writeXMLTail( FILE * fp )
        {
            fprintf( fp, "</tests>" );
        }

        void case_type(std::string & dest, std::string const * src)
        {
            dest = src->substr( 0, src->find("_",0) );
        }

        int replace_all( std::string & str, const std::string & pattern, const std::string & newpat )
        { 
            int count = 0; 
            const size_t nsize = newpat.size();
            const size_t psize = pattern.size();

            for( size_t pos = str.find( pattern, 0 );
                pos != std::string::npos;
                pos = str.find( pattern,pos + nsize ) )
            { 
                str.replace( pos, psize, newpat ); 
                count++; 
            } 
            return count; 
        }

        void module_path(std::string& path)
        {
            path.insert( 0, "/" );
            replace_all( path, "_", "/" );
        }

        std::string project_name()
        {
            return gtestframe::ProjectInfo::GetInstance()->pro_info().pro_name;
        }

        const char* export_path(std::string const & filename)
        {
            p.append(filename);
            if(path_==NULL)
            {
#ifdef BOOST_WINDOWS_API
                p.insert( 0, "c:/" );
#endif
            }
            else
            {
                p.insert( 0, path_ );
            }
            return p.c_str();
        }
        void convert(
            std::string const & s1, 
            std::string & s2)
        {
#ifdef BOOST_WINDOWS_API
            std::string t1 = "utf-8";
            std::string t2 = "acp";
            static std::map<std::string, unsigned int> name_cp;
            if (name_cp.empty()) {
                name_cp["unicode"] = 0;
                name_cp["acp"] = CP_ACP;
                name_cp["tacp"] = CP_THREAD_ACP;
                name_cp["utf8"] = CP_UTF8;
                name_cp["utf-8"] = CP_UTF8;
                name_cp["gbk"] = 936;
            }
            std::basic_string<WCHAR> unicode;
            int n1 = MultiByteToWideChar(name_cp[t2], 0, &s1[0], s1.length(), NULL, 0);
            if (n1 > 0) {
                unicode.reserve(n1);
                MultiByteToWideChar(name_cp[t2], 0, &s1[0], s1.length(), &unicode[0], n1);
                int n2 = WideCharToMultiByte(name_cp[t1], 0, &unicode[0], n1, NULL, 0, NULL, NULL);
                if (n2 > 0) {
                    s2.resize(n2);
                    WideCharToMultiByte(name_cp[t1], 0, &unicode[0], n1, &s2[0], n2, NULL, NULL);
                }
            }
#endif
        }

    private:
        char* path_;
        std::string p;
    };
} //namespace gtestframe

#endif