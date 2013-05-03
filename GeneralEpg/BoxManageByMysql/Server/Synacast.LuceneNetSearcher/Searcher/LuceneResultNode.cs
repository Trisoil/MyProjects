using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher.Searcher
{
    public class LuceneResultNode
    {
        public List<string> Result { get; set; }

        public int AllCount { get; set; }

        public Dictionary<int, int> Types { get; set; }
    }
}
