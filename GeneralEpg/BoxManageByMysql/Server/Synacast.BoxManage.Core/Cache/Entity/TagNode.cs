using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Language;

    [Serializable]
    public class TagNode : LanguagePack<SimpleLanguageNode>
    {
        public Dictionary<VideoNodeKey, List<int>> Channels { get; set; }

        public override bool Equals(object obj)
        {
            var o = obj as TagNode;
            if (o == null) return false;
            if (o == this) return true;
            return o.Language[CustomArray.LanguageArray[0]].Title == Language[CustomArray.LanguageArray[0]].Title;
        }

        public override int GetHashCode()
        {
            return Language[CustomArray.LanguageArray[0]].Title.GetHashCode();
        }
    }
}
