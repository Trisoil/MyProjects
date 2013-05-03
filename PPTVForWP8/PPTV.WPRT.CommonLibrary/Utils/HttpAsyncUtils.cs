using System;
using System.IO;
using System.Net;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.Utils
{
    public delegate void HttpAsyncHandler(string responseData);

    /// <summary>
    /// Http异步请求帮助类
    /// </summary>
    public static class HttpAsyncUtils
    {
        /// <summary>
        /// GET请求方式
        /// </summary>
        /// <param name="url"></param>
        /// <param name="queryString"></param>
        /// <param name="callback"></param>
        /// <returns></returns>
        public static void HttpGet(string url, string queryString, string contentType, HttpAsyncHandler callback)
        {
            if (!string.IsNullOrEmpty(queryString))
            {
                url += "?" + queryString;
            }

            HttpWebRequest webRequest = WebRequest.Create(url) as HttpWebRequest;
            webRequest.Method = "GET";
            if (!string.IsNullOrEmpty(contentType))
                webRequest.ContentType = contentType;
            webRequest.BeginGetResponse(asyncResult =>
            {
                var request = asyncResult.AsyncState as HttpWebRequest;
                if (request != null)
                {
                    try
                    {
                        using (var response = request.EndGetResponse(asyncResult))
                        using (var reader = new System.IO.StreamReader(response.GetResponseStream()))
                        {
                            callback(reader.ReadToEnd());
                        }
                    }
                    catch (WebException ex)
                    {
                        callback(ex.Message);
                    }
                }
            }, webRequest);
        }

        /// <summary>
        /// POST请求方式
        /// </summary>
        /// <param name="url"></param>
        /// <param name="queryString"></param>
        /// <param name="callback"></param>
        public static void HttpPost(string url, string queryString, string contentType, HttpAsyncHandler callback)
        {
            HttpWebRequest webRequest = WebRequest.Create(url) as HttpWebRequest;
            webRequest.Method = "POST";
            if (!string.IsNullOrEmpty(contentType))
                webRequest.ContentType = contentType;
            webRequest.BeginGetRequestStream(requestPost_Callback, new RequestStat(webRequest, queryString, callback));
        }

        #region Private

        /// <summary>
        /// 异步POST请求流回调
        /// </summary>
        /// <param name="asynchronousResult"></param>
        private static void requestPost_Callback(IAsyncResult asynchronousResult)
        {
            var stat = asynchronousResult.AsyncState as RequestStat;
            if (stat != null)
            {
                HttpWebRequest webRequest = stat.Request;
                using (var stream = new StreamWriter(webRequest.EndGetRequestStream(asynchronousResult)))
                {
                    stream.Write(stat.Parameters);
                }
                webRequest.BeginGetResponse(asyncResult =>
                {
                    var request = asyncResult.AsyncState as WebRequest;
                    if (request != null)
                    {
                        try
                        {
                            using (var response = (HttpWebResponse)request.EndGetResponse(asyncResult))
                            using (var stream = new System.IO.StreamReader(response.GetResponseStream()))
                            {
                                stat.CallBack(stream.ReadToEnd());
                            }
                        }
                        catch (WebException ex)
                        {
                            stat.CallBack(ex.Message);
                        }
                    }
                }, webRequest);
            }
        }

        /// <summary>
        /// 异步请求传递状态
        /// </summary>
        private class RequestStat
        {
            public RequestStat(HttpWebRequest webRequest, string parameters, HttpAsyncHandler callBack)
            {
                Request = webRequest;
                Parameters = parameters;
                CallBack = callBack;
            }

            public HttpWebRequest Request;
            public HttpAsyncHandler CallBack;
            public string Parameters;
        }

        #endregion
    }
}
