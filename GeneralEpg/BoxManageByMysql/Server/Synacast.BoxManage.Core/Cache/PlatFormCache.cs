using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    #region Namespaces

    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.ServicesFramework.Reflection;
    using Synacast.BoxManage.Core.Utils;

    #endregion

    public class PlatFormCache : CacheBase<List<AuthNode>>
    {
        #region Fields

        private static string[] _splictword = new string[] { "," };
        private static string[] _attrNames = new string[] { "PlatformName", "FormateImage", "FormateName", "ReplaceName", "NotContainsNames", "SearchTypeIndex" };
        private static string[] _attrBoolNames = new string[] { "IsVisable", "IsForbiddenSupport" };
        private Dictionary<AuthKey, string> _auth;
        private Dictionary<string, string> _alias;
        public static readonly PlatFormCache Instance = new PlatFormCache();

        private PlatFormCache()
        { }

        /// <summary>
        /// 重载平台行为的Auth特例
        /// </summary>
        public Dictionary<AuthKey, string> AuthItems
        {
            get
            {
                return _auth;
            }
        }

        public Dictionary<string, string> Alias
        {
            get {
                return _alias;
            }
        }

        #endregion

        public override void SerializeInit()
        {
            //base.SerializeInit();
            //_auth = SerizlizerUtils<Dictionary<AuthKey, string>>.DeserializeLastCache("PlatFormCacheAuthBuffer");
            RefreshCache();
        }

        public override void RefreshExtension()
        {
            //base.RefreshExtension();
            //SerizlizerUtils<Dictionary<AuthKey, string>>.SerializeLastCache(_auth, "PlatFormCacheAuthBuffer");
        }

        public override void RefreshCache()
        {
            var authkeys = new Dictionary<AuthKey, string>(AuthenticationCache.Instance.Items.Count);
            var alias = new Dictionary<string, string>();
            var list = new List<AuthNode>(PlatFormInfoCache.Instance.Items.Count + AuthenticationCache.Instance.Items.Count);
            var query = from platform in PlatFormInfoCache.Instance.Items join auth in AuthenticationCache.Instance.Items on platform.PlatformID equals auth.PlatformID into auths select FormatPlatForm(platform, auths, authkeys, alias);
            foreach (var auths in query)
            {
                list.AddRange(auths);
            }
            _auth = authkeys;
            _alias = alias;
            _cache = list;
        }

        private List<AuthNode> FormatPlatForm(PlatFormInfo platform, IEnumerable<AuthenticationInfo> auths, Dictionary<AuthKey, string> authKeys, Dictionary<string,string> alias)
        {
            var list = new List<AuthNode>(auths.Count() + 1);
            var node = new AuthNode();
            node.PlatformId = platform.PlatformID.ToString();
            node.Licence = null;
            foreach (string attrName in _attrNames)
                FormatStrAttr(platform, node, attrName);
            foreach (string attrName in _attrBoolNames)
                FormatBoolAttr(platform, node, attrName);
            list.Add(node);
            if (!string.IsNullOrEmpty(platform.Alias))
            {
                var pas = platform.Alias.Split(_splictword, StringSplitOptions.RemoveEmptyEntries);
                foreach (var alia in pas)
                    alias.Add(alia, node.PlatformName);
            }
            foreach (var auth in auths)
                FormatAuth(platform, auth, list, authKeys);
            return list;
        }

        private void FormatAuth(PlatFormInfo platform, AuthenticationInfo auth, List<AuthNode> list, Dictionary<AuthKey, string> authKeys)
        {
            var node = new AuthNode();
            var licences = auth.AuthLicence.Split(_splictword, StringSplitOptions.RemoveEmptyEntries);
            node.PlatformId = platform.PlatformID.ToString();
            node.Licence = auth.AuthKey;
            foreach (string attrName in _attrNames)
                FormatStrAttr(platform, auth, node, attrName);
            foreach (string attrName in _attrBoolNames)
                FormatBoolAttr(platform, auth, node, attrName);
            list.Add(node);
            foreach (var licence in licences)
            {
                authKeys.Add(new AuthKey() { Auth = licence, PlatForm = node.PlatformName }, node.Licence);
            }
        }

        private void FormatStrAttr(PlatFormInfo platform, AuthNode node, string attrName)
        {
            var attrValue = ReflectionOptimizer.GetProperty<PlatFormInfo, string>(platform, attrName);
            ReflectionOptimizer.SetProperty<AuthNode, string>(node, attrValue, attrName);
        }

        private void FormatStrAttr(PlatFormInfo platform, AuthenticationInfo auth, AuthNode node, string attrName)
        {
            var attrValue = ReflectionOptimizer.GetProperty<AuthenticationInfo, string>(auth, attrName);
            if (string.IsNullOrEmpty(attrValue))
            {
                attrValue = ReflectionOptimizer.GetProperty<PlatFormInfo, string>(platform, attrName);
            }
            ReflectionOptimizer.SetProperty<AuthNode, string>(node, attrValue, attrName);
        }

        private void FormatBoolAttr(PlatFormInfo platform, AuthNode node, string attrName)
        {
            bool value;
            if (!string.IsNullOrEmpty(node.PlatformName) && AppSettingCache.Instance.ContainerKey(node.PlatformName) && string.Compare(attrName, AppSettingCache.Instance[node.PlatformName]) == 0)
            {
                value = true;
            }
            else
            {
                var attrValue = ReflectionOptimizer.GetProperty<PlatFormInfo, string>(platform, attrName);
                value = Convert.ToBoolean(int.Parse(attrValue));
            }
            ReflectionOptimizer.SetProperty<AuthNode, bool>(node, value, attrName);
        }

        private void FormatBoolAttr(PlatFormInfo platform, AuthenticationInfo auth, AuthNode node, string attrName)
        {
            bool value;
            if (!string.IsNullOrEmpty(node.PlatformName) && AppSettingCache.Instance.ContainerKey(node.PlatformName) && string.Compare(attrName, AppSettingCache.Instance[node.PlatformName]) == 0)
            {
                value = true;
            }
            else
            {
                var attrValue = ReflectionOptimizer.GetProperty<AuthenticationInfo, string>(auth, attrName);
                if (string.IsNullOrEmpty(attrValue))
                {
                    attrValue = ReflectionOptimizer.GetProperty<PlatFormInfo, string>(platform, attrName);
                }
                value = Convert.ToBoolean(int.Parse(attrValue));
            }
            ReflectionOptimizer.SetProperty<AuthNode, bool>(node, value, attrName);
        }
    }
}
