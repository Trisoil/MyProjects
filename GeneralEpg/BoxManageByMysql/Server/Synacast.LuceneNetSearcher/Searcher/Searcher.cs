using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Xml.Linq;

namespace Synacast.LuceneNetSearcher.Searcher
{
    #region Namespaces

    using PanGu;
    using PanGu.Match;
    using Lucene.Net.Index;
    using Lucene.Net.Search;
    using Lucene.Net.Documents;
    using Lucene.Net.QueryParsers;
    using Synacast.LuceneNetSearcher.Index;
    using Synacast.LuceneNetSearcher.Analyzer;
    using Synacast.LuceneNetSearcher.Utils;
    using Synacast.LuceneNetSearcher.Filter;
    using Synacast.LuceneNetSearcher.Cache;
    using Synacast.LuceneNetSearcher.Stat;

    #endregion

    public class Searcher
    {
        private static string[] _splitWord = new string[] { " " };
        private static MatchOptions _option = new MatchOptions() { SynonymOutput = true, WildcardOutput = true };

        public static LuceneResultNode Search(IndexBase index, SearchNode node)
        {
            try
            {
                var query = MultiFieldQueryParser.Parse(Lucene.Net.Util.Version.LUCENE_29, node.Queries, node.Fields, node.Occurs, new PanGuAnalyzer(true));
                foreach (ContainFilterNode cf in node.ContainFilters)
                {
                    query = new FilteredQuery(query, new ContainFilter(new Term(cf.FieldName, cf.Text)));
                }
                foreach (LongRangeNode lr in node.LongRnageFilters)
                {
                    query = new FilteredQuery(query, NumericRangeFilter.NewLongRange(lr.FieldName, lr.MinValue, lr.MaxValue, true, true));
                }
                return ResponseSearch(index, query, node, 0);
            }
            catch (Lucene.Net.QueryParsers.ParseException)
            {
                return new LuceneResultNode() { AllCount = 0, Result = new List<string>() };
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public static LuceneResultNode SearchEx(IndexBase index, SearchNode node)
        {
            try
            {
                var query = MultiFieldQueryParser.Parse(Lucene.Net.Util.Version.LUCENE_29, node.Queries, node.Fields, node.Occurs, new PanGuAnalyzer(true));
                foreach (NumberRangeNode n in node.NumberRangeFiters)
                {
                    query = new FilteredQuery(query, NumericRangeFilter.NewIntRange(n.FieldName, n.MinValue, n.MaxValue, true, true));
                }
                foreach (LongRangeNode lr in node.LongRnageFilters)
                {
                    query = new FilteredQuery(query, NumericRangeFilter.NewLongRange(lr.FieldName, lr.MinValue, lr.MaxValue, true, true));
                }
                foreach (ContainFilterNode cf in node.ContainFilters)
                {
                    query = new FilteredQuery(query, new ContainFilter(new Term(cf.FieldName, cf.Text)));
                }
                return ResponseSearch(index, query, node, 0);
            }
            catch (Lucene.Net.QueryParsers.ParseException)
            {
                return new LuceneResultNode() { AllCount = 0, Result = new List<string>() };
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public static LuceneResultNode SearchMaxHit(IndexBase index, SearchNode node, int maxHit)
        {
            try
            {
                var query = MultiFieldQueryParser.Parse(Lucene.Net.Util.Version.LUCENE_29, node.Queries, node.Fields, node.Occurs, new PanGuAnalyzer(true));
                foreach (ContainFilterNode cf in node.ContainFilters)
                {
                    query = new FilteredQuery(query, new ContainFilter(new Term(cf.FieldName, cf.Text)));
                }
                return ResponseSearch(index, query, node, maxHit);
            }
            catch (Lucene.Net.QueryParsers.ParseException)
            {
                return new LuceneResultNode() { AllCount = 0, Result = new List<string>() };
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public static LuceneResultNode SearchMaxHitEx(IndexBase index, SearchNode node, int maxHit)
        {
            try
            {
                var query = MultiFieldQueryParser.Parse(Lucene.Net.Util.Version.LUCENE_29, node.Queries, node.Fields, node.Occurs, new PanGuAnalyzer(true));
                foreach (NumberRangeNode n in node.NumberRangeFiters)
                {
                    query = new FilteredQuery(query, NumericRangeFilter.NewIntRange(n.FieldName, n.MinValue, n.MaxValue, true, true));
                }
                foreach (ContainFilterNode cf in node.ContainFilters)
                {
                    query = new FilteredQuery(query, new ContainFilter(new Term(cf.FieldName, cf.Text)));
                }
                return ResponseSearch(index, query, node, maxHit);
            }
            catch (Lucene.Net.QueryParsers.ParseException)
            {
                return new LuceneResultNode() { AllCount = 0, Result = new List<string>() };
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public static int DistinctSearch(IndexBase index, SearchNode node, XElement root)
        {
            try
            {
                var query = MultiFieldQueryParser.Parse(Lucene.Net.Util.Version.LUCENE_29, node.Queries, node.Fields, node.Occurs, new PanGuAnalyzer(true));
                foreach (ContainFilterNode cf in node.ContainFilters)
                {
                    query = new FilteredQuery(query, new ContainFilter(new Term(cf.FieldName, cf.Text)));
                }
                return ResponseDistinctSearch(index, query, node, root);
            }
            catch (Lucene.Net.QueryParsers.ParseException)
            {
                return 0;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        /// <summary>
        /// 处理客户端输入
        /// </summary>
        public static string SegmentKeyWord(string keyword)
        {
            #region Deleted
            //var cnbuilder = new StringBuilder();
            //var enbuilder = new StringBuilder ();
            //foreach (char k in keyword)
            //{
            //    if (SpellUtils.IsChinese(k.ToString()))
            //    {
            //        cnbuilder.Append(k);
            //        enbuilder.Append(" ");
            //    }
            //    else
            //    {
            //        enbuilder.Append(k);
            //    }
            //}
            //var rbuilder = new StringBuilder();
            //if (enbuilder.Length > 0)
            //{
            //    var enkeys = enbuilder.ToString().Split(_splitWord, StringSplitOptions.RemoveEmptyEntries);
            //    foreach (string enkey in enkeys)
            //    {
            //        rbuilder.AppendFormat("{0}^{1}.0 ", enkey, 1);
            //        //rbuilder.AppendFormat("{0}*^{1}.0", enkey, 1);
            //    }
            //}
            //if (cnbuilder.Length > 0)
            //{
            //    var segment = new Segment();
            //    var words = segment.DoSegment(cnbuilder.ToString(), _option);
            //    foreach (var word in words)
            //    {
            //        if (word == null)
            //            continue;
            //        rbuilder.AppendFormat("{0}^{1}.0 ", word.Word, (int)Math.Pow(3, word.Rank));
            //    }
            //}
            //return rbuilder.ToString().Trim();
            #endregion
            var builder = new StringBuilder(20);
            var segment = new Segment();
            var words = segment.DoSegment(keyword, _option);
            foreach (var word in words)
            {
                if (word == null)
                    continue;
                builder.AppendFormat("{0}^{1}.0 ", word.Word, (int)Math.Pow(3, word.Rank));
            }
            return builder.ToString();
        }

        public static string SegmentChWord(string keyword)
        {
            var builder = new StringBuilder(keyword.Length * 3);
            builder.AppendFormat("{0}^{1}.0 ", keyword, 1);
            builder.AppendFormat("{0}*^{1}.0 ", keyword, 1);
            return builder.ToString();
        }

        #region Privated

        private static LuceneResultNode ResponseSearch(IndexBase index, Query query, SearchNode snode, int maxhit)
        {
            var searcher = new IndexSearcher(index.Directory, true);
            try
            {
                int maxHit = maxhit;
                if (maxHit == 0)
                    maxHit = SettingCache.MaxHit;
                var node = new LuceneResultNode();
                var flag = false;
                TopDocs docs = null;
                int allhit = 0;
                if (snode.ShowNav == 1)
                {
                    node.Types = new Dictionary<int, int>(7);
                    for (var j = 1; j <= snode.TotalTypes; j++)
                    {
                        var filter = new ContainFilter(new Term("type", j.ToString()));
                        var docss = searcher.Search(query, filter, maxHit);
                        if (j == snode.VodType)
                        {
                            docs = docss;
                            flag = true;
                        }
                        if (docss.totalHits > 0)
                        {
                            int hit = docss.scoreDocs.Length;
                            node.Types[j] = hit;
                            allhit += hit;
                        }
                    }
                }
                if (!flag)
                {
                    if (snode.VodType == 0)
                    {
                        if (allhit == 0)
                        {
                            if (maxhit == 0)
                                allhit = 500;
                            else
                                allhit = maxhit;
                        }
                        docs = searcher.Search(query, null, allhit);
                    }
                    else
                    {
                        var filter = new ContainFilter(new Term("type", snode.VodType.ToString()));
                        docs = searcher.Search(query, filter, maxHit);
                    }
                }
                int all = docs.scoreDocs.Length;
                if (SettingCache.IsStatAct && all > 0 && snode.IsStat)
                    ThreadPool.QueueUserWorkItem(state => LuceneDictionary.Instance.AddNode(snode.Key, all, snode.Pindex));
                //if (all > maxHit)
                //    all = maxHit;
                int i = (snode.Start - 1) * snode.Limit;
                if (snode.IsNoPaging)
                {
                    i = 0;
                    snode.Limit = all;
                }
                int page = snode.Limit;
                if (page > all)
                    page = all;
                var result = new List<string>(page);

                while (i < all && result.Count < snode.Limit)
                {
                    var doc = searcher.Doc(docs.scoreDocs[i].doc);
                    string value = doc.Get(snode.IndexValue);
                    result.Add(value);
                    i++;
                }
                node.Result = result;
                node.AllCount = all;
                return node;
            }
            finally
            {
                searcher.Close();
            }
        }

        private static int ResponseDistinctSearch(IndexBase index, Query query, SearchNode snode, XElement root)
        {
            var searcher = new IndexSearcher(index.Directory, true);
            try
            {
                var docs = searcher.Search(query, null, SettingCache.MaxHit);
                IEnumerable<string> results;
                if (!snode.IsNoPaging)
                {
                    results = docs.scoreDocs.Select(v => searcher.Doc(v.doc).Get(snode.IndexValue)).Distinct();
                }
                else
                {
                    results = docs.scoreDocs.Select(v => LuceneNetUtils.FormatTraditionChinese(searcher.Doc(v.doc).Get(snode.IndexValue))).Distinct();
                }
                return LuceneNetUtils.PageList(results, snode, root);
            }
            finally
            {
                searcher.Close();
            }
        }

        

        #endregion
    }
}
