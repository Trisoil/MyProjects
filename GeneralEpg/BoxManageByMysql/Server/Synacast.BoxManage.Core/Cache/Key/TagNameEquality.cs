//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Synacast.BoxManage.Core.Cache.Key
//{
//    using Synacast.BoxManage.Core.Cache.Entity;
//    using Synacast.BoxManage.Core.Cache.Custom;

//    public class TagNameEquality : IEqualityComparer<TagNode2>
//    {

//        #region IEqualityComparer<TagInfo> 成员

//        public bool Equals(TagNode2 x, TagNode2 y)
//        {
//            return x.Language[CustomArray.LanguageArray[0]].Title.Equals(y.Language[CustomArray.LanguageArray[0]].Title);
//        }

//        public int GetHashCode(TagNode2 obj)
//        {
//            return obj.Language[CustomArray.LanguageArray[0]].Title.GetHashCode();
//        }

//        #endregion
//    }
//}
