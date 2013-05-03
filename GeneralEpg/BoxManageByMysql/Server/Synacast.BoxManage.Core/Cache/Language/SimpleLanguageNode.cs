using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Synacast.BoxManage.Core.Utils;

namespace Synacast.BoxManage.Core.Cache.Language
{
    /// <summary>
    /// 基础语言包
    /// </summary>
    [Serializable]
    public class SimpleLanguageNode : LanguageNodeBase 
    {
        public virtual string Title { get; set; }
    }

    [Serializable]
    public class TraditionSimpleNode : SimpleLanguageNode
    {
        private SimpleLanguageNode _simpleNode;

        public TraditionSimpleNode(SimpleLanguageNode simpleNode)
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
    }
}
