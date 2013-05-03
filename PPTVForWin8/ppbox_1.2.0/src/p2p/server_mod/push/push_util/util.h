#ifndef _PUSH_SERVER_UTIL_H_
#define _PUSH_SERVER_UTIL_H_


#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>
#include "tinyxml/tinyxml.h"

namespace push_util
{
    inline void DeleteExitFileAndCreatNewFile(std::string& filename)
    {
        using namespace tinyxml;
        try
        {   
            boost::filesystem::path path(boost::filesystem::current_path().string());
            path /= filename;
            if (boost::filesystem::exists(path))
            {
                boost::filesystem::remove(path);
            }
            
            TiXmlDeclaration *XmlDec = new TiXmlDeclaration("1.0", "utf-8", "yes");
            TiXmlDocument *new_file = new TiXmlDocument(filename);
            new_file->LinkEndChild(XmlDec);
            new_file->SaveFile();
            delete new_file;
        }
        catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
        {
            std::cout << "boost::filesystem::exists() throw an eception." << std::endl;
            return;
        }
    }

    inline void CreatXmlFileIfNotExists(std::string& filename)
    {
        using namespace tinyxml;
        try
        {   
            boost::filesystem::path path(boost::filesystem::current_path().string());
            path /= filename;
            if (!boost::filesystem::exists(path))
            {
                TiXmlDeclaration *XmlDec = new TiXmlDeclaration("1.0", "UTF-8", "yes");
                TiXmlDocument *new_file = new TiXmlDocument(filename);
                new_file->LinkEndChild(XmlDec);
                new_file->SaveFile();
                delete new_file;
            }
        }
        catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
        {
            std::cout << "boost::filesystem::exists() throw an eception." << std::endl;
            return;
        }
    }
}

#endif