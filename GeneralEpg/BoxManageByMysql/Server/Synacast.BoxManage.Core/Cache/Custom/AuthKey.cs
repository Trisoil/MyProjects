using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Custom
{
    [Serializable]
    public class AuthKey
    {
        private string _platform;

        public string Auth { get; set; }

        public string PlatForm
        {
            get {
                if (!string.IsNullOrEmpty(_platform) && PlatFormCache.Instance.Alias != null && PlatFormCache.Instance.Alias.ContainsKey(_platform))
                    return PlatFormCache.Instance.Alias[_platform];
                return _platform;
            }
            set {
                _platform = value;
            }
        }

        public override bool Equals(object obj)
        {
            AuthKey o = obj as AuthKey;
            if (o == this) return true;
            if (o == null) return false;
            return o.Auth == Auth && o.PlatForm == PlatForm;
        }

        public override int GetHashCode()
        {
            return string.Format("{0}authkey{1}", Auth, PlatForm).GetHashCode();
        }
    }
}
