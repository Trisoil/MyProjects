using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    public class AppSettingCache : SubCacheBase <object>
    {
        #region Fields

        private Dictionary<string, string> _dic;
        private string[] _split = new string[] { "," };

        public static readonly AppSettingCache Instance = new AppSettingCache();
        public static int[] VodFilterState;
        public static int[] VodFilterDuration;
        public static int LiveHiddenMaxCount;
        public static int LanguageNum;

        private AppSettingCache()
        { }

        #endregion

        public override object RefreshCache()
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
            VodFilterState = this["VodFilterState"].Split(_split, StringSplitOptions.RemoveEmptyEntries).Select(s => int.Parse(s)).ToArray();
            VodFilterDuration = this["VodFilterDuration"].Split(_split, StringSplitOptions.RemoveEmptyEntries).Select(s => int.Parse(s)).ToArray();
            LiveHiddenMaxCount = int.Parse(this["LiveHiddenMaxCount"]);
            LanguageNum = int.Parse(AppSettingCache.Instance["LanauageNum"]);
            return null;
        }

        public bool ContainerKey(string name)
        {
            return _dic.ContainsKey(name);
        }

        public string this[string name]
        {
            get {
                return _dic[name];
            }
        }
    }
}
