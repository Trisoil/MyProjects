using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher.Analyzer
{
    using Lucene.Net.Analysis;

    public class ContainAnalyzer : Analyzer 
    {

        public override TokenStream TokenStream(string fieldName, System.IO.TextReader reader)
        {
            throw new NotImplementedException();
        }
    }
}
