using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher
{
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
    }
}
