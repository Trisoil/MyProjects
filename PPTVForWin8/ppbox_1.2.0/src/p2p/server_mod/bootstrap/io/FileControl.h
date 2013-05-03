//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef FILE_CONTRON_H
#define FILE_CONTRON_H

#include <list>

namespace my_file_io {

    void FDel(const std::string& file_name);
    size_t FLen(const std::string& file_name);
    void GetDirFileList(std::string dir, std::list<std::string>& file_list);
    void FMove(const std::string& from_name, const std::string& to_name, bool del_from = true);
    bool AccessDir(const std::string dir_name);
    bool AccessFile(const std::string file_name);
    bool MakeDir(const std::string& directory);
    void DelDir(const std::string& in_dir_name, bool del_this = true);
    void CopyDir(std::string from_dir_name, std::string to_dir_name, bool copy_this);
}

#endif //FILE_CONTRON_H