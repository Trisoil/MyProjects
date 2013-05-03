//Disk.h

#ifndef _DISK_H_
#define _DISK_H_

#if WIN32
#include <Windows.h>
#elif defined __linux__
#include <sys/statfs.h>
#else
#include <sys/param.h>
#include <sys/mount.h>
#endif

namespace Disk
{
    Util::ErrorCode get_logical_devices(
        std::vector<std::string> & devs)
    {
#if WIN32
        UINT32 dwSize = GetLogicalDriveStrings(NULL, NULL);
        if (dwSize == 0) {
            return Util::ERROR_ERROR;
        }
        char *disk = NULL;
        disk = new char[dwSize];
        assert(disk != NULL);
        ZeroMemory(disk, dwSize);
        GetLogicalDriveStrings(dwSize, (char*)disk);

        char *pDisk = disk;
        for ( ;(*pDisk != NULL) && (*(pDisk +1) == ':'); pDisk += 4) {
            devs.push_back(pDisk);
        }
        return Util::ERROR_SUCCEED;
#else
        return Util::ERROR_UNSUPPORTED;
#endif
    }

    Util::ErrorCode get_space(
        std::string const & directory, 
        Util::UInt64 & total_space, 
        Util::UInt64 & free_space, 
        Util::UInt64 & available_space)
    {
#if WIN32
        ULARGE_INTEGER ulFreeByteAvailable;
        ULARGE_INTEGER ulTotalNumberOfBytes;
        ULARGE_INTEGER ulTotalNumberOfFreeBytes;
        if (GetDiskFreeSpaceEx(directory.c_str(), &ulFreeByteAvailable, &ulTotalNumberOfBytes, &ulTotalNumberOfFreeBytes) == 0)
            return Util::ERROR_ERROR;
        total_space = ulTotalNumberOfBytes.QuadPart;
        free_space = ulTotalNumberOfFreeBytes.QuadPart;
        available_space = ulFreeByteAvailable.QuadPart;
        return Util::ERROR_SUCCEED;
#else
    struct statfs buf;
    if (statfs(directory.c_str(), &buf) == 0) {
        total_space = Util::UInt64(buf.f_bsize) * Util::UInt64(buf.f_blocks);
        free_space = Util::UInt64(buf.f_bsize) * Util::UInt64(buf.f_bfree);
        available_space = Util::UInt64(buf.f_bsize) * Util::UInt64(buf.f_bavail);
        return Util::ERROR_SUCCEED;
    }
    return Util::ERROR_ERROR;
#endif
    }
}

#endif // #ifndef _DISK_H_
