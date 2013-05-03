using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Synacast.LuceneNetSearcher
{
    using PanGu;
    using Synacast.LuceneNetSearcher.Cache;
    using Synacast.LuceneNetSearcher.Spell;
    using Synacast.LuceneNetSearcher.Stat;
    using Synacast.LuceneNetSearcher.Analyzer;

    /// <summary>
    /// Lucene初始化引擎，依赖于PanGu分词库
    /// </summary>
    public class LuceneEngine  
    {
        public void Init()
        {
            string configPath = string.Format("{0}/Config/LuceneNet.xml", AppDomain.CurrentDomain.BaseDirectory);
            if (File.Exists(configPath))
            {
                Segment.Init(configPath);
            }
            else
            {
                Segment.Init();
            }
            SpellContainer.Instance.Init();
            SettingCache.Instance.Init();
            if (SettingCache.IsStatAct)
                LuceneDictionary.Instance.Init();
        }
    }
}
