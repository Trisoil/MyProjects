using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.NhibernateActiveRecord.Query;
    using Synacast.BoxManage.Core.Entity;    

    public class BoxTagCache : SubCacheBase<IList<BoxTagInfo>>
    {
        private Dictionary<int, BoxTagInfo> _dic;

        private BoxTagCache()
        { }

        public static readonly BoxTagCache Instance = new BoxTagCache();

        public Dictionary<int, BoxTagInfo> Dictionary
        {
            get {
                return _dic;
            }
        }

        public override IList<BoxTagInfo> RefreshCache()
        {
            string hql = "from BoxTagInfo b where b.IsVisible=1";
            var list = BoxTagInfo.Execute(new HqlReadQuery<BoxTagInfo>(hql)) as IList<BoxTagInfo>;
            _dic = list.ToPerfectDictionary(t => t.ID);
            list.Clear();
            return null;
        }

        public BoxTagInfo this[int tagId]
        {
            get {
                BoxTagInfo boxtag = null;
                _dic.TryGetValue(tagId, out boxtag);
                return boxtag;
            }
        }
    }
}
