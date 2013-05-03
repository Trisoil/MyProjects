using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Custom
{
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;

    public class VideoNodeKeyArray
    {
        private Dictionary<VideoNodeKey, int> _list;
        private Dictionary<VideoNodeKey, bool> _forbidden;
        private static bool _inited = false;

        private VideoNodeKeyArray()
        { }

        public static readonly VideoNodeKeyArray Instance = new VideoNodeKeyArray();

        public Dictionary<VideoNodeKey, int> Items
        {
            get {
                return _list;
            }
        }

        public Dictionary<VideoNodeKey, bool> Forbidden
        {
            get {
                return _forbidden;
            }
        }

        public void Init()
        {
            if (_inited)
                return;
            var types = BoxTypeCache.Instance.Items.Values;
            var plats = PlatformUtils.FormatVodPlatform();
            var list = new Dictionary<VideoNodeKey, int>(types.Count * plats.Count + 1);
            var fb = new Dictionary<VideoNodeKey, bool>(plats.Count);
            int i = 0;
            foreach (var f in plats)
            {
                var key = new VideoNodeKey(f.PlatformName, 0, f.Licence);
                fb.Add(key, f.IsForbiddenSupport);
                list.Add(key, ++i);
                foreach (var type in types)
                {
                    list.Add(new VideoNodeKey(f.PlatformName, type.TypeID, f.Licence), ++i);
                }
            }
            list.Add(new VideoNodeKey(AuthNode.NoPlatFormName, 0, AuthNode.NoPlatFormLicence), ++i);
            _list = list;
            _forbidden = fb;
            _inited = true;
        }

        public void ForbiddenInit()
        {
            var plats = PlatformUtils.FormatVodPlatform();
            var fb = new Dictionary<VideoNodeKey, bool>(plats.Count);
            foreach (var f in plats)
            {
                var key = new VideoNodeKey(f.PlatformName, 0, f.Licence);
                fb.Add(key, f.IsForbiddenSupport);
            }
            _forbidden = fb;
        }

        public VideoNodeKey FindKey(string plat, int type, string auth)
        {
            return _list.Keys.FirstOrDefault(v => v.PlatForm == plat && v.Type == type && v.Auth == auth);
        }

        public void Reset()
        {
            _inited = false;
        }
    }
}
