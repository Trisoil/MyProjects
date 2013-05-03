using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Microsoft.VisualBasic;

namespace Synacast.LuceneNetSearcher.Utils
{
    using log4net;
    using Lucene.Net.Index;
    using Lucene.Net.Store;
    using Lucene.Net.Analysis;
    using Lucene.Net.Documents;
    using Synacast.LuceneNetSearcher.Index;
    using Synacast.LuceneNetSearcher.Cache;
    using Synacast.LuceneNetSearcher.Searcher;

    public class LuceneNetUtils
    {
        public static ILog GetLogger(string name)
        {
            log4net.Config.XmlConfigurator.Configure();
            return LogManager.GetLogger(name);
        }

        /// <summary>
        /// 添加索引
        /// </summary>
        public static void AddIndex(IndexNode node, List<IndexBase> indexs)
        {
            var doc = CreateDoucment(node);
            foreach (var index in indexs)
            {
                index.AddIndex(doc);
            }
        }

        /// <summary>
        /// 添加索引
        /// </summary>
        public static void AddIndex(IndexNode node, IndexBase index)
        {
            index.AddIndex(CreateDoucment(node));
        }

        /// <summary>
        /// 添加索引
        /// </summary>
        public static void AddIndex(List<IndexNode> nodes, List<IndexBase> indexs)
        {
            var docs = new List<Document>(nodes.Count);
            foreach (var node in nodes)
                docs.Add(CreateDoucment(node));
            foreach (var index in indexs)
                index.AddIndex(docs);
        }

        /// <summary>
        /// 添加索引
        /// </summary>
        public static void AddIndex(List<IndexNode> nodes, IndexBase index)
        {
            foreach (var node in nodes)
                index.AddIndex(CreateDoucment(node));
        }

        /// <summary>
        /// 创建Document
        /// </summary>
        public static Document CreateDoucment(IndexNode node)
        {
            Document doc = new Document();
            foreach (KeyValuePair<string, string> name in node.IndexNames)
            {
                var field = new Field(name.Key, name.Value, Field.Store.YES, Field.Index.ANALYZED);
                var boost = SettingCache.Boost;
                switch (name.Key)
                { 
                    case "name":
                        break;
                    case "catalog":
                        boost = boost - 4; break;
                    case "act":
                        boost = boost - 8; break;
                    case "area":
                        boost = boost - 16; break;
                }
                field.SetBoost(boost);
                doc.Add(field);
            }
            foreach (KeyValuePair<string, string> value in node.IndexValues)
            {
                doc.Add(new Field(value.Key, value.Value, Field.Store.YES, Field.Index.NOT_ANALYZED));
            }
            if (node.IndexNumerices != null)
            {
                foreach (KeyValuePair<string, int> numeric in node.IndexNumerices)
                {
                    doc.Add(new NumericField(numeric.Key, Field.Store.YES, true).SetIntValue(numeric.Value));
                }
            }
            if (node.IndexLongs != null)
            {
                foreach (var numeric in node.IndexLongs)
                {
                    doc.Add(new NumericField(numeric.Key, Field.Store.YES, true).SetLongValue(numeric.Value));
                }
            }
            return doc;
        }

        public static int PageList(IEnumerable<string> source, SearchNode filter, XElement root)
        {
            var index = (filter.Start - 1) * filter.Limit;
            var max = index + filter.Limit;
            int num = 0;
            using (var enumerator = source.GetEnumerator())
            {
                while (enumerator.MoveNext())
                {
                    if (index <= num && num < max)
                    {
                        root.Add(new XElement("tag", enumerator.Current));
                    }
                    num++;
                }
            }
            return num;
        }

        public static string FormatTraditionChinese(string chinese)
        {
            return Strings.StrConv(chinese, VbStrConv.TraditionalChinese, 0);
        }

        public static string FormatStrArray(int capacity, params object[] sources)
        {
            var builder = new StringBuilder(capacity);
            foreach (var source in sources)
            {
                builder.Append(source);
            }
            return builder.ToString();
        }
    }
}
