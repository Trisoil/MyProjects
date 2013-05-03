using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Key
{
    using Synacast.BoxManage.Core.Cache.Custom;

    public class BoxTagKey
    {
        public string TagType;
        public int BKType;

        public BoxTagKey(string dimension, int bkType)
        {
            TagType = dimension;
            BKType = bkType;
        }

        public override bool Equals(object obj)
        {
            if (this == obj) return true;
            BoxTagKey key = obj as BoxTagKey;
            if (key == null) return false;
            return TagType == key.TagType && BKType == key.BKType;
        }

        public override int GetHashCode()
        {
            return ToString().GetHashCode();
        }

        public override string ToString()
        {
            return string.Format("{0}-{1}", TagType, BKType);
        }
    }
}
