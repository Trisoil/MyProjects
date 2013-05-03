using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.LuceneNet
{
    using Synacast.LuceneNetSearcher.Index;

    public abstract class NewLuceneNetBase
    {
        private IndexBase _indexdic;

        public IndexBase IndexDic
        {
            get {
                return _indexdic;
            }
        }

        public IndexBase Init()
        {
            return new IndexBase();
        }

        public void CompleteIndex(IndexBase index)
        {
            index.CompleteIndex();
            _indexdic = index;
        }
    }
}
