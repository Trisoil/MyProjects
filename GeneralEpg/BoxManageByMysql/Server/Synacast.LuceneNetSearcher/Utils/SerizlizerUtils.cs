using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace Synacast.BoxManage.Core.Utils
{
    /// <summary>
    /// 序列化帮助类
    /// </summary>
    public class SerizlizerUtils<T> where T : class
    {
        /// <summary>
        /// 序列化内存为本地数据
        /// </summary>
        /// <param name="obj">内存中的数据</param>
        /// <param name="fileName">文件名，null为内存类型名</param>
        public static void SerializeLastCache(T obj, string fileName)
        {
            //XmlRootAttribute xt = new XmlRootAttribute("CacheRoot");
            //XmlSerializer formatter = new XmlSerializer(typeof(T), xt);
            BinaryFormatter formatter = new BinaryFormatter();
            string filePath = string.Format("{0}/Config/Buffer/{1}.boxcache", AppDomain.CurrentDomain.BaseDirectory, JudgeFileName(fileName));
            using (FileStream stream = new FileStream(filePath, FileMode.Create))
            {
                formatter.Serialize(stream, obj);
            }
        }

        /// <summary>
        /// 反序列化本地文件成内存数据
        /// </summary>
        /// <param name="fileName">本地数据文件名,null为类型名</param>
        /// <returns></returns>
        public static T DeserializeLastCache(string fileName)
        {
            //XmlRootAttribute xt = new XmlRootAttribute("CacheRoot");
            //XmlSerializer formatter = new XmlSerializer(typeof(T), xt);
            BinaryFormatter formatter = new BinaryFormatter();
            string filePath = string.Format("{0}/Config/Buffer/{1}.boxcache", AppDomain.CurrentDomain.BaseDirectory, JudgeFileName(fileName));
            if (File.Exists(filePath))
            {
                using (FileStream stream = new FileStream(filePath, FileMode.Open))
                {
                    return formatter.Deserialize(stream) as T;
                }
            }
            return null;
        }

        /// <summary>
        /// 拆分内存序列化为本地文件
        /// </summary>
        public static void SerializeSplitCache(Dictionary<int,T> dic, string bufferName)
        {
            int count = dic.Values.Count / 5;
            var buffer1 = SplitCache(dic, 0, count);
            var buffer2 = SplitCache(dic, count, count);
            var buffer3 = SplitCache(dic, count * 2, count);
            var buffer4 = SplitCache(dic, count * 3, count);
            var lastcount = dic.Values.Count - count * 4;
            var buffer5 = SplitCache(dic, count * 4, lastcount);

            //var buffer1 = list.Take(count).ToList();
            //var buffer2 = list.Skip(count).Take(count).ToList();
            //var buffer3 = list.Skip(count * 2).Take(count).ToList();
            //var buffer4 = list.Skip(count * 3).Take(count).ToList();
            //var buffer5 = list.Skip(count * 4).ToList();

            SerizlizerUtils<T[]>.SerializeLastCache(buffer1, string.Format("{0}1", bufferName));
            SerizlizerUtils<T[]>.SerializeLastCache(buffer2, string.Format("{0}2", bufferName));
            SerizlizerUtils<T[]>.SerializeLastCache(buffer3, string.Format("{0}3", bufferName));
            SerizlizerUtils<T[]>.SerializeLastCache(buffer4, string.Format("{0}4", bufferName));
            SerizlizerUtils<T[]>.SerializeLastCache(buffer5, string.Format("{0}5", bufferName));
        }

        private static T[] SplitCache(Dictionary<int, T> dic, int index, int count)
        {
            T[] list = new T[count];
            var max = index + count;
            for (int i = index; i < max; i++)
            {
                list[i - index] = dic.Values.ElementAt(i);
            }
            return list;
        }

        public static void SerializeSplitCache(List<T> list, string bufferName)
        {
            int count = list.Count / 5;
            var buffer1 = new T[count];
            list.CopyTo(0, buffer1, 0, count);
            var buffer2 = new T[count];
            list.CopyTo(count, buffer2, 0, count);
            var buffer3 = new T[count];
            list.CopyTo(count * 2, buffer3, 0, count);
            var buffer4 = new T[count];
            list.CopyTo(count * 3, buffer4, 0, count);
            var lastcount = list.Count - count * 4;
            var buffer5 = new T[lastcount];
            list.CopyTo(count * 4, buffer5, 0, lastcount);

            SerizlizerUtils<T[]>.SerializeLastCache(buffer1, string.Format("{0}1", bufferName));
            SerizlizerUtils<T[]>.SerializeLastCache(buffer2, string.Format("{0}2", bufferName));
            SerizlizerUtils<T[]>.SerializeLastCache(buffer3, string.Format("{0}3", bufferName));
            SerizlizerUtils<T[]>.SerializeLastCache(buffer4, string.Format("{0}4", bufferName));
            SerizlizerUtils<T[]>.SerializeLastCache(buffer5, string.Format("{0}5", bufferName));
        }

        /// <summary>
        /// 反序列化本地拆分文件为内存
        /// </summary>
        public static List<T> DeserializeSplitCache(string bufferName)
        {
            var buffer1 = SerizlizerUtils<T[]>.DeserializeLastCache(string.Format("{0}1", bufferName));
            var buffer2 = SerizlizerUtils<T[]>.DeserializeLastCache(string.Format("{0}2", bufferName));
            var buffer3 = SerizlizerUtils<T[]>.DeserializeLastCache(string.Format("{0}3", bufferName));
            var buffer4 = SerizlizerUtils<T[]>.DeserializeLastCache(string.Format("{0}4", bufferName));
            var buffer5 = SerizlizerUtils<T[]>.DeserializeLastCache(string.Format("{0}5", bufferName));
            if (buffer1 != null)
            {
                var list = new List<T>(buffer1.Length + buffer2.Length + buffer3.Length + buffer4.Length + buffer5.Length);
                list.AddRange(buffer1);
                list.AddRange(buffer2);
                list.AddRange(buffer3);
                list.AddRange(buffer4);
                list.AddRange(buffer5);
                return list;
            }
            return null;
        }

        private static string JudgeFileName(string fileName)
        {
            if (string.IsNullOrEmpty(fileName))
            {
                Type type = typeof(T);
                if (type.IsGenericType && type.GetGenericArguments().Length > 0)
                    return type.GetGenericArguments()[0].FullName;
                return type.ToString();
            }
            return fileName;
        }
    }
}
