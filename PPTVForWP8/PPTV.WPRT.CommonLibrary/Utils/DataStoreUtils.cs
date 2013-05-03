using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.IsolatedStorage;

using Windows.Storage;

namespace PPTV.WPRT.CommonLibrary.Utils
{
    public static class DataStoreUtils
    {
        public static void AddLocalSettingValue(string key, object value)
        {
            var localSetting = IsolatedStorageSettings.ApplicationSettings;
            localSetting[key] = value;
        }

        public static object GetLocalSettingValue(string key)
        {
            var localSetting = IsolatedStorageSettings.ApplicationSettings;
            if (localSetting.Contains(key))
                return localSetting[key];
            return null;
        }

        //public static void AddLocalSettingValue(string key, object value)
        //{
        //    var localSetting = ApplicationData.Current.LocalSettings;
        //    localSetting.Values[key] = value;
        //}

        //public static object GetLocalSettingValue(string key)
        //{
        //    var localSetting = ApplicationData.Current.LocalSettings;
        //    if (localSetting.Values.ContainsKey(key))
        //        return localSetting.Values[key];
        //    return null;
        //}

        //public static void AddRoamingSettingValue(string key, object value)
        //{
        //    var roamingSetting = ApplicationData.Current.RoamingSettings;
        //    roamingSetting.Values[key] = value;
        //}

        //public static object GetRoamingSettingValue(string key)
        //{
        //    var roamingSetting = ApplicationData.Current.RoamingSettings;
        //    if (roamingSetting.Values.ContainsKey(key))
        //        return roamingSetting.Values[key];
        //    return null;
        //}
    }
}
