//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "LocalDisk.h"
#include "SmallFileMode/SmallFileLocalDisk.h"

#ifndef WIN32
#include "BigFileMode/BigFileLocalDiskManager.h"
#include <sys/stat.h>
static boost::uint64_t  reg_disk_file_size_bound = (1ULL << 30) * 128;
#endif

namespace super_node
{

boost::shared_ptr<ILocalDisk> CreateLocalDisk(const std::string& path)
{
    if (boost::filesystem::is_directory(path))
        return boost::shared_ptr<ILocalDisk>(new SmallFileLocalDisk(path));

#ifndef WIN32
    struct stat st_info;
    boost::uint64_t file_size_bound = (boost::uint64_t)(-1);

    if (stat(path.c_str(), &st_info) == 0)
    {
        if (S_ISBLK(st_info.st_mode))
            file_size_bound = 0; // will use lseek64 to get the file size
        else if (S_ISREG(st_info.st_mode))
            file_size_bound = reg_disk_file_size_bound;
    }
    else
    {
        // file not exist, treat it as a regular file and create it
        file_size_bound = reg_disk_file_size_bound;
    }

    if (file_size_bound != (boost::uint64_t)(-1))
        return boost::shared_ptr<ILocalDisk>(BigFileLocalDiskManager::Instance().CreateBigFileLocalDisk(path, file_size_bound));
#endif

    return boost::shared_ptr<ILocalDisk>();
}

}
