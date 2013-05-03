using System;
using System.Net;

namespace PPTVData
{
    public static class EPGSetting
    {
        public static string PlatformName = "wp7";
        public static string Auth = "3dd5a901dcf9a61d127bab56fbcf7089";

        public static void SetEPG(string platformName, string auth)
        {
            EPGSetting.PlatformName = platformName;
            EPGSetting.Auth = auth;
        }
    }
}
