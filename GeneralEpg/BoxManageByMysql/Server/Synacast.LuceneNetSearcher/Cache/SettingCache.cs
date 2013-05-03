using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.LuceneNetSearcher.Cache
{
    public class SettingCache
    {
        private Dictionary<string, string> _dic;
        public static readonly SettingCache Instance = new SettingCache();
        public static int MaxIndex;
        public static int MaxHit;
        public static int LuceneDictCount;
        public static int LuceneHotWord;
        public static bool IsStatAct;
        public static float Boost;

        private SettingCache()
        { }

        public void Init()
        {
            RefreshCache();
            MaxIndex = int.Parse(_dic["LuceneMaxIndex"]);
            MaxHit = int.Parse(_dic["LuceneMaxHit"]);
            LuceneDictCount = int.Parse(_dic["LuceneDictCount"]);
            LuceneHotWord = int.Parse(_dic["LuceneHotWord"]);
            IsStatAct = bool.Parse(SettingCache.Instance["LuceneStatAct"]);
            Boost = float.Parse(SettingCache.Instance["LuceneBoost"]);
        }

        private void RefreshCache()
        {
            string config = string.Format("{0}/Config/LuceneSetting.xml", AppDomain.CurrentDomain.BaseDirectory);
            var root = XElement.Load(config);
            Dictionary<string, string> dic = new Dictionary<string, string>();
            var settings = from data in root.Elements("Setting") select new KeyValuePair<string, string>((string)data.Attribute("Name"), (string)data.Attribute("Value"));
            foreach (var setting in settings)
            {
                dic.Add(setting.Key, setting.Value);
            }
            _dic = dic;
        }

        public string this[string name]
        {
            get {
                return _dic[name];
            }
        }
    }
}
