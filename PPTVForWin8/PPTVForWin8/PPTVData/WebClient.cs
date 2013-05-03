using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace PPTVData
{
    public class WebClient
    {
        HttpWebRequest _httpWebRequest;

        public WebClient()
        {
            ResponseHeaders = null;
        }

        public event DownloadStringCompletedEventHandler DownloadStringCompleted;

        public WebHeaderCollection ResponseHeaders { get; private set; }

        /// <summary>
        /// 取消HTTP请求
        /// </summary>
        public void CancelAsync()
        {
            if (_httpWebRequest != null)
            {
                _httpWebRequest.Abort();
                _httpWebRequest = null;
            }
        }

        /// <summary>
        /// Get请求
        /// </summary>
        /// <param name="uri"></param>
        public void DownloadStringAsync(string uri)
        {
            _httpWebRequest = HttpWebRequest.CreateHttp(uri);
            _httpWebRequest.BeginGetResponse(ResponseCallback, null);
        }

        /// <summary>
        /// Post请求，由于服务端实现，请求参数放在请求串后
        /// </summary>
        /// <param name="address"></param>
        /// <param name="data"></param>
        public void UploadStringAsync(string address, string data)
        {
            _httpWebRequest = (HttpWebRequest)HttpWebRequest.Create(string.Format("{0}?{1}", address, data));
            _httpWebRequest.Method = "POST";
            _httpWebRequest.BeginGetRequestStream(PostCallback, string.Empty);
        }

        /// <summary>
        /// Post请求消息体回调，暂时无信息写入消息体
        /// </summary>
        /// <param name="asyncResult"></param>
        private void PostCallback(IAsyncResult asyncResult)
        {
            try
            {
                var data = asyncResult.AsyncState as string;
                using (var stream = _httpWebRequest.EndGetRequestStream(asyncResult))
                using (var write = new StreamWriter(stream))
                {
                    write.Write(data);
                }
                _httpWebRequest.BeginGetResponse(ResponseCallback, null);
            }
            catch (WebException) { }
        }
        
        /// <summary>
        /// Response回调
        /// </summary>
        /// <param name="asynchronousResult"></param>
        private void ResponseCallback(IAsyncResult asynchronousResult)
        {
            try
            {
                using (var response = (HttpWebResponse)_httpWebRequest.EndGetResponse(asynchronousResult))
                using (var stream = response.GetResponseStream())
                {
                    ResponseHeaders = response.Headers;
                    if (DownloadStringCompleted != null)
                        DownloadStringCompleted(this, new HttpCompletedEventArgs(stream, false, null, asynchronousResult.AsyncState));
                }
            }
            catch (WebException) { }
        }
    }

    public class HttpCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs
    {
        public HttpCompletedEventArgs(Stream result, bool cancelled, Exception error, object userState)
            : base(error, cancelled, userState)
        {
            Result = result;
        }

        public Stream Result
        {
            get;
            private set;
        }
    }

    public delegate void DownloadStringCompletedEventHandler(Object sender, HttpCompletedEventArgs e);
}