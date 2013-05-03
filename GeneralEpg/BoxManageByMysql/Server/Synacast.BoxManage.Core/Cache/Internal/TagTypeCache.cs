using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    public class TagTypeCache : SubCacheBase<object >
    {
        private Dictionary<int, TagTypeInfo> _dic;

        public static readonly TagTypeCache Instance = new TagTypeCache();

        public Dictionary<int, TagTypeInfo> Dictionary
        {
            get {
                return _dic;
            }
        }

        public override object  RefreshCache()
        {
            string hql = "from TagTypeInfo t";
            var list = TagTypeInfo.Execute(new HqlReadQuery<TagTypeInfo>(hql)) as IList<TagTypeInfo>;
            _dic = list.ToPerfectDictionary(t => t.TagTypeID);
            list.Clear();
            return null;
        }

        public TagTypeInfo this[int tagId]
        {
            get {
                if (_dic.ContainsKey(tagId))
                    return _dic[tagId];
                return null;
            }
        }
    }
}
