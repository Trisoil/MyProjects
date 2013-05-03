//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Synacast.BoxManage.Core.Deleted
//{
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Cache.Custom;
//    using Synacast.BoxManage.Core.Cache.Internal.Key;

//    internal class BoxTagCache : SubCacheBase<BoxTagInfo>
//    {
//        #region Fields

//        private Dictionary<BoxTagCacheKey, BoxTagInfo> _dic = new Dictionary<BoxTagCacheKey, BoxTagInfo>();
//        private string[] _split = new string[] { " " };

//        #endregion

//        public static readonly BoxTagCache Instance = new BoxTagCache();
        
//        private BoxTagCache()
//        { }

//        public override List<BoxTagInfo> RefreshCache()
//        {
//            _dic.Clear();
//            BoxTagInfo[] tags = BoxTagInfo.FindAll();
//            foreach (BoxTagInfo tag in tags)
//            {
//                string[] keys = tag.BKTags.Split(_split, StringSplitOptions.RemoveEmptyEntries);
//                foreach (string key in keys)
//                {
//                    BoxTagCacheKey dkey = new BoxTagCacheKey(key, (TagTypeEnum)tag.TagType, tag.BKType);
//                    if (!_dic.ContainsKey(dkey))
//                        _dic.Add(dkey, tag);
//                }
//            }
//            return new List<BoxTagInfo>(tags);
//        }

//        public BoxTagInfo this[BoxTagCacheKey index]
//        {
//            get {
//                BoxTagInfo info = null;
//                _dic.TryGetValue(index, out info);
//                return info;
//            }
//        }
//    }
//}
