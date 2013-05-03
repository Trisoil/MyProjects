using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVForWin8.Utils
{
    using PPTVData.Entity;
    using PPTVData.Factory;

    /// <summary>
    /// 云同步帮助类
    /// </summary>
    public static class CloudUtils
    {
        public static string CloudDevice(int device)
        {
            switch (device)
            {
                case 1:
                    return "来自于PC客户端";
                case 2:
                    return "来自于Ipad客户端";
                case 4:
                    return "来自于Iphone客户端";
                case 8:
                    return "来自于Android Pad客户端";
                case 16:
                    return "来自于Android Phone客户端";
                case 32:
                    return "来自于Android TV客户端";
                case 64:
                    return "来自于主站";
                default:
                    return "来自于Win8客户端";
            }
        }
    }
}
