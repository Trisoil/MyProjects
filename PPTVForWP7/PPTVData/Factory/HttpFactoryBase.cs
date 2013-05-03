using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Threading;
using System.Xml;
using System.IO;
using System.Text;

namespace PPTVData.Factory
{
    /// <summary>
    /// 网络数据管理基类
    /// </summary>
    /// <typeparam name="M">数据回调返回实体类</typeparam>
    public abstract class HttpFactoryBase<M>
        where M : class, new()
    {
        private Timer _timer;
        private HttpWebClient _webClient;

        public HttpFactoryBase()
        {
            _timer = new Timer(_timer_CallBack, null, Timeout.Infinite, Timeout.Infinite);
        }

        /// <summary>
        /// 数据下载成功触发事件
        /// </summary>
        public EventHandler<HttpFactoryArgs<M>> HttpSucessHandler;

        /// <summary>
        /// 数据下载超时触发事件
        /// </summary>
        public EventHandler<HttpFactoryArgs<M>> HttpTimeOutHandler;

        /// <summary>
        /// 数据下载失败触发事件
        /// </summary>
        public EventHandler<HttpFactoryArgs<M>> HttpFailHandler;

        protected virtual int TimeOut
        {
            get { return 10000; }
        }

        /// <summary>
        /// 开始下载数据
        /// </summary>
        /// <param name="paras"></param>
        public virtual void DownLoadDatas(params object[] paras)
        {
            Cancel();
            _webClient = new HttpWebClient();
            _webClient.DownloadStringCompleted += new DownloadStringCompletedEventHandler(_webClient_DownloadStringCompleted);
            _webClient.DownloadStringAsync(CreateUri(paras));
            _timer.Change(TimeOut, Timeout.Infinite);
        }

        /// <summary>
        /// 取消下载
        /// </summary>
        public virtual void Cancel()
        {
            if (_timer != null)
                _timer.Change(Timeout.Infinite, Timeout.Infinite);
            if (_webClient != null)
            {
                _webClient.DownloadStringCompleted -= new DownloadStringCompletedEventHandler(_webClient_DownloadStringCompleted);
                _webClient.CancelAsync();
                _webClient = null;
            }
        }

        /// <summary>
        /// 定时器回调，超时时间到达后数据下载未完成则取消下载
        /// </summary>
        /// <param name="state"></param>
        private void _timer_CallBack(object state)
        {
            Cancel();
            if (HttpTimeOutHandler != null)
                Deployment.Current.Dispatcher.BeginInvoke(
                HttpTimeOutHandler, this, new HttpFactoryArgs<M>());
        }

        /// <summary>
        /// 数据下载完成后回调
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void _webClient_DownloadStringCompleted(object sender, DownloadStringCompletedEventArgs e)
        {
            try
            {
                var list = AnalysisXML(e.Result);
                if (list != null)
                {
                    if (HttpSucessHandler != null)
                        Deployment.Current.Dispatcher.BeginInvoke(
                            HttpSucessHandler, this, new HttpFactoryArgs<M>(string.Empty, list));
                }
                else
                {
                    if (HttpFailHandler != null)
                        Deployment.Current.Dispatcher.BeginInvoke(
                        HttpFailHandler, this, new HttpFactoryArgs<M>("数据解析错误", null));
                }
            }
            catch (Exception ex)
            {
                if (HttpFailHandler != null)
                    Deployment.Current.Dispatcher.BeginInvoke(
                    HttpFailHandler, this, new HttpFactoryArgs<M>(ex.Message, null));
            }
            finally
            {
                Cancel();
            }
        }

        /// <summary>
        /// 分析下载数据
        /// </summary>
        /// <param name="result"></param>
        /// <returns></returns>
        protected virtual M AnalysisXML(string result)
        {
            try
            {
                using (var reader = XmlReader.Create(new StringReader(result)))
                {
                    //var count = AnalysisCount(reader);
                    return AnalysisData(reader);
                }
            }
            catch
            {
                return null;
            }
        }

        /// <summary>
        /// 生成下载Uri
        /// </summary>
        /// <param name="paras"></param>
        /// <returns></returns>
        protected abstract string CreateUri(params object[] paras);

        /// <summary>
        /// 序列化成对象集合
        /// </summary>
        /// <param name="reader"></param>
        /// <param name="count"></param>
        /// <returns></returns>
        protected abstract M AnalysisData(XmlReader reader);
    }

    /// <summary>
    /// 下载事件传递参数类型
    /// </summary>
    /// <typeparam name="M"></typeparam>
    public class HttpFactoryArgs<M> : EventArgs
        where M : class
    {
        public HttpFactoryArgs()
        {
        }

        public HttpFactoryArgs(string message, M result)
        {
            Message = message;
            Result = result;
        }

        public string Message { get; set; }
        public M Result { get; set; }
    }
}
