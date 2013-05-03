using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Entity
{
    public enum DACPageType
    {
        COVER = 0, //封面
        RECOMMEND,//推荐
        VOD,//频道
        RANK,//排行
        SEARCH,//搜索
        History,//最近观看
        FAV,//收藏
        TraceProgram,//追剧
        DETAILPAGE,
        LOCALPLAY
    }
}
