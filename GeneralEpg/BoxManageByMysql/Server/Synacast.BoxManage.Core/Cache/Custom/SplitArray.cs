using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Custom
{
    public static class SplitArray
    {
        /// <summary>
        /// 逗号
        /// </summary>
        public static readonly string[] DHArray = new string[] { "," };

        /// <summary>
        /// 空格
        /// </summary>
        public static readonly string[] BlankArray = new string[] { " " };

        /// <summary>
        /// 竖线
        /// </summary>
        public static readonly string[] LineArray = new string[] { "|" };

        /// <summary>
        /// 竖线
        /// </summary>
        public static readonly string Line = "|";

        public static readonly string DH = ",";

        public static readonly string IndexFlag = "superyp";

        public static readonly string IndexCnName = "supercnyp";
    }
}
