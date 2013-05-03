using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Utils
{
    using Synacast.LuceneNetSearcher.Spell;
    public class SpellUtils
    {
        public static string GetChineseSpell(string strText)
        {
            int len = strText.Length;
            var myStr = new StringBuilder(len);
            for (int i = 0; i < len; i++)
            {
                myStr.Append(getSpell(strText.Substring(i, 1)));
            }
            return myStr.ToString();
        }
        public static string getSpell(string cnChar)
        {
            byte[] arrCN = Encoding.Default.GetBytes(cnChar);
            if (arrCN.Length > 1)
            {
                int area = (short)arrCN[0];
                int pos = (short)arrCN[1];
                int code = (area << 8) + pos;
                int[] areacode = {45217,45253,45761,46318,46826,47010,47297,47614,48119,48119,49062,
49324,49896,50371,50614,50622,50906,51387,51446,52218,52698,52698,52698,52980,53689,
54481};
                for (int i = 0; i < 26; i++)
                {
                    int max = 55290;
                    if (i != 25) max = areacode[i + 1];
                    if (areacode[i] <= code && code < max)
                    {
                        return Encoding.Default.GetString(new byte[] { (byte)(65 + i) });
                    }
                }
                return "*";
            }
            else return cnChar;
        }

        public static List<string> GetChineseFrist(string strText)
        {
            var c = strText[0];
            var r = SpellContainer.Instance[c];
            if (r != null)
            {
                return r.FirstSpells;
            }
            var l = new List<string>(1);
            l.Add(c.ToString().ToLower());
            return l;
        }

        public static bool IsNumber(string word)
        {
            if (string.IsNullOrEmpty(word))
                return false;
            return System.Text.RegularExpressions.Regex.IsMatch(word, @"^[0-9]+$");
        }
    }
}
