using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.ProcessManageMonitor.Core.Cache
{
    public class AppSettingCache
    {
        #region Fields

        private Dictionary<string, string> _dic;

        public static readonly AppSettingCache Instance = new AppSettingCache();

        private AppSettingCache()
        { }

        #endregion

        public object RefreshCache()
        {
            string config = string.Format("{0}/Config/BoxAppSetting.xml", AppDomain.CurrentDomain.BaseDirectory);
            var root = XElement.Load(config);
            Dictionary<string, string> dic = new Dictionary<string, string>();
            var settings = from data in root.Elements("Setting") select new KeyValuePair<string, string>((string)data.Attribute("Name"), (string)data.Attribute("Value"));
            foreach (var setting in settings)
            {
                dic.Add(setting.Key, setting.Value);
            }
            _dic = dic;
            return null;
        }

        public Dictionary<string, string> Dictionary
        {
            get {
                return _dic;
            }
        }

    }
}
