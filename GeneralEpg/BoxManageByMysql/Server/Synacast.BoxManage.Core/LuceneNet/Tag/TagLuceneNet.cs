using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.LuceneNet.Tag
{
    public class TagLuceneNet : NewLuceneNetBase 
    {
        public static readonly TagLuceneNet Instance = new TagLuceneNet();

        private TagLuceneNet()
        { }
    }
}
