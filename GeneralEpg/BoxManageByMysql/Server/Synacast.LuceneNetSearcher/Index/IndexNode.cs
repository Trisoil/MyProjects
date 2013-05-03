using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher.Index
{
    [Serializable]
    public class IndexNode
    {
        public List<KeyValuePair<string, string>> IndexNames { get; set; }

        public List<KeyValuePair<string, string>> IndexValues { get; set; }

        public List<KeyValuePair<string, int>> IndexNumerices { get; set; }

        public List<KeyValuePair<string, long>> IndexLongs { get; set; }

    }

}
