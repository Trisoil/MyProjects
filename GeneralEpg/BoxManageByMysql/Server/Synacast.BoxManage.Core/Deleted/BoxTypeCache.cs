//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Synacast.BoxManage.Core.Deleted
//{
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Utils;
//    using Synacast.BoxManage.Core.Cache.Custom;
//    using Synacast.NhibernateActiveRecord.Query;
//    using Synacast.BoxManage.Core.Cache;

//    public class BoxTypeCache : CacheBase<List<BoxTypeInfo>>
//    {
//        #region Fields

//        private Dictionary<string, BoxTypeInfo> _dic;

//        #endregion

//        public static readonly BoxTypeCache Instance = new BoxTypeCache();

//        private BoxTypeCache()
//        { }

//        public override void RefreshCache()
//        {
//            string hql = "from BoxTypeInfo b";
//            var types = BoxTypeInfo.Execute(new HqlReadQuery<BoxTypeInfo>(hql)) as List<BoxTypeInfo>;
//            var dic = new Dictionary<string, BoxTypeInfo>(types.Count * 2);
//            foreach (BoxTypeInfo type in types)
//            {
//                string[] keys = type.BKTypes.Split(SplitArray.BlankArray, StringSplitOptions.RemoveEmptyEntries);
//                foreach (string key in keys)
//                {
//                    if (!dic.ContainsKey(key))
//                        dic.Add(key, type);
//                }
//            }
//            _dic = dic;
//            _cache = types;
//        }

//        internal BoxTypeInfo this[string index]
//        {
//            get
//            {
//                BoxTypeInfo info = null;
//                _dic.TryGetValue(index, out info);
//                return info;
//            }
//        }

//        internal int this[int index]
//        {
//            get
//            {
//                BoxTypeInfo info = null;
//                _dic.TryGetValue(index.ToString(), out info);
//                if (info == null)
//                    return 0;
//                return info.TypeID;
//            }
//        }
//    }
//}
