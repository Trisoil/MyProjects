using System;
using System.Net;
using System.Text;
using System.IO;
using System.Collections.Generic;

namespace PPTVData
{
    public class PPTVWaitHandle : System.Threading.WaitHandle
    {
        public PPTVWaitHandle()
            : base()
        {
        }
    }

    public class DownloadStringCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs
    {
        public DownloadStringCompletedEventArgs(string result, bool cancelled, Exception error, object userState)
            : base(error, cancelled, userState)
        {
            Result = result;
        }
        public string Result
        {
            get;
            private set;
        }
    }

    public class OpenReadCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs
    {
        public OpenReadCompletedEventArgs(Stream result,bool cancelled,Exception error,object userState)
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

    public delegate void DownloadStringCompletedEventHandler(Object sender, DownloadStringCompletedEventArgs e);
    public delegate void OpenReadCompletedEventHandler(object sender, OpenReadCompletedEventArgs e);

    public class HttpWebClient
    {
        HttpWebRequest _webRequest = null;
        
        public event DownloadStringCompletedEventHandler DownloadStringCompleted;
        public event OpenReadCompletedEventHandler OpenReadCompleted;

        public void CancelAsync()
        {
            if (_webRequest != null)
            {
                _webRequest.Abort();
            }
        }

        public void OpenReadAsync(string uri)
        {
            _webRequest = HttpWebRequest.CreateHttp(uri);
            _webRequest.AllowReadStreamBuffering = true;
            
            _webRequest.BeginGetResponse(StreamRespCallback, null);
        }

        private void StreamRespCallback(IAsyncResult asynchronousResult)
        {
            try
            {
                HttpWebResponse response = (HttpWebResponse)_webRequest.EndGetResponse(asynchronousResult);
                Stream responseStream = response.GetResponseStream();
                if (OpenReadCompleted != null)
                {
                    System.Windows.Deployment.Current.Dispatcher.BeginInvoke(
                        OpenReadCompleted, this, new OpenReadCompletedEventArgs(responseStream, false, null, null));
                }
            }
            catch (WebException) { }
        }
        
        public void DownloadStringAsync(string uri)
        {
            _webRequest = HttpWebRequest.CreateHttp(uri);
            _webRequest.Headers[HttpRequestHeader.IfModifiedSince] = DateTime.UtcNow.ToString("yyyy/MM/dd HH:mm:ss");
            _webRequest.BeginGetResponse(RespCallback, null);
        }

        public void DownloadStringAsync(string uri,object userToken)
        {
            _webRequest = HttpWebRequest.CreateHttp(uri);
            _webRequest.Headers[HttpRequestHeader.IfModifiedSince] = DateTime.UtcNow.ToString("yyyy/MM/dd HH:mm:ss");
            _webRequest.BeginGetResponse(RespCallback, userToken);
        }

        private void RespCallback(IAsyncResult asynchronousResult)
        {
            try
            {
                using (var response = (HttpWebResponse)_webRequest.EndGetResponse(asynchronousResult))
                using (var reader = new StreamReader(response.GetResponseStream()))
                {
                    if (DownloadStringCompleted != null)
                    {
                        DownloadStringCompleted(this,
                            new DownloadStringCompletedEventArgs(
                                reader.ReadToEnd(), false, null, asynchronousResult.AsyncState));
                    }
                }
            }
            catch (WebException) { }
        }
    }
}
