using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.IO;
using System.Net;
using System.Xml;
using System.Net.Http;

using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.System.Threading;

namespace PPTVData.Factory
{
    using PPTVData.Utils;

    /// <summary>
    /// 网络数据管理基类
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <typeparam name="M"></typeparam>
    public abstract class HttpFactoryBase<M>
        where M : class, new()
    {
        private ThreadPoolTimer _timer;

        protected WebClient _webClient;
        protected CoreDispatcher _dispatcher;
        protected static string[] KEY = 
        { 
            "70706C6976656F6B",
			"15B9FDAEDA40F86BF71C73292516924A294FC8BA31B6E9EA",
			"29028A7698EF4C6D3D252F02F4F79D5815389DF18525D326",
			"D046E6B6A4A85EB6C44C73372A0D5DF1AE76405173B3D5EC",
			"435229C8F79831131923F18C5DE32F253E2AF2AD348C4615",
			"9B2915A72F8329A2FE6B681C8AAE1F97ABA8D9D58576AB20",
			"B3B0CD830D92CB3720A13EF4D93B1A133DA4497667F75191",
			"AD327AFB5E19D023150E382F6D3B3EB5B6319120649D31F8",
			"C42F31B008BF257067ABF115E0346E292313C746B3581FB0",
			"529B75BAE0CE2038466704A86D985E1C2557230DDF311ABC",
			"8A529D5DCE91FEE39E9EE9545DF42C3D9DEC2F767C89CEAB"
        };

        public HttpFactoryBase()
        {
            _dispatcher = Window.Current.CoreWindow.Dispatcher;
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

        /// <summary>
        /// 简化版，数据下载超时或失败触发
        /// </summary>
        public Action HttpFailorTimeOut;

        /// <summary>
        /// 网络超时时间
        /// </summary>
        protected virtual int TimerOut
        {
            get { return 12; }
        }

        /// <summary>
        ///是否利用后台线程回调
        /// </summary>
        protected virtual bool IsBackThreadCallBack
        {
            get { return false; }
        }

        /// <summary>
        /// HTTP请求方法
        /// </summary>
        protected virtual HttpMethod Method
        {
            get { return HttpMethod.Get; }
        }

        /// <summary>
        /// HTTP请求头
        /// </summary>
        //protected Dictionary<string, string> Heads
        //{
        //    get { return null; }
        //}

        /// <summary>
        /// 用于子类中断HTTP请求
        /// </summary>
        protected virtual HttpStatusCode StatusCode { get; set; }

        /// <summary>
        /// POST提交参数
        /// </summary>
        protected virtual string PostArgs
        {
            get { return string.Empty; }
        }
        
        /// <summary>
        /// 开始下载数据
        /// </summary>
        /// <param name="paras"></param>
        public virtual void DownLoadDatas(params object[] paras)
        {
            Cancel();
            var uri = CreateUri(paras);

            _webClient = new WebClient();
            _webClient.DownloadStringCompleted += new DownloadStringCompletedEventHandler(_webClient_DownloadStringCompleted);
            if (Method == HttpMethod.Get)
            {
                _webClient.DownloadStringAsync(uri);
                _timer = ThreadPoolTimer.CreateTimer(_threadTimer_CallBack, TimeSpan.FromSeconds(TimerOut));
            }
            else
            {
                var postargs = PostArgs;
                if (StatusCode != HttpStatusCode.InternalServerError)
                    _webClient.UploadStringAsync(uri, postargs);
                StatusCode = HttpStatusCode.OK;
            }
        }
        
        /// <summary>
        /// 取消下载
        /// </summary>
        public virtual void Cancel()
        {
            if (_timer != null)
            {
                _timer.Cancel();
                _timer = null;
            }
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
        private void _threadTimer_CallBack(ThreadPoolTimer timer)
        {
            Cancel();
            if (HttpTimeOutHandler != null)
                _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    HttpTimeOutHandler(this, new HttpFactoryArgs<M>());
                });
            if (HttpFailorTimeOut != null)
                _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    HttpFailorTimeOut();
                });
        }
            
        /// <summary>
        /// 数据下载完成后回调
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void _webClient_DownloadStringCompleted(object sender, HttpCompletedEventArgs e)
        {
            try
            {
                var content = AnalysisContent(e.Result);
                if (content != null)
                {
                    if (HttpSucessHandler != null)
                    {
                        if (!IsBackThreadCallBack)
                            _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                            {
                                HttpSucessHandler(this, new HttpFactoryArgs<M>(string.Empty, content));
                            });
                        else
                            HttpSucessHandler(this, new HttpFactoryArgs<M>(string.Empty, content));
                    }
                }
                else
                {
                    if (HttpFailHandler != null)
                        _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                        {
                            HttpFailHandler(this, new HttpFactoryArgs<M>("数据解析错误", null));
                        });
                    if (HttpFailorTimeOut != null)
                        _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                        {
                            HttpFailorTimeOut();
                        });
                }
            }
            catch (Exception ex)
            {
                if (HttpFailHandler != null)
                    _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        HttpFailHandler(this, new HttpFactoryArgs<M>(ex.Message, null));
                    });
                if (HttpFailorTimeOut != null)
                    _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        HttpFailorTimeOut();
                    });
            }
            finally
            {
                Cancel();
            }
        }

        /// <summary>
        /// 分析下载数据，非Xml类数据重载该方法
        /// </summary>
        /// <param name="result"></param>
        /// <returns></returns>
        protected virtual M AnalysisContent(Stream content)
        {
            try
            {
                using (var reader = XmlReader.Create(content))
                {
                    return AnalysisData(reader);
                }
            }
            catch (XmlException)
            {
                return new M();
            }
            catch
            {
                return null;
            }
        }

        /// <summary>
        /// 序列化XML成对象集合，Xml类数据重载该方法
        /// </summary>
        /// <param name="reader"></param>
        /// <returns></returns>
        protected virtual M AnalysisData(XmlReader reader)
        {
            return null;
        }

        /// <summary>
        /// 生成下载Uri
        /// </summary>
        /// <param name="paras"></param>
        /// <returns></returns>
        protected abstract string CreateUri(params object[] paras);
        
        /// <summary>
        /// 3DES加密
        /// </summary>
        /// <param name="source"></param>
        /// <param name="keyIndex"></param>
        /// <returns></returns>
        protected string Get3DESClientKeys(string source, int keyIndex, bool isUrlEncode = true)
        {
            byte[] bufferOut = null;

            var src = source;
            if (isUrlEncode)
                src = WebUtility.UrlEncode(source);
            BroccoliProducts.DESCrytography.TripleDES(Encoding.UTF8.GetBytes(src),
                ref bufferOut, DataCommonUtils.HexToBytes(KEY[keyIndex]), DataCommonUtils.HexToBytes(KEY[0]), true);
            return Convert.ToBase64String(bufferOut);
        }

        /// <summary>
        /// 3DES，数据无需URLEncode
        /// </summary>
        /// <param name="source"></param>
        /// <param name="keyIndex"></param>
        /// <returns></returns>
        protected string Get3DES(byte[] source, int keyIndex)
        {
            byte[] bufferOut = null;

            BroccoliProducts.DESCrytography.TripleDES(source,
                ref bufferOut, DataCommonUtils.HexToBytes(KEY[keyIndex]), DataCommonUtils.HexToBytes(KEY[0]), true);
            return Convert.ToBase64String(bufferOut);
        }
    }
}
