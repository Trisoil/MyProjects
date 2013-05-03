using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Language
{
    using Synacast.BoxManage.Core.Utils;

    [Serializable]
    public class DoubleLanguageNode : LanguageNodeBase 
    {
        /// <summary>
        /// 主标题
        /// </summary>
        public virtual string Title { get; set; }

        /// <summary>
        /// 副标题
        /// </summary>
        public virtual string SubTitle { get; set; }
    }

    [Serializable]
    public class TraditionDoubleNode : DoubleLanguageNode
    {
        private DoubleLanguageNode _simpleNode;

        public TraditionDoubleNode(DoubleLanguageNode simpleNode)
        {
            _simpleNode = simpleNode;
        }

        public override string Title
        {
            get
            {
                return LanguageUtils.FormatTraditionChinese(_simpleNode.Title);
            }
        }

        public override string SubTitle
        {
            get
            {
                return LanguageUtils.FormatTraditionChinese(_simpleNode.SubTitle);
            }
        }
    }
}
