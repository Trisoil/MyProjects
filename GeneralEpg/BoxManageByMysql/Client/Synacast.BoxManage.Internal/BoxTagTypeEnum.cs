using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Internal
{
    /// <summary>
    /// 分类类型枚举
    /// </summary>
    [Serializable]
    public enum BoxTagTypeEnum
    {
         /// <summary>
        /// 分类
        /// </summary>
        Catalog = 1,

        /// <summary>
        /// 地区
        /// </summary>
        Area = 2,

        /// <summary>
        /// 年份
        /// </summary>
        Year = 3,

        /// <summary>
        /// 演员
        /// </summary>
        Act = 4,

        /// <summary>
        /// 导演
        /// </summary>
        Director = 5
    }
}
