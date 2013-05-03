using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Language;

    [Serializable]
    public class BoxTypeNode : LanguagePack<SimpleLanguageNode>
    {
        /// <summary>
        /// ID
        /// </summary>
        public int TypeID { get; set; }

        /// <summary>
        /// 大类图片地址
        /// </summary>
        public string PicLink { get; set; }

        /// <summary>
        /// 是否支持叶子索引
        /// </summary>
        public int TreeSupport { get; set; }

        /// <summary>
        /// 输出维度
        /// </summary>
        public string ResponseDimension { get; set; }
    }
}
