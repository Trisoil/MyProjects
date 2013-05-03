using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.BoxManage.Core
{
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache;

    /// <summary>
    /// 拓展方法
    /// </summary>
    public static class ExtensionClass
    {
        /// <summary>
        /// 完美输出缓存
        /// </summary>
        public static List<T> ToPerfectList<T>(this IEnumerable<T> source)
        {
            var list = new List<T>(source.Count());
            using (var enumerator = source.GetEnumerator())
            {
                while (enumerator.MoveNext())
                {
                    list.Add(enumerator.Current);
                }
            }
            return list;
        }

        /// <summary>
        /// 完美生成字典
        /// </summary>
        public static Dictionary<T, V> ToPerfectDictionary<T, V>(this IEnumerable<V> source, Func<V, T> keySelector)
        {
            var dic = new Dictionary<T, V>(source.Count());
            foreach (var element in source)
            {
                dic[keySelector(element)] = element;
            }
            return dic;
        }

        /// <summary>
        /// 按指定字符格式化集合为字符串
        /// </summary>
        public static string FormatListToStr<T>(this IEnumerable<T> source, string split)
        {
            var builder = new StringBuilder(50);
            using (var enumertor = source.GetEnumerator())
            {
                while (enumertor.MoveNext())
                {
                    builder.Append(enumertor.Current);
                    builder.Append(split);
                }
            }
            if (builder.Length < 2)
                return string.Empty;
            builder.Remove(builder.Length - 1, 1);
            return builder.ToString();
        }

        /// <summary>
        /// 按指定字符格式化集合为字符串
        /// </summary>
        public static string FormatListToStr<T>(this IEnumerable<T> source, string split, int capacity)
        {
            var builder = new StringBuilder(capacity);
            using (var enumertor = source.GetEnumerator())
            {
                while (enumertor.MoveNext())
                {
                    builder.Append(enumertor.Current);
                    builder.Append(split);
                }
            }
            if (builder.Length < 2)
                return string.Empty;
            builder.Remove(builder.Length - 1, 1);
            return builder.ToString();
        }

        /// <summary>
        /// 格式化字符串为集合
        /// </summary>
        public static string[] FormatStrToArray(this string source,string[] split)
        {
            return source.Split(split, StringSplitOptions.RemoveEmptyEntries);
        }

        /// <summary>
        /// 格式化字符串为集合
        /// </summary>
        public static string[] FormatStrToArrayEmpty(this string source, string[] split)
        {
            return source.Split(split, StringSplitOptions.None);
        }

        public static List<int> FormatStrToInt(this string source, string[] split)
        {
            var array = source.Split(split, StringSplitOptions.RemoveEmptyEntries);
            var list = new List<int>(array.Length);
            foreach (var s in array)
            {
                int result;
                if (int.TryParse(s, out result))
                {
                    list.Add(result);
                }
            }
            return list;
        }

        public static void AddEnumerator<T>(this List<T> source, IEnumerable<T> enumerable)
        {
            using (var enumerator = enumerable.GetEnumerator())
            {
                while (enumerator.MoveNext())
                {
                    source.Add(enumerator.Current);
                }
            }
        }

        public static int BackPageList(this IEnumerable<int> source, int c, int s, XElement root)
        {
            var index = (s - 1) * c;
            var max = index + c;
            int num = 0;
            using (var enumerator = source.GetEnumerator())
            {
                while (enumerator.MoveNext())
                {
                    if (index <= num && num < max)
                    {
                        root.Add(ResponseUtils.ResponseBack(ListCache.Instance.Dictionary[enumerator.Current]));
                    }
                    num++;
                }
            }
            return num;
        }
    }
}
