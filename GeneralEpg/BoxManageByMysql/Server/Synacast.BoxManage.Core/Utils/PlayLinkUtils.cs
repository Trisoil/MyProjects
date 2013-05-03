using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web;

namespace Synacast.BoxManage.Core.Utils
{
    public class PlayLinkUtils
    {
        public static string MakePlayLink(string channelName)
        {
            string tmp = string.Format("jump.g1d.net||{0}", HttpUtility.UrlEncode(channelName, Encoding.GetEncoding("gb2312")).Replace("+", "%20"));

            return HttpUtility.UrlEncode(NewUrl(tmp, "utf-8"), Encoding.GetEncoding("gb2312"));
        }

        /// <summary>
        /// 返回经过playlink加密后的字符串
        /// </summary>
        /// <param name="text">要加密的源字符串</param>
        /// <param name="encoding">编码方式,如gb2312</param>
        /// <returns></returns>
        public static string NewUrl(string text, string encoding)
        {
            byte[] bin = Encoding.GetEncoding(encoding).GetBytes(text);
            byte[] byteKey = Encoding.GetEncoding(encoding).GetBytes("kioe257ds");//kioe257ds pplive
            byte[] bout = new byte[bin.Length];
            for (int i = 0; i < bin.Length; i++)
                bout[i] = (byte)((int)bin[i] + (int)byteKey[i % byteKey.Length]);
            return Convert.ToBase64String(bout);
        }

        /// <summary>
        /// 返回经过playlink解密后的字符串
        /// </summary>
        /// <param name="text">密文</param>
        /// <param name="encoding">编码方式,如gb2312</param>
        /// <returns></returns>
        public static string OldUrl(string text, string encoding)
        {
            byte[] bin = Convert.FromBase64String(text);
            byte[] byteKey = Encoding.GetEncoding(encoding).GetBytes("kioe257ds");//kioe257ds pplive
            byte[] bout = new byte[bin.Length];
            for (int i = 0; i < bin.Length; i++)
                bout[i] = (byte)((int)bin[i] - (int)byteKey[i % byteKey.Length]);
            return Encoding.GetEncoding(encoding).GetString(bout);
        }
    }
}
