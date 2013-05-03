using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal.Key
{
    using Synacast.BoxManage.Core.Cache.Custom;

    public class BoxTagCacheKey
    {
        public string TagName;
        public string TagType;
        public int BKType;

        public BoxTagCacheKey(string tagName, string dimension, int bkType)
        {
            TagName = tagName;
            TagType = dimension;
            BKType = bkType;
        }

        public override bool Equals(object obj)
        {
            if (this == obj) return true;
            BoxTagCacheKey key = obj as BoxTagCacheKey;
            if (key == null) return false;
            return TagName == key.TagName && TagType == key.TagType && BKType == key.BKType;
        }

        public override int GetHashCode()
        {
            return ToString().GetHashCode();
        }

        public override string ToString()
        {
            return string.Format("{0}-{1}-{2}", TagName, TagType, BKType);
        }
    }
}
