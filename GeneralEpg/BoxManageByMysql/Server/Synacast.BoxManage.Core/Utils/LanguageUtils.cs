using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualBasic;

namespace Synacast.BoxManage.Core.Utils
{
    public class LanguageUtils
    {
        public static string FormatTraditionChinese(string chinese)
        {
            return Strings.StrConv(chinese, VbStrConv.TraditionalChinese, 0);
        }

        public static string FormatTagEnglisth(string source)
        {
            return null;   
        }
    }
}
