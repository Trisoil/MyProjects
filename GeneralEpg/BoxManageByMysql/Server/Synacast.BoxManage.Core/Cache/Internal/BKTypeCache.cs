using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    internal class BKTypeCache : SubCacheBase<IList<BKTypeInfo>>
    {
        private Dictionary<int, BKTypeInfo> _dic;
        public static readonly BKTypeCache Instance = new BKTypeCache();

        private BKTypeCache()
        { }

        public override IList<BKTypeInfo> RefreshCache()
        {
            var hql = "from BKTypeInfo b";
            var list = BKTypeInfo.Execute(new HqlReadQuery<BKTypeInfo>(hql)) as IList<BKTypeInfo>;
            _dic = list.ToPerfectDictionary(bk => bk.BkTypeId);
            list.Clear();
            return null;
        }

        //public BKTypeInfo this[int index]
        //{
        //    get {
        //        BKTypeInfo info = null;
        //        _dic.TryGetValue(index, out info);
        //        return info;
        //    }
        //}

        public string this[int index]
        {
            get {
                BKTypeInfo info = null;
                _dic.TryGetValue(index, out info);
                if (info != null)
                    return info.Name;
                return string.Empty;
            }
        }
        
    }
}
