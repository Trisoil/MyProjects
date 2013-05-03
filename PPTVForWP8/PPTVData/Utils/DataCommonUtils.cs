using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;
using System.Diagnostics;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Windows;
using System.Windows.Media;

using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Popups;
using Windows.Storage.Search;

namespace PPTVData.Utils
{
    using PPTVData.Entity;
    using PPTVData.Factory;

    public static class DataCommonUtils
    {
        /// <summary>
        /// 应用程序启动时间
        /// </summary>
        public static DateTime AppStartTime { get; set; }

        public static async Task<bool> CheckFileExist(string fileName)
        {
            StorageFolder folder = ApplicationData.Current.LocalFolder;
            var files = await folder.GetFilesAsync();
            return files.Any(file => file.Name == fileName);//.FileName
        }

        /// <summary>
        /// 解密
        /// </summary>
        /// <param name="url"></param>
        /// <param name="key"></param>
        /// <returns></returns>
        public static string Decode(string url, string key)
        {
            byte[] bin = Convert.FromBase64String(url);
            byte[] byteKey = Encoding.UTF8.GetBytes(key);
            byte[] bout = new byte[bin.Length];
            for (int i = 0; i < bin.Length; i++)
                bout[i] = (byte)((int)bin[i] - (int)byteKey[i % byteKey.Length]);
            char[] utf8Chars = new char[Encoding.UTF8.GetCharCount(bout, 0, bout.Length)];
            Encoding.UTF8.GetChars(bout, 0, bout.Length, utf8Chars, 0);
            return new string(utf8Chars);
        }

        /// <summary>
        /// 加密
        /// </summary>
        /// <param name="url"></param>
        /// <param name="key"></param>
        /// <returns></returns>
        public static string Encode(string url, string key)
        {
            if (string.IsNullOrEmpty(url))
                return string.Empty;
            byte[] bin = Encoding.UTF8.GetBytes(url);
            byte[] byteKey = Encoding.UTF8.GetBytes(key);
            byte[] bout = new byte[bin.Length];
            for (int i = 0; i < bin.Length; i++)
                bout[i] = (byte)((int)bin[i] + (int)byteKey[i % byteKey.Length]);
            return Convert.ToBase64String(bout);
        }

        public static byte[] HexToBytes(string str)
        {
            if (str.Length == 0 || str.Length % 2 != 0)
                return new byte[0];

            byte[] buffer = new byte[str.Length / 2];
            char c;
            for (int bx = 0, sx = 0; bx < buffer.Length; ++bx, ++sx)
            {
                // Convert first half of byte
                c = str[sx];
                buffer[bx] = (byte)((c > '9' ? (c > 'Z' ? (c - 'a' + 10) : (c - 'A' + 10)) : (c - '0')) << 4);

                // Convert second half of byte
                c = str[++sx];
                buffer[bx] |= (byte)(c > '9' ? (c > 'Z' ? (c - 'a' + 10) : (c - 'A' + 10)) : (c - '0'));
            }

            return buffer;
        }

        /// <summary>
        /// 将频道ID转换成web网站url
        /// </summary>
        /// <param name="channelId"></param>
        /// <returns></returns>
        public static string ConvertToWebSite(int channelId)
        {
            var build = new StringBuilder(50);
            build.Append("http://v.pptv.com/show/");
            build.Append(WebSiteEncode(channelId, 0, 0, 0));
            build.Append(".html");
            return build.ToString();
        }

        /// <summary>
        /// 频道ID加密,用于生成对应网站url
        /// </summary>
        /// <param name="chnID"></param>
        /// <param name="setID"></param>
        /// <param name="cataID"></param>
        /// <param name="source"></param>
        /// <returns></returns>
        public static string WebSiteEncode(int chnID, int setID, short cataID, int source)
        {
            const string bkey = "p>c~hf";
            byte[] bytes = new byte[11];
            bytes[3] = (byte)((chnID & 0xff000000) >> 24);
            bytes[2] = (byte)((chnID & 0xff0000) >> 16);
            bytes[1] = (byte)((chnID & 0xff00) >> 8);
            bytes[0] = (byte)((chnID & 0xff));
            bytes[7] = (byte)((setID & 0xff000000) >> 24);
            bytes[6] = (byte)((setID & 0xff0000) >> 16);
            bytes[5] = (byte)((setID & 0xff00) >> 8);
            bytes[4] = (byte)((setID & 0xff));
            bytes[9] = (byte)((cataID & 0xff00) >> 8);
            bytes[8] = (byte)((cataID & 0xff));
            bytes[10] = (byte)((source & 0xff));
            for (int i = 1; i < 11; i++)
            {
                bytes[i] = (byte)(bytes[i] ^ bytes[i - 1]);
                bytes[i] += (byte)bkey[i % 6];
            }
            var str = Convert.ToBase64String(bytes);
            str = str.Replace("i", "ia");
            str = str.Replace("+", "ib");
            str = str.Replace("/", "ic");
            str = str.Replace("=", "");
            return str;
        }

        public static T FindChildOfType<T>(DependencyObject root) where T : class
        {
            var queue = new Queue<DependencyObject>();
            queue.Enqueue(root);
            while (queue.Count > 0)
            {
                DependencyObject current = queue.Dequeue();
                for (int i = VisualTreeHelper.GetChildrenCount(current) - 1; 0 <= i; i--)
                {
                    var child = VisualTreeHelper.GetChild(current, i);
                    var typedChild = child as T;
                    if (typedChild != null)
                    {
                        return typedChild;
                    }
                    queue.Enqueue(child);
                }
            }
            return null;
        }

        public static IEnumerable<T> FindChildren<T>(DependencyObject parent) where T : class
        {
            var count = VisualTreeHelper.GetChildrenCount(parent);
            if (count > 0)
            {
                for (var i = 0; i < count; i++)
                {
                    var child = VisualTreeHelper.GetChild(parent, i);
                    var t = child as T;
                    if (t != null)
                        yield return t;

                    var children = FindChildren<T>(child);
                    foreach (var item in children)
                        yield return item;
                }
            }
        }

        #region XML Serialize Helper

        /// <summary>
        /// 把对象序列化为XML 文件
        /// </summary>
        /// <typeparam name="T">对象类型</typeparam>
        /// <param name="t"></param>
        /// <param name="stream"></param>
        /// <returns></returns>
        private static async Task XMLSerialize<T>(T t, Stream stream)
        {
            var xml = new XmlSerializer(typeof(T));
            await Task.Run(() =>
            {
                xml.Serialize(stream, t);
            });
        }

        /// <summary>
        /// 把 XML 文件反序列化为对象
        /// </summary>
        /// <typeparam name="T">对象类型</typeparam>
        /// <param name="stream"></param>
        /// <returns></returns>
        private static async Task<T> XMLDeserialize<T>(Stream stream) where T : class, new()
        {
            try
            {
                var xml = new XmlSerializer(typeof(T));
                var temp = await Task.Run<T>(() => { return xml.Deserialize(stream) as T; });
                return temp;
            }
            catch (InvalidOperationException)
            {
                return new T();
            }
        }

        #endregion

        #region StorageHelper

        private static async Task<bool> CheckFileExist(StorageFolder folder, string fileName)
        {
            try
            {
                var files = await folder.GetFileAsync(fileName);
                return true;
            }
            catch (FileNotFoundException)
            {
                return false;
            }
        }

        /// <summary>
        /// 创建实体到 LocalFolder中
        /// 若本地中已经存在则替换
        /// </summary>
        /// <typeparam name="T">数据对象</typeparam>
        /// <param name="t"></param>
        /// <param name="fileName">文件名称</param>
        /// <returns></returns>
        public static async Task CreatEntity<T>(T t, string fileName)
        {
            var folder = ApplicationData.Current.LocalFolder;
            var file = await folder.CreateFileAsync(fileName, CreationCollisionOption.ReplaceExisting);
            using (var randomAccessSteam = await file.OpenAsync(FileAccessMode.ReadWrite))
            using (var outputStream = randomAccessSteam.GetOutputStreamAt(0))
            using (var stream = outputStream.AsStreamForWrite())
            {
                await XMLSerialize(t, stream);
            }
        }

        /// <summary>
        /// 取出文件保存的对象
        /// </summary>
        /// <typeparam name="TResult">返回类型</typeparam>
        /// <param name="fileName">文件名称</param>
        /// <returns></returns>
        public static async Task<TResult> GetContent<TResult>(string fileName) where TResult : class, new()
        {
            var folder = ApplicationData.Current.LocalFolder;
            if (await CheckFileExist(folder, fileName))
            {
                var file = await folder.GetFileAsync(fileName);
                using (var randomAccessStream = await file.OpenAsync(FileAccessMode.Read))
                using (var inPutStream = randomAccessStream.GetInputStreamAt(0))
                using (var stream = inPutStream.AsStreamForRead())
                {
                    return await XMLDeserialize<TResult>(stream);
                }
            }
            else
            {
                var result = new TResult();
                await CreatEntity(result, fileName);
                return result;
            }
        }

        /// <summary>
        /// 删除 LocalFolder中的数据
        /// </summary>
        /// <param name="fileNmae">文件名称</param>
        /// <returns></returns>
        public static async Task<bool> DeleteEntity(string fileNmae)
        {
            var folder = ApplicationData.Current.LocalFolder;
            if (await CheckFileExist(folder, fileNmae))
            {
                var files = await folder.GetFileAsync(fileNmae);
                await files.DeleteAsync();
            }
            return true;
        }

        #endregion

    }

}
