using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Impl.Utils
{
    public static class ImplUtils
    {
        public static int ConvertHot(int source)
        {
            if (source <= 100)
                return 0;
            else if (source > 100 && source <= 200)
                return 1;
            else if (source > 200 && source <= 500)
                return 2;
            else if (source > 500 && source <= 1000)
                return 3;
            else if (source > 1000 && source <= 2500)
                return 4;
            return 5;
        }

        public static string SlogImageJoin(string[] source)
        {
            if (source == null)
                return string.Empty;
            var builder = new StringBuilder(source[0].Length + source[1].Length + source[2].Length + source[3].Length);
            builder.Append(source[0]);
            builder.Append(source[1]);
            builder.Append(source[2]);
            builder.Append(source[3]);
            return builder.ToString();
        }
    }
}
