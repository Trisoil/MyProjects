using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Entity
{
    /// <summary>
    /// 搜索EPG序列化类型
    /// </summary>
    public class ChannelSearchInfo
    {
        public List<ChannelSearchNavInfo> Navs { get; set; }
        public List<ChannelSearchListInfo> Lists { get; set; }
        public ChannelSearchOutlineInfo OutlineInfo { get; set; }
    }

    /// <summary>
    /// 搜索结果概括信息
    /// </summary>
    public class ChannelSearchOutlineInfo
    {
        public int Count { get; set; }//总数
        public int PageCount { get; set; }//页数
        public int CountInPage { get; set; }
        public int CurrentPage { get; set; }//当前页 从1开始
    }

    /// <summary>
    /// 搜索分类结果类型
    /// </summary>
    public class ChannelSearchNavInfo
    {
        public int Count { get; set; }

        public string Name { get; set; }

        public int Id { get; set; }
    }

    /// <summary>
    /// 搜索频道结果类型
    /// </summary>
    public class ChannelSearchListInfo
    {
        /// <summary>
        /// 频道ID
        /// </summary>
        public int Vid { get; set; }

        /// <summary>
        /// 频道名称
        /// </summary>
        public string Text { get; set; }

        /// <summary>
        /// 频道缩略图
        /// </summary>
        public string ImgSource { get; set; }

        public double Duration { get; set; }

        public string CatalogTags { get; set; }
    }
}
