using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    /// <summary>
    /// 代码一个平台：机顶盒，手机等
    /// </summary>
    [Serializable]
    public class AuthNode 
    {
        #region Fields

        private static string[] _splitword = new string[] { "," };
        private string _name;
        public static readonly string NoPlatFormName = "noflatformkey";
        public static readonly string NoPlatFormLicence = null;

        #endregion

        public string PlatformId { get; set; }

        /// <summary>
        /// 平台名，null代表机顶盒
        /// </summary>
        public string PlatformName
        {
            get
            {
                if (string.IsNullOrEmpty(_name))
                    return null;
                return _name;
            }
            set
            {
                _name = value.ToLower(System.Globalization.CultureInfo.CurrentCulture);
            }
        }

        /// <summary>
        /// 该平台应过滤包含字符的频道名
        /// </summary>
        public string NotContainsNames { get; set; }

        /// <summary>
        /// 该平台输出频道名最后应过滤的字符串
        /// </summary>
        public string FormateName { get; set; }

        /// <summary>
        /// 该平台输出频道名替换的字符串
        /// </summary>
        public string ReplaceName { get; set; }

        /// <summary>
        /// 视频图片输出大小格式化
        /// </summary>
        public string FormateImage { get; set; }

        /// <summary>
        /// 该平台是否显示隐藏频道
        /// </summary>
        public bool IsVisable { get; set; }

        /// <summary>
        /// 身份验证信息
        /// </summary>
        public string Licence { get; set; }

        /// <summary>
        /// 所需建立LuceneNet索引的类型
        /// </summary>
        public string SearchTypeIndex { get; set; }

        /// <summary>
        /// 所需建立LuceneNet索引的类型
        /// </summary>
        public int[] SearchTypeIndexList
        {
            get {
                return SearchTypeIndex.Split(_splitword, StringSplitOptions.RemoveEmptyEntries).Select(v => int.Parse(v)).ToArray();
            }
        }

        /// <summary>
        /// 是否支持屏蔽
        /// </summary>
        public bool IsForbiddenSupport { get; set; }

        public override bool Equals(object obj)
        {
            AuthNode o = obj as AuthNode;
            if (this == o) return true;
            if (o == null) return false;
            return o.PlatformName == PlatformName && o.IsVisable == IsVisable && o.NotContainsNames == NotContainsNames;
        }

        public override int GetHashCode()
        {
            return string.Format("{0}-{1}", PlatformName, IsVisable).GetHashCode();
        }
    }
}
