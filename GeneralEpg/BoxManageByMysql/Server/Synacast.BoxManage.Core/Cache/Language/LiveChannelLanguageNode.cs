using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Language
{
    using Synacast.BoxManage.Core.Utils;

    [Serializable]
    public class LiveChannelLanguageNode
    {
        /// <summary>
        /// 频道名称
        /// </summary>
        public virtual string ChannelName { get; set; }

        /// <summary>
        /// 频道码流率标识（极速，清晰，高清，蓝光等）
        /// </summary>
        public virtual string Mark { get; set; }

        /// <summary>
        /// 频道介绍
        /// </summary>
        public virtual string Introduction { get; set; }
    }

    [Serializable]
    public class TraditionLiveChannelNode : LiveChannelLanguageNode 
    {
        private LiveChannelLanguageNode _simpleNode;

        public TraditionLiveChannelNode(LiveChannelLanguageNode simpleNode)
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

        public override string Mark
        {
            get
            {
                return LanguageUtils.FormatTraditionChinese(_simpleNode.Mark);
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
