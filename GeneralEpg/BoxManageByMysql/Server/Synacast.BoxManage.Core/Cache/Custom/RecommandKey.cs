using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Custom
{
    using Synacast.BoxManage.Core.Cache.Internal;

    /// <summary>
    /// 推荐导航Hash键
    /// </summary>
    [Serializable]
    public class RecommandKey
    {
        public RecommandKey(int mode, string plat, string auth)
        {
            Mode = mode;
            PlatForm = plat;
            Auth = auth;
        }
        public RecommandKey(string plat, string auth, int navid)
        {
            PlatForm = plat;
            Auth = auth;
            NavId = navid;
        }

        public int Mode { get; set; }

        public string PlatForm { get; set; }

        public string Auth { get; set; }

        public int NavId { get; set; }

        public override bool Equals(object obj)
        {
            if (obj == this) return true;
            RecommandKey key = obj as RecommandKey;
            if (key == null) return false;
            AuthKey authkey = new AuthKey() { Auth = key.Auth, PlatForm = key.PlatForm };
            if (PlatFormCache.Instance.AuthItems.ContainsKey(authkey))
                return key.PlatForm == PlatForm && key.Mode == Mode && key.NavId == NavId && PlatFormCache.Instance.AuthItems[authkey] == Auth;
            key.Auth = null;
            key.PlatForm = authkey.PlatForm;
            return key.PlatForm == PlatForm && key.Mode == Mode && key.NavId == NavId && key.Auth == Auth;
        }

        public override int GetHashCode()
        {
            return ToString().GetHashCode();
        }

        public override string ToString()
        {
            string platform;
            if (!string.IsNullOrEmpty(PlatForm) && PlatFormCache.Instance.Alias != null && PlatFormCache.Instance.Alias.ContainsKey(PlatForm))
                platform = PlatFormCache.Instance.Alias[PlatForm];
            else
                platform = PlatForm;
            return string.Format("{0}-{1}-{2}", Mode, platform, NavId);
        }
    }
}
