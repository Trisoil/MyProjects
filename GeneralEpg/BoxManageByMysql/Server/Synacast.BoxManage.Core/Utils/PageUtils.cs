using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Utils
{
    public class PageUtils
    {
        /// <summary>
        /// 获取分页个数
        /// </summary>
        /// <param name="all">记录总数</param>
        /// <param name="pagecount">每页个数</param>
        /// <returns>页数</returns>
        public static int PageCount(int all, int pageCount)
        {
            if (pageCount == 0)
                return 1;
            double pagec = Convert.ToDouble(pageCount);
            double pagecc = all / pagec;
            return Convert.ToInt32(Math.Ceiling(pagecc));
        }

        /// <summary>
        /// 获取当前页的第一条记录的索引
        /// </summary>
        /// <param name="pageCount">每页个数</param>
        /// <param name="page">当前页，从1开始</param>
        /// <returns>记录起始索引</returns>
        public static int PageIndex(int pageCount, int page)
        {
            return pageCount * (page - 1);
        }

    }
}
