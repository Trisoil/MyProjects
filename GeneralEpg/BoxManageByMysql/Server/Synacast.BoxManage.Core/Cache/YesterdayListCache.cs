using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;

    public class YesterdayListCache 
    {
        #region Fields

        private List<YesterdayVideoNode> _cache;
        private static string _buffername = "YesterdayListCacheBuffer";
        public readonly static YesterdayListCache Instance = new YesterdayListCache();

        #endregion

        private YesterdayListCache()
        { }

        public void SerializeInit()
        {
            _cache = SerizlizerUtils<YesterdayVideoNode>.DeserializeSplitCache(_buffername);
        }

        public List<YesterdayVideoNode> Items
        {
            get
            {
                if (_cache == null || _cache.Count == 0)
                    _cache = SerizlizerUtils<YesterdayVideoNode>.DeserializeSplitCache(_buffername);
                if (_cache == null || _cache.Count == 0)
                    _cache = VideoUtils.ConvertVodList(ListCache.Instance.Items[new VideoNodeKey(null, 0, null)].Ver1);
                return _cache;
            }
        }

        public void Refresh(IEnumerable<int> videos)
        {
            _cache = VideoUtils.ConvertVodList(videos);
        }

    }
}
