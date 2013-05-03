using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Impl.Lists
{
    public class SearchComparer:IComparer<string>
    {
        string _source;

        public SearchComparer(string source)
        {
            _source = source;
        }

        #region IComparer<string> 成员

        public int Compare(string x, string y)
        {
            if (x == _source && y != _source)
                return -1;
            else if (x != _source && y == _source)
                return 1;
            return x.Length - y.Length;
        }

        #endregion
    }
}
