using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Custom
{
    [Serializable]
    public class TagNodeKey
    {
        public int Type { get; set; }

        public string Dimension { get; set; }

        public override bool Equals(object obj)
        {
            var o = obj as TagNodeKey;
            if (o == null) return false;
            if (o == this) return true;
            return o.Dimension == Dimension && o.Type == Type;
        }

        public override int GetHashCode()
        {
            return ToString().GetHashCode();
        }

        public override string ToString()
        {
            return string.Format("{0}tagkey{1}", Type, Dimension);
        }
    }
}
