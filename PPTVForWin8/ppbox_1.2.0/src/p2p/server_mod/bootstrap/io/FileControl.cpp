//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include "FileControl.h"
#include <iostream>

namespace my_file_io
{

    void FDel(const std::string& file_name)
    {    
        try
        {
            boost::filesystem::path path_ = boost::filesystem::current_path() / file_name;
            boost::system::error_code ec;
            boost::filesystem::remove( path_, ec );
            //std::cout << "FDel\n";
        }
        catch(boost::filesystem::filesystem_error e)
        {
            //std::cout <<"FDel crash\n";
        }
    }

    size_t FLen(const std::string& file_name)
    {
        try{
            boost::filesystem::path path_ = boost::filesystem::current_path() / file_name;
            return boost::filesystem::file_size(path_);
            //std::cout << "FLen\n";
        }
        catch(boost::filesystem::filesystem_error e)
        {
            //std::cout <<"FLen crash\n";
        }
        
    }

    void GetDirFileList(const std::string dir, std::list<std::string>& file_list)
    {
        try{
            boost::filesystem::path path_ = boost::filesystem::current_path() / dir;

            int current_path_len = (boost::filesystem::current_path()).string().length();

            if ( !boost::filesystem::exists( path_ ) ) 
                return;
            boost::filesystem::directory_iterator end_itr; // 缺省构造生成一个结束迭代器
            for ( boost::filesystem::directory_iterator itr( path_ ); itr != end_itr; ++itr )
            {
                if ( boost::filesystem::is_directory(itr->status()) )
                {
                    continue; //目录跳过
                }
                else
                {
                    file_list.push_back((itr->string()).substr(current_path_len + 1));
                }
            } 
            //std::cout <<"GetDirFileList\n";
        }
        catch(boost::filesystem::filesystem_error e)
        {
            //std::cout <<"GetDirFileList crash\n";
        }
    }

    void FMove(const std::string& from_name, const std::string& to_name, 
        bool del_from /* = true */)
    {
        try{
            boost::filesystem::path from_path = boost::filesystem::current_path() / from_name;
            boost::filesystem::path to_path = boost::filesystem::current_path() / to_name;
            boost::filesystem::path to_parent_path = to_path.parent_path();

            if(boost::filesystem::exists(to_path))
                boost::filesystem::remove(to_path);

            if( !boost::filesystem::exists(to_parent_path))
                boost::filesystem::create_directory(to_parent_path);

            boost::filesystem::copy_file(from_path, to_path);

            if(del_from)
                boost::filesystem::remove(from_path);
            //std::cout <<"FMove\n";
        }
        catch(boost::filesystem::filesystem_error e)
        {
            //std::cout <<"FMove crash\n";
        }

    }

    bool AccessDir(const std::string dir_name)
    {
        try{
            boost::filesystem::path path_ = boost::filesystem::current_path() / dir_name;
            //std::cout <<"AccessDir\n";
            if(boost::filesystem::exists(path_))
                return true;
            else
                return false;
        }
        catch(boost::filesystem::filesystem_error e)
        {
            //std::cout <<"AccessDir crash\n";
        }
        
    }

    bool AccessFile(const std::string file_name)
    {
        try{
            boost::filesystem::path path_ = boost::filesystem::current_path() / file_name;
            //std::cout <<"AccessFile\n";
            if(boost::filesystem::exists(path_))
                return true;
            else
                return false;
        }
        catch(boost::filesystem::filesystem_error e)
        {
            //std::cout <<"AccessFile crash\n";
        }
        
    }
    
    bool MakeDir(const std::string& directory)
    {
        try{
            boost::filesystem::path path_ = boost::filesystem::current_path() / directory;
            //std::cout <<"MakeDir\n";
            if(boost::filesystem::create_directory(path_))
                return true;
            else
                return false;
        }
        catch(boost::filesystem::filesystem_error e)
        {
            //std::cout <<"MakeDir crash\n";
        }
        
        
    } 

    void DelDir(const std::string& in_dir_name, bool del_this)
    {
        try{
            boost::filesystem::path path_ = boost::filesystem::current_path() / in_dir_name;
            boost::filesystem::remove_all(path_);
            //std::cout <<"DelDir\n";
            if (!del_this)
                MakeDir(in_dir_name);
            
        }
        catch(boost::filesystem::filesystem_error e)
        {
            //std::cout <<"DelDir crash\n";
        }
        
    }

    

    void CopyDir(std::string from_dir_name, std::string to_dir_name, bool copy_this)
    {
        try{
            if (false == AccessDir(from_dir_name))
            {
                return;
            }

            // 防止拷贝自己
            if (from_dir_name == to_dir_name)
            {
                return;
            }

            if ( AccessDir(to_dir_name) == false)
                MakeDir(to_dir_name);

            std::list<std::string> file_list;

            GetDirFileList(from_dir_name, file_list);

            for (std::list<std::string>::iterator it = file_list.begin();
                it != file_list.end(); ++it)
            {
                std::string new_name = *it;
                std::string file_name = new_name.substr(from_dir_name.size() + (boost::filesystem::current_path()).string().length());

                new_name = to_dir_name + file_name;

                FMove(from_dir_name + "\\" + file_name, new_name, false);
            }
            //std::cout <<"CopyDir\n";
        }
        catch(boost::filesystem::filesystem_error e)
        {
            //std::cout <<"CopyDir crash\n";
        }
        
    }

}