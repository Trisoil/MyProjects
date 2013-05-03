//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Synacast.BoxManage.Core.Cache.Custom
//{
//    using Synacast.BoxManage.Core.Cache.Internal;

//    /// <summary>
//    /// TagCache的Hash主键
//    /// </summary>
//    [Serializable]
//    public class TagNodeKey2
//    {
//        #region Constructor

//        public TagNodeKey2(int type, string dimension, string platForm, string auth)
//        {
//            Type = type;
//            Dimension = dimension;
//            PlatForm = platForm;
//            Auth = auth;
//        }

//        #endregion

//        /// <summary>
//        /// Tag所属类型：电影，电视剧等
//        /// </summary>
//        public int Type { get; set; }

//        /// <summary>
//        /// Tag分类：地域，年份等
//        /// </summary>
//        public string Dimension { get; set; }

//        /// <summary>
//        /// Tag所属平台：机顶盒，手机等
//        /// </summary>
//        public string PlatForm { get; set; }

//        public string Auth { get; set; }

//        public override bool Equals(object obj)
//        {
//            TagNodeKey2 key = obj as TagNodeKey2;
//            if (obj == null)
//                return false;
//            if (this == key)
//                return true;
//            AuthKey authkey = new AuthKey() { Auth = key.Auth, PlatForm = key.PlatForm };
//            if (PlatFormCache.Instance.AuthItems.ContainsKey(authkey))
//                return key.Type == Type && key.Dimension == Dimension && key.PlatForm == PlatForm && PlatFormCache.Instance.AuthItems[authkey] == Auth;
//            key.Auth = null;
//            key.PlatForm = authkey.PlatForm;
//            return key.Type == Type && key.Dimension == Dimension && key.PlatForm == PlatForm && key.Auth == Auth;
//        }

//        public override int GetHashCode()
//        {
//            return ToString().GetHashCode();
//        }

//        public override string ToString()
//        {
//            string platform;
//            if (!string.IsNullOrEmpty(PlatForm) && PlatFormCache.Instance.Alias != null && PlatFormCache.Instance.Alias.ContainsKey(PlatForm))
//                platform = PlatFormCache.Instance.Alias[PlatForm];
//            else
//                platform = PlatForm;
//            return string.Format("{0}--{1}--{2}", Type, Dimension, platform);
//        }

//    }
//}
