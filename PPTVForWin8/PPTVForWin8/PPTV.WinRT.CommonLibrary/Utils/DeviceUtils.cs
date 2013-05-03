using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.Utils
{
    public class DeviceUtils
    {
        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool GetDiskFreeSpaceEx(string lpDirectoryName,
                                              out ulong lpFreeBytesAvailable,
                                              out ulong lpTotalNumberOfBytes,
                                              out ulong lpTotalNumberOfFreeBytes);

        public static ulong GetFreeBytesAvailable()
        {
            ulong FreeBytesAvailable;
            ulong TotalNumberOfBytes;
            ulong TotalNumberOfFreeBytes;
            bool success = GetDiskFreeSpaceEx(".",
                                            out FreeBytesAvailable,
                                            out TotalNumberOfBytes,
                                            out TotalNumberOfFreeBytes);
            if (!success) return ulong.MinValue;
            return FreeBytesAvailable;
        }
    }
}
