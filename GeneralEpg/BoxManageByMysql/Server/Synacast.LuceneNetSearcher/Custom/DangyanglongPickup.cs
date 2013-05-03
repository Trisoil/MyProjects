using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher.Custom
{
    using PanGu;
    using Synacast.LuceneNetSearcher.Utils;
    using Synacast.LuceneNetSearcher.Cache;

    public class DangyanglongPickup : ICustomRule 
    {
        private string _text;

        #region ICustomRule 成员

        public void AfterSegment(SuperLinkedList<WordInfo> result)
        {
            if (Text.Length > SettingCache.MaxIndex)
                return;
            var list = SpellUtils.FormatFirstWord(Text);
            var builder = new StringBuilder(10);
            for (int i = 0; i < list.Count; i++)
            {
                if (SpellUtils.IsCnOrNum(list[i]))
                {
                    builder.Append(list[i]);
                    if (list[i].Length < 2)
                        continue;
                    var cns = SpellUtils.GetSpellSegment(list[i], false, true);
                    if (cns != null)
                    {
                        foreach (var cn in cns)
                        {
                            var n = cn;
                            if (i != 0)
                                n = string.Format("{0}{1}", cn, Text.Replace(list[i], ""));
                            AddIndex(result, n, i, SettingCache.MaxIndex - i);
                        }
                    }
                }
                else
                {
                    builder.Append(list[i][0]);
                }
            }
            var results = SpellUtils.GetSpellSegment(builder.ToString(), false, true);
            if (results != null)
            {
                foreach (var r in results)
                    AddIndex(result, r, 0, SettingCache.MaxIndex);
            }
        }

        private void AddIndex(SuperLinkedList<WordInfo> result, string index, int position, int rank)
        {
            var wordinfo = new WordInfo(index, position, POS.POS_D_N, 1.0, rank, WordType.None, WordType.None);
            result.AddFirst(wordinfo);
        }

        public string Text
        {
            get
            {
                return _text;
            }
            set
            {
                _text = value;
            }
        }

        #endregion
    }
}
