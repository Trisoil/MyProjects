using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher.Filter
{
    using Lucene.Net.Index;
    using Lucene.Net.Search;

    public class ContainTermEnum : PrefixTermEnum 
    {
        private Term prefix;
        private bool endEnum = false;

        public ContainTermEnum(IndexReader reader, Term contain)
            : base(reader, contain)
        {
        }

        public override bool EndEnum()
        {
            return endEnum;
        }

        public override bool TermCompare(Term term)
        {
            prefix = base.GetPrefixTerm();
            if ((System.Object)term.Field() == (System.Object)prefix.Field() && term.Text().Equals(prefix.Text()))
            {
                return true;
            }
            endEnum = true;
            return false;
        }
    }
}
