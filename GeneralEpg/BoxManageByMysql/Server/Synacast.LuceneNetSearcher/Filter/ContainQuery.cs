using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher.Filter
{
    using Lucene.Net.Index;
    using Lucene.Net.Search;

    public class ContainQuery : PrefixQuery
    {
        private Term prefix;

        public ContainQuery(Term prefix)
            : base(prefix)
		{ 
			this.prefix = prefix;
		}

        public override FilteredTermEnum GetEnum(Lucene.Net.Index.IndexReader reader)
        {
            return new ContainTermEnum(reader, prefix);
        }
    }
}
