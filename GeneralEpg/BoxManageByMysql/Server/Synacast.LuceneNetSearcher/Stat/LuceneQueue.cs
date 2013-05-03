using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Management.Instrumentation;

namespace Synacast.LuceneNetSearcher.Stat
{
    using log4net;
    using Synacast.LuceneNetSearcher.Utils;
    using Synacast.LuceneNetSearcher.Cache;
    using Synacast.LuceneNetSearcher.Stat.Instrument;
    using Synacast.BoxManage.Core.Utils;

    public class LuceneQueue
    {
        #region Fields

        private bool _isInit = false;
        private bool _isLock = false;
        private MutexLock _lock = new MutexLock();
        private Dictionary<string, InstrumentNode> _dic;
        private List<InstrumentNode> _cache;
        private string _bufferName;

        #endregion

        public List<InstrumentNode> Items {
            get {
                return _cache;
            }
        }

        public LuceneQueue(string bufferName)
        {
            _bufferName = bufferName;
        }

        public void Init()
        {
            if (!_isInit)
            {
                _dic = new Dictionary<string, InstrumentNode>(SettingCache.LuceneDictCount);
                try
                {
                    _cache = SerizlizerUtils<List<InstrumentNode>>.DeserializeLastCache(_bufferName);
                }
                catch
                { }
                if (_cache == null)
                    _cache = new List<InstrumentNode>(SettingCache.LuceneDictCount);
                PublishCache();
                _isInit = true;
            }
        }

        public bool AddNode(string key, int hitCount, string pIdex, params object[] objs)
        {
            if (!_isLock && !_lock.TryEnter())
                return false;
            try
            {
                var count = 1;
                if (objs != null && objs.Length > 0)
                    count = int.Parse(objs[0].ToString());
                if (!_dic.ContainsKey(key))
                {
                    if (_dic.Count < SettingCache.LuceneDictCount)
                    {
                        InstrumentNode node = new InstrumentNode();
                        node.Key = key;
                        node.LastHitCount = hitCount;
                        node.SearchCount = count;
                        node.PIndex = pIdex;
                        _dic[node.Key] = node;
                    }
                }
                else
                {
                    _dic[key].LastHitCount = hitCount;
                    _dic[key].SearchCount += count;
                }
                return true;
            }
            finally
            {
                _lock.Exit();
            }
        }

        public void Refresh()
        {
            _isLock = true;
            while (_lock.TryEnter())
            { }
            try
            {
                if (_cache == null)
                {
                    _cache = _dic.Values.OrderByDescending(v => v.SearchCount).ToPerfectList();
                }
                else
                {
                    var list = new List<InstrumentNode>(SettingCache.LuceneHotWord);
                    foreach (var v in _cache)
                    {
                        Instrumentation.Revoke(v);
                        if (_dic.ContainsKey(v.Key))
                        {
                            v.SearchCount += _dic[v.Key].SearchCount;
                            _dic.Remove(v.Key);
                        }
                    }
                    _cache = _dic.Values.Union(_cache).OrderByDescending(v => v.SearchCount).Distinct().Take(SettingCache.LuceneHotWord).ToPerfectList();
                }
                PublishCache();
                _dic.Clear();
                SerizlizerUtils<List<InstrumentNode>>.SerializeLastCache(_cache, _bufferName);
            }
            finally
            {
                _isLock = false;
                _lock.Exit();
            }
        }


        private void PublishCache()
        {
            foreach (var v in _cache)
            {
                Instrumentation.Publish(v);
            }
        }
    }
}
