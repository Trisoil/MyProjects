using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher.Searcher
{
    using Lucene.Net.Search;

    [Serializable]
    public class SearchNode
    {
        public string Key { get; set; }

        public string Pindex { get; set; }

        public string Queries { get; set; }

        public string[] Fields { get; set; }

        public BooleanClause.Occur[] Occurs { get; set; }

        //public string SortFields { get; set; }
        public int TotalTypes { get; set; }

        public string IndexValue { get; set; }

        public int Start { get; set; }

        public int Limit { get; set; }

        public bool IsNoPaging { get; set; }

        public bool IsStat { get; set; }

        public int VodType { get; set; }

        public int ShowNav { get; set; }

        public List<NumberRangeNode> NumberRangeFiters { get; set; }

        public List<LongRangeNode> LongRnageFilters { get; set; }

        public List<ContainFilterNode> ContainFilters { get; set; }
    }

    [Serializable]
    public class LongRangeNode
    {
        public string FieldName { get; set; }

        public long MinValue { get; set; }

        public long MaxValue { get; set; }
    }

    [Serializable]
    public class NumberRangeNode
    {
        public string FieldName { get; set; }

        public int MinValue { get; set; }

        public int MaxValue { get; set; }
    }

    [Serializable]
    public class ContainFilterNode
    {
        public string FieldName { get; set; }

        public string Text { get; set; }
    }
}
