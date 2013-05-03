using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Custom
{
    using Synacast.BoxManage.Core.Cache.Internal;
    /// <summary>
    /// ListCache的Hash键
    /// </summary>
    [Serializable]
    public class VideoNodeKey
    {
        #region Constructor

        public VideoNodeKey(string plat, int type, string auth)
        {
            PlatForm = plat;
            Type = type;
            Auth = auth;
        }

        #endregion

        /// <summary>
        /// 频道所属平台：机顶盒，手机等
        /// </summary>
        public string PlatForm { get; set; }

        /// <summary>
        /// 频道所属类型：电影，电视剧等
        /// </summary>
        public int Type { get; set; }

        /// <summary>
        /// 身份验证
        /// </summary>
        public string Auth { get; set; }

        public override bool Equals(object obj)
        {
            VideoNodeKey key = obj as VideoNodeKey;
            if (key == this) return true;
            if (key == null) return false;
            AuthKey authkey = new AuthKey() { Auth = key.Auth, PlatForm = key.PlatForm };
            if (PlatFormCache.Instance.AuthItems.ContainsKey(authkey))
                return key.PlatForm == PlatForm && key.Type == Type && PlatFormCache.Instance.AuthItems[authkey] == Auth;
            key.Auth = null;
            key.PlatForm = authkey.PlatForm;
            return key.PlatForm == PlatForm && key.Type == Type && key.Auth == Auth;
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
            return string.Format("{0}-{1}", platform, Type);
        }
    }

    #region Deleted
    //public enum VideoNodeKey
    //{
    //    /// <summary>
    //    /// 电影
    //    /// </summary>
    //    Movie = 1,

    //    /// <summary>
    //    /// 电视剧
    //    /// </summary>
    //    TelePlay,

    //    /// <summary>
    //    /// 动漫
    //    /// </summary>
    //    Cartoon,

    //    /// <summary>
    //    /// 综艺
    //    /// </summary>
    //    Shows,

    //    /// <summary>
    //    /// 体育
    //    /// </summary>
    //    Sports,

    //    /// <summary>
    //    /// 新闻
    //    /// </summary>
    //    News,

    //    /// <summary>
    //    /// 游戏
    //    /// </summary>
    //    Games,

    //    /// <summary>
    //    /// 手机通用
    //    /// </summary>
    //    Mobile,

    //    /// <summary>
    //    /// 只支持IPhone123
    //    /// </summary>
    //    IPhone123,

    //    /// <summary>
    //    /// 只支持Android
    //    /// </summary>
    //    Android,

    //    /// <summary>
    //    /// 只支持IPhone4
    //    /// </summary>
    //    IPhone4

    //}

    #endregion

}

