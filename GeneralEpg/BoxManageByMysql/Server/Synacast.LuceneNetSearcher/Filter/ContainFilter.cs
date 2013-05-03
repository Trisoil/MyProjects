using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher.Filter
{
    using Lucene.Net.Index;
    using Lucene.Net.Search;

    public class ContainFilter : MultiTermQueryWrapperFilter
    {
        public ContainFilter(Term prefix)
            : base(new ContainQuery(prefix))
		{
		}

        public virtual Term GetPrefix()
        {
            return ((ContainQuery)query).GetPrefix();
        }

        public override string ToString()
        {
            System.Text.StringBuilder buffer = new System.Text.StringBuilder();
            buffer.Append("ContainFilter(");
            buffer.Append(GetPrefix().ToString());
            buffer.Append(")");
            return buffer.ToString();
        }
    }
}
