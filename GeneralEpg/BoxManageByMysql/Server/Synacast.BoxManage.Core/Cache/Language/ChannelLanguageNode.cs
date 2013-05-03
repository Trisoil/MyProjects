using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Language
{
    using Synacast.BoxManage.Core.Utils;

    /// <summary>
    /// 频道语言包
    /// </summary>
    [Serializable]
    public class ChannelLanguageNode
    {
        /// <summary>
        /// 频道名称
        /// </summary>
        public virtual string ChannelName { get; set; }

        /// <summary>
        /// 频道副名称
        /// </summary>
        public virtual string SubTitle { get; set; }

        /// <summary>
        /// 频道码流率标识（极速，清晰，高清，蓝光等）
        /// </summary>
        public virtual string Mark { get; set; }

        /// <summary>
        /// 分集名字最大字节数
        /// </summary>
        public virtual int MaxBytes { get; set; }

        /// <summary>
        /// 分集名字最大字符数
        /// </summary>
        public virtual int MaxChars { get; set; }

        /// <summary>
        /// 频道分类
        /// </summary>
        public virtual string CatalogTags { get; set; }

        /// <summary>
        /// 频道地区
        /// </summary>
        public virtual string AreaTags { get; set; }

        /// <summary>
        /// 导演
        /// </summary>
        public virtual string Directors { get; set; }

        /// <summary>
        /// 演员
        /// </summary>
        public virtual string Actors { get; set; }

        /// <summary>
        /// 频道介绍
        /// </summary>
        public virtual string Introduction { get; set; }

    }

    [Serializable]
    public class TraditionChannelNode : ChannelLanguageNode 
    {
        ChannelLanguageNode _simpleNode;

        public TraditionChannelNode(ChannelLanguageNode simpleNode)
        {
            _simpleNode = simpleNode;
        }

        public override string ChannelName
        {
            get
            {
                return LanguageUtils.FormatTraditionChinese(_simpleNode.ChannelName);
            }
        }

        public override string SubTitle
        {
            get
            {
                return LanguageUtils.FormatTraditionChinese(_simpleNode.SubTitle);
            }
        }

        public override string Mark
        {
            get
            {
                return LanguageUtils.FormatTraditionChinese(_simpleNode.Mark);
            }
        }

        public override string CatalogTags
        {
            get
            {
                return LanguageUtils.FormatTraditionChinese(_simpleNode.CatalogTags);
            }
        }

        public override string AreaTags
        {
            get
            {
                return LanguageUtils.FormatTraditionChinese(_simpleNode.AreaTags);
            }
        }

        public override string Directors
        {
            get
            {
                return LanguageUtils.FormatTraditionChinese(_simpleNode.Directors);
            }
        }

        public override string Actors
        {
            get
            {
                return LanguageUtils.FormatTraditionChinese(_simpleNode.Actors);
            }
        }

        public override string Introduction
        {
            get
            {
                return LanguageUtils.FormatTraditionChinese(_simpleNode.Introduction);
            }
        }
    }
}
