using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Synacast.LuceneNetSearcher.Stat
{
    using Synacast.BoxManage.Core.Utils;
    using Synacast.LuceneNetSearcher.Utils;

    public class LuceneDictionary
    {
        private static Dictionary<int, LuceneQueue> _dic = new Dictionary<int, LuceneQueue>();
        public static readonly LuceneDictionary Instance = new LuceneDictionary();

        private LuceneDictionary()
        { }

        public void Init()
        {
            var files = Directory.GetFiles(string.Format("{0}/Config/Buffer/", AppDomain.CurrentDomain.BaseDirectory), "HotWordBuffer*");
            foreach (var file in files)
            {
                var buffername = file.Substring(file.LastIndexOf("/") + 1).Replace(".boxcache", "");
                var pindex = int.Parse(buffername.Replace("HotWordBuffer", ""));
                var node = new LuceneQueue(buffername);
                node.Init();
                _dic[pindex] = node;
            }
        }

        public Dictionary<int, LuceneQueue> Dictionary
        {
            get {
                return _dic;
            }
        }

        public bool AddNode(string key, int hitCount, string pIdex)
        {
            try
            {
                var index = int.Parse(pIdex);
                if (!_dic.ContainsKey(index))
                {
                    var node = new LuceneQueue(LuceneNetUtils.FormatStrArray(16, "HotWordBuffer", pIdex));
                    node.Init();
                    _dic[index] = node;
                }
                return _dic[index].AddNode(key, hitCount, pIdex);
            }
            catch
            {
                return false;
            }
        }

        public bool AddNode(string key, int hitCount, int searchCount, string pIdex)
        {
            try
            {
                var index = int.Parse(pIdex);
                if (!_dic.ContainsKey(index))
                {
                    var node = new LuceneQueue(LuceneNetUtils.FormatStrArray(16, "HotWordBuffer", pIdex));
                    node.Init();
                    _dic[index] = node;
                }
                return _dic[index].AddNode(key, hitCount, pIdex, searchCount);
            }
            catch
            {
                return false;
            }
        }

        public void Refresh()
        {
            foreach (var node in _dic.Values)
            {
                node.Refresh();
            }
        }
        
    }
}
