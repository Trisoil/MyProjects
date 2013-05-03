using System;
using System.Net;
using System.Collections.Generic;
using System.Text;

namespace PPTVData.Utils
{
    public class HttpUtils
    {
        /// <summary>
        /// 格式化参数串为引用集合
        /// </summary>
        /// <param name="queryString"></param>
        /// <returns></returns>
        public static List<APIParameter> GetQueryParameters(string queryString)
        {
            if (queryString.StartsWith("?"))
            {
                queryString = queryString.Remove(0, 1);
            }

            List<APIParameter> result = new List<APIParameter>();

            if (!string.IsNullOrEmpty(queryString))
            {
                string[] p = queryString.Split('&');
                foreach (string s in p)
                {
                    if (!string.IsNullOrEmpty(s))
                    {
                        if (s.IndexOf('=') > -1)
                        {
                            string[] temp = s.Split('=');
                            result.Add(new APIParameter(temp[0], temp[1]));
                        }
                    }
                }
            }

            return result;
        }

        /// <summary>
        /// 格式化参数串为请求参数串
        /// </summary>
        /// <param name="paras"></param>
        /// <returns></returns>
        public static string GetQueryFromParas(List<APIParameter> paras)
        {
            if (paras == null || paras.Count == 0)
                return string.Empty;
            var sbList = new StringBuilder();
            int count = 1;
            foreach (APIParameter para in paras)
            {
                sbList.AppendFormat("{0}={1}", para.Name, para.Value);
                if (count < paras.Count)
                    sbList.Append("&");
                count++;
            }
            return sbList.ToString();
        }

        /// <summary>
        /// 不区分大小写,获得querysring中的值
        /// </summary>
        /// <param name="url"></param>
        /// <param name="key"></param>
        /// <returns></returns>
        public static string GetQuerystring(Uri url, string key)
        {
            string retVal = "";
            string abUrl = url.AbsoluteUri;
            string query = abUrl.Substring(abUrl.IndexOf("?") + 1);
            query = query.Replace("?", "");
            string[] querys = query.Split('&');
            foreach (string qu in querys)
            {
                string[] vals = qu.Split('=');
                if (vals[0].ToString().ToLower() == key.ToLower())
                {
                    retVal = vals[1].ToString();
                    break;
                }
            }
            return retVal;
        }

        /// <summary>
        /// MD5加密 WIN8中暂未实现
        /// </summary>
        /// <param name="plainText"></param>
        /// <returns></returns>
        //public static string MD5Encrpt(string plainText)
        //{
        //    MD5 md5Hash = MD5.Create();
        //    byte[] data = md5Hash.ComputeHash(Encoding.UTF8.GetBytes(plainText));
        //    StringBuilder sbList = new StringBuilder();
        //    foreach (byte d in data)
        //    {
        //        sbList.Append(d.ToString("x2"));
        //    }
        //    return sbList.ToString();
        //}
    }

    /// <summary>
    /// HTTP请求参数键值对
    /// </summary>
    public class APIParameter
    {
        private string name = null;
        private string value = null;

        public APIParameter(string name, string value)
        {
            this.name = name;
            this.value = value;
        }

        public string Name
        {
            get { return name; }
        }

        public string Value
        {
            get { return value; }
        }
    }

    /// <summary>
    /// 键值对比较，用于排序
    /// </summary>
    public class ParameterComparer : IComparer<APIParameter>
    {
        public int Compare(APIParameter x, APIParameter y)
        {
            if (x.Name == y.Name)
            {
                return string.Compare(x.Value, y.Value);
            }
            else
            {
                return string.Compare(x.Name, y.Name);
            }
        }
    }
}
