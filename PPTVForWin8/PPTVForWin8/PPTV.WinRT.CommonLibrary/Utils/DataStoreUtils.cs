using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.Storage;

namespace PPTV.WinRT.CommonLibrary.Utils
{
    public static class DataStoreUtils
    {
        public static void AddLocalSettingValue(string key, object value)
        {
            var localSetting = ApplicationData.Current.LocalSettings;
            localSetting.Values[key] = value;
        }

        public static object GetLocalSettingValue(string key)
        {
            var localSetting = ApplicationData.Current.LocalSettings;
            if (localSetting.Values.ContainsKey(key))
                return localSetting.Values[key];
            return null;
        }

        public static void AddRoamingSettingValue(string key, object value)
        {
            var roamingSetting = ApplicationData.Current.RoamingSettings;
            roamingSetting.Values[key] = value;
        }

        public static object GetRoamingSettingValue(string key)
        {
            var roamingSetting = ApplicationData.Current.RoamingSettings;
            if (roamingSetting.Values.ContainsKey(key))
                return roamingSetting.Values[key];
            return null;
        }

        public static void AddRoamingCompositeValue(string key, string valueKey, object value)
        {
            var roamingSetting = ApplicationData.Current.RoamingSettings;
            var compositeValue = GetCompositeValue(roamingSetting, key);
            if (compositeValue != null)
            {
                compositeValue[valueKey] = value;
                roamingSetting.Values[key] = compositeValue;
            }
        }

        public static void RemoveRoamingCompositeValue(string key, string valueKey)
        { 
            var roamingSetting = ApplicationData.Current.RoamingSettings;
            var compositeValue = GetCompositeValue(roamingSetting, key);
            if (compositeValue != null)
            {
                compositeValue.Remove(valueKey);
                roamingSetting.Values[key] = compositeValue;
            }
        }

        public static ApplicationDataCompositeValue GetCompositeValue(ApplicationDataContainer setting, string key)
        {
            ApplicationDataCompositeValue compositeValue = null;
            if (setting.Values.ContainsKey(key))
                compositeValue = setting.Values[key] as ApplicationDataCompositeValue;
            else
                compositeValue = new ApplicationDataCompositeValue();
            return compositeValue;
        }
    }
}
