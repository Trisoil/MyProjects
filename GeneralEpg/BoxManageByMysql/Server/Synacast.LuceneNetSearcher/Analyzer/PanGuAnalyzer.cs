using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher.Analyzer
{
    using PanGu;
    using Lucene.Net.Analysis;

    public class PanGuAnalyzer : Analyzer 
    {
        private bool _OriginalResult = false;

        public PanGuAnalyzer()
        {
        }

        /// <summary>
        /// Return original string.
        /// Does not use only segment
        /// </summary>
        /// <param name="originalResult"></param>
        public PanGuAnalyzer(bool originalResult)
        {
            _OriginalResult = originalResult;
        }

        public override TokenStream TokenStream(string fieldName, System.IO.TextReader reader)
        {
            TokenStream result = new PanGuTokenizer(reader, _OriginalResult);
            result = new LowerCaseFilter(result);
            return result;
        }
    }
}
