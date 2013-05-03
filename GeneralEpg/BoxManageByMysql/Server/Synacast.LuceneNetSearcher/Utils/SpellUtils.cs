using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace Synacast.LuceneNetSearcher.Utils
{
    using PanGu;
    using Synacast.LuceneNetSearcher.Cache;
    using Synacast.LuceneNetSearcher.Spell;

    public class SpellUtils
    {
        /// <summary>
        /// 根据中文分词返回拼音与首字母
        /// </summary>
        public static List<string> GetSpellSegment(string word, bool isAll, bool isFirst)
        {
            if (IsNumOrWord(word))
            {
                var list = new List<string>(1);
                list.Add(word);
            }
            var en = FormatEnWord(word);
            var cn = FormatCnWord(word);
            var rootstr = "root";
            if (!string.IsNullOrEmpty(en))
                rootstr = en;
            var parents = new List<SpellNode>();
            var root = new SpellNode(isAll, isFirst) { AllSpell = rootstr, FirstSpell = rootstr };
            parents.Add(root);
            for (int i = 0; i < cn.Length; i++)
            {
                var ps = new List<SpellNode>();
                var n = cn[i];
                var node = SpellContainer.Instance[n];
                if (node != null)
                {
                    for (int j = 0; j < node.AllSpells.Count; j++)
                    {
                        foreach (var parent in parents)
                        {
                            var c = new SpellNode(isAll, isFirst) { AllSpell = node.AllSpells[j], FirstSpell = node.FirstSpells[j], Parent = parent };
                            parent.AddChild(c);
                            ps.Add(c);
                        }
                    }
                    parents = ps;
                }
                else
                {
                    foreach (var parent in parents)
                    {
                        parent.EditSpell(n);
                    }
                }
            }
            var result = new List<string>();
            LoadChildren(root, result, isAll, isFirst);
            return result.Distinct().ToList();
        }

        /// <summary>
        /// 根据中文分词返回全拼与拼音首字母
        /// </summary>
        public static List<string> GetSpellSegment(string word)
        {
            if (!IsChinese(word))
                return null;
            var rootstr = "root";
            var parents = new List<SpellNode>();
            var isAll = true;
            var isFirst = true;
            var root = new SpellNode(isAll, isFirst) { AllSpell = rootstr, FirstSpell = rootstr };
            parents.Add(root);
            for (int i = 0; i < word.Length; i++)
            {
                var ps = new List<SpellNode>();
                var node = SpellContainer.Instance[word[i]];
                if (node == null)
                    return null;
                for (int j = 0; j < node.AllSpells.Count; j++)
                {
                    foreach (var parent in parents)
                    {
                        var c = new SpellNode(isAll, isFirst) { AllSpell = node.AllSpells[j], FirstSpell = node.FirstSpells[j], Parent = parent };
                        parent.AddChild(c);
                        ps.Add(c);
                    }
                }
                parents = ps;

            }
            var result = new List<string>();
            LoadChildren(root, result, isAll, isFirst);
            return result.Distinct().ToList();
        }

        private static void LoadChildren(SpellNode parent, List<string> result, bool isAll, bool isFirst)
        {
            if (parent.Chindren != null)
            {
                foreach (var child in parent.Chindren)
                {
                    LoadChildren(child, result, isAll, isFirst);
                }
            }
            else
            {
                if (isAll)
                    result.Add(parent.AllSpell);
                if (isFirst)
                    result.Add(parent.FirstSpell);
            }
        }

        /// <summary>
        /// 拼音首字母分词
        /// </summary>
        public static List<WordInfo> GetCnSegment(string word)
        {
            //if (word.Length > SettingCache.MaxIndex)
            //    return null;
            string all = word;
            var result = new List<WordInfo>();
            var list = FormatFirstWord(word);
            if (list.Count <= 0)
                return null;
            if (list.Count > 1)
            {
                var builder = new StringBuilder(10);
                bool flag = false;
                for (int i = 0; i < list.Count; i++)
                {
                    if (IsChinese(list[i]))
                    {
                        builder.Append(list[i]);
                        if (flag)
                            continue;
                        var cns = GetSpellSegment(list[i], false, true);
                        if (cns != null)
                        {
                            flag = true;
                            foreach (var cn in cns)
                            {
                                var n = cn;
                                if (i != 0)
                                    n = string.Format("{0}{1}", cn, word.Replace(list[i], ""));
                                AddIndex(result, n, i, SettingCache.MaxIndex - i);
                            }
                        }
                    }
                    else if (IsNumber(list[i]))
                    {
                        builder.Append(list[i]);
                    }
                    else
                    {
                        builder.Append(list[i][0]);
                    }
                }
                all = builder.ToString();
            }
            else
                all = list[0];
            var results = GetSpellSegment(all, false, true);
            if (results != null)
            {
                foreach (var r in results)
                    AddIndex(result, r, 0, SettingCache.MaxIndex);
            }
            return result;
        }

        private static void AddIndex(List<WordInfo> result, string index, int position, int rank)
        {
            var wordinfo = new WordInfo(index, position, POS.POS_D_N, 1.0, rank, WordType.None, WordType.None);
            result.Add(wordinfo);
        }

        /// <summary>
        /// 判断输入字符是否为汉字
        /// </summary>
        public static bool IsChinese(string word)
        {
            return Regex.IsMatch(word, @"^[\u4e00-\u9fa5]+$");
        }

        public static bool IsNumber(string word)
        {
            return System.Text.RegularExpressions.Regex.IsMatch(word, @"^[0-9]+$");
        }

        public static bool IsNumOrWord(string word)
        {
            return System.Text.RegularExpressions.Regex.IsMatch(word, @"^([0-9]|[a-z]|[A-Z])+$");
        }

        public static bool IsCnOrNum(string word)
        {
            return System.Text.RegularExpressions.Regex.IsMatch(word, @"^([0-9]|[\u4e00-\u9fa5])+$");
        }

        public static string FormatChineseText(string text)
        {
            if (text.Length <= SettingCache.MaxIndex)
                return Regex.Match(text, @"[a-z|A-Z]*[\u4e00-\u9fa5]+").Value;
            return string.Empty;
        }

        public static string FormatEnWord(string text)
        {
            return Regex.Match(text, "^[a-z|A-Z|0-9]*").Value;
        }

        public static string FormatCnWord(string text)
        {
            return Regex.Match(text, @"[\u4e00-\u9fa5]+.*$").Value;
        }

        public static List<string> FormatFirstWord(string text)
        {
            var matchs = Regex.Matches(text, "[a-z|A-Z|0-9]+|[\u4e00-\u9fa5]+");
            return matchs.OfType<Match>().Select(m => m.Value).ToPerfectList();
        }

    }

    internal class SpellNode
    {
        private List<SpellNode> _children;
        private StringBuilder _allspell;
        private StringBuilder _firstspell;

        internal SpellNode(bool isall,bool isfrist)
        {
            this.IsALL = isall;
            this.IsFirst = isfrist;
            if (IsALL)
                _allspell = new StringBuilder();
            if (IsFirst)
                _firstspell = new StringBuilder();
        }

        internal SpellNode Parent { get; set; }

        internal bool IsALL { get; set; }

        internal bool IsFirst { get; set; }

        internal string AllSpell
        {
            get {
                if (IsALL)
                {
                    return _allspell.ToString();
                }
                return null;
            }
            set {
                if (IsALL)
                {
                    _allspell.Append(value);
                   
                }
            }
        }

        internal string FirstSpell
        {
            get {
                if (IsFirst)
                {
                    return _firstspell.ToString();
                }
                return null;
            }
            set {
                if (IsFirst)
                {
                    _firstspell.Append(value);
                }
            }
        }

        internal List<SpellNode> Chindren
        {
            get
            {
                return _children;
            }
        }

        internal void EditSpell(char word)
        {
            if (IsFirst)
                _firstspell.Append(word);
            if (IsALL)
                _allspell.Append(word);
        }

        internal void InsertSpell(SpellNode parent)
        {
            if (parent.AllSpell  != "root" && IsALL)
                _allspell.Insert(0, Parent.AllSpell);
            if (parent.FirstSpell != "root" && IsFirst)
                _firstspell.Insert(0, parent.FirstSpell);
        }

        internal void AddChild(SpellNode child)
        {
            if (_children == null)
                _children = new List<SpellNode>();
            _children.Add(child);
            child.InsertSpell(this);
        }
    }
}
