using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Imaging;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows;
using System.Windows.Controls;

using PPTVData.Factory;

namespace PPTVData
{
    public class Utils
    {
        public static string _DACUrl = "http://ios.synacast.com/1.html?";
        public static UserStateInfo _userStateInfo;

        /// <summary>
        /// 设备唯一ID
        /// </summary>
        public static string DeviceUniqueId = CreateDeviceUniqueId();

        public static string[] Splite(string str, string spliteString)
        {
            int position = -1;
            string tempStr = str;
            List<string> list = new List<string>();
            while (true)
            {
                position = tempStr.IndexOf(spliteString);
                if (position == -1)
                {   // 没有读到
                    list.Add(tempStr);
                    break;
                }
                list.Add(tempStr.Substring(0, position));
                tempStr = tempStr.Substring(position + spliteString.Length);
            }
            return list.ToArray();
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

        /// <summary>
        /// 生成设备唯一ID
        /// </summary>
        /// <returns></returns>
        private static string CreateDeviceUniqueId()
        {
            object bytes = null;
            if (Microsoft.Phone.Info.DeviceExtendedProperties.TryGetValue("DeviceUniqueId", out bytes))
            {
                var buffer = (byte[])bytes;
                var build = new System.Text.StringBuilder(buffer.Length * 2);
                foreach (var b in buffer)
                {
                    build.Append(b.ToString("X"));
                }
                return build.ToString();
            }
            return string.Empty;
        }
    }
}
