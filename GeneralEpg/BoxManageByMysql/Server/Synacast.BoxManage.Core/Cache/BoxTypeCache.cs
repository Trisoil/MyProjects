using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.NhibernateActiveRecord.Query;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Cache.Language;

    public class BoxTypeCache : CacheBase<Dictionary<int,BoxTypeNode>>
    {
        public static readonly BoxTypeCache Instance = new BoxTypeCache();

        private BoxTypeCache()
        { }

        public override void RefreshCache()
        {
            var query = from boxtype in BoxTypeInfoCache.Instance.Items
                        join tagtype in TagTypeCache.Instance.Dictionary.Values on boxtype.TypeID equals tagtype.BoxTypeID into boxtypes
                        select new BoxTypeNode() { TypeID = boxtype.TypeID, PicLink = boxtype.PicLink, ResponseDimension = boxtypes.Where(v => v.IsRecommand).OrderBy(v => v.Rank).Select(v => v.TagTypeLimitName).FormatListToStr("|", 16), TreeSupport = Convert.ToInt32(boxtype.IsTreeSupported), Language = SimpleLanguageHandler.ProcessLang(boxtype.TypeName, boxtype.TypeNameEng) };
            _cache = query.ToPerfectDictionary(b => b.TypeID);
        }

        
    }
}
