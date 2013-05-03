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
using System.Windows.Threading;
using System.Security.Cryptography;
using PPTVData.Entity;
using System.IO;
using System.IO.IsolatedStorage;
using System.Text;
using System.Xml;
namespace PPTVData.Factory
{
    public class RegisterFactory
    {
        #region Post注册信息 超时抛出事件
        public delegate void PostRegisterInfoTimeoutEventHandler(object sender);
        public event PostRegisterInfoTimeoutEventHandler PostRegisterInfoTimeout;
        #endregion

        #region Post注册信息 成功抛出事件
        public delegate void PostRegisterInfoSuccessEventHandler(object sender, string resultState, string resultContent);
        public event PostRegisterInfoSuccessEventHandler PostRegisterInfoSuccess;
        #endregion

        #region Post注册信息 失败抛出事件
        public delegate void PostRegisterInfoFailedEventHandler(object sender, Exception exc);
        public event PostRegisterInfoFailedEventHandler PostRegisterInfoFailed;
        #endregion

        string _registerURL = "http://passport.pptv.com/HFormReg.do";
        DispatcherTimer _timer = null;
        HttpWebRequest _httpWebRequest;
        RequestState _requestState = new RequestState();
        RegisterInfo _registerInfo;
        public RegisterFactory()
        {
            _timer = new DispatcherTimer();
            _timer.Interval = TimeSpan.FromSeconds(Timeout);
            _timer.Tick += new EventHandler(_timer_Tick);
        }

        void _timer_Tick(object sender, EventArgs e)
        {
            System.Windows.Deployment.Current.Dispatcher.BeginInvoke(() =>
            {
                (sender as DispatcherTimer).Stop();
                if (PostRegisterInfoTimeout != null)
                    PostRegisterInfoTimeout(this);
            });
        }

        private int _timeout = 10;//超时时间
        public int Timeout
        {
            get { return _timeout; }
            set
            {
                _timeout = value;
            }
        }

        public void PostRegisterInfoAsync(RegisterInfo registerInfo)
        {
            _requestState = new RequestState();
            _registerInfo = registerInfo;
            _httpWebRequest = (HttpWebRequest)WebRequest.Create(_registerURL);
            _timer.Start();
            _requestState.request = _httpWebRequest;
            _httpWebRequest.Method = "POST";
            _httpWebRequest.ContentType = "application/x-www-form-urlencoded";
            _httpWebRequest.BeginGetRequestStream(new AsyncCallback(RequestCallback), _httpWebRequest);
        }

        private void RequestCallback(IAsyncResult asyncResult)
        {
            UTF8Encoding encoding = new UTF8Encoding();
            string pars = "UserName=" + _registerInfo.UserName + "&PassWord=" + _registerInfo.PassWord + "&UserMail=" + _registerInfo.UserMail;

            Random random = new Random();
            int randomNumber = random.Next(999999);
            string randomKey = string.Format("{0:D6}", random);
            string validateStr = _registerInfo.PassWord + randomKey + (_registerInfo.UserMail.StartsWith("@") ? "" : _registerInfo.UserMail) + "" + _registerInfo.UserName;
            string md5Validate = MD5Helper.GetMd5String(validateStr).Substring(13, 10).ToLower();
            pars += "&RandomKey=" + randomKey + "&ValidateKey=" + md5Validate + "&ReturnURL=" + "";

            Stream _body = _httpWebRequest.EndGetRequestStream(asyncResult);
            byte[] formBytes = encoding.GetBytes(pars);
            _body.Write(formBytes, 0, formBytes.Length);
            _body.Close();

            _httpWebRequest.BeginGetResponse(new AsyncCallback(ResponseCallback), _requestState);
        }

        //得到返回数据
        private void ResponseCallback(IAsyncResult asyncResult)
        {
            RequestState requestState = (RequestState)asyncResult.AsyncState;
            HttpWebRequest httpWebRequest = requestState.request;
            requestState.response = (HttpWebResponse)httpWebRequest.EndGetResponse(asyncResult);
            Stream responseStream = requestState.response.GetResponseStream();
            requestState.streamResponse = responseStream;

            //HttpWebResponse httpWebResponse = (HttpWebResponse)_httpWebRequest.EndGetResponse(asyncResult);
            //Stream responseStream = httpWebResponse.GetResponseStream();
            IAsyncResult asynchronousInputRead = responseStream.BeginRead(requestState.BufferRead, 0, RequestState.BUFFER_SIZE, new AsyncCallback(ReadCallBack), requestState);
        }

        private void ReadCallBack(IAsyncResult asyncResult)
        {
            try
            {
                RequestState requestState = (RequestState)asyncResult.AsyncState;
                Stream responseStream = requestState.streamResponse;
                int read = responseStream.EndRead(asyncResult);
                if (read > 0)
                {
                    requestState.requestData.Append(Encoding.UTF8.GetString(requestState.BufferRead, 0, read));
                    IAsyncResult asynchronousResult = responseStream.BeginRead(requestState.BufferRead, 0, RequestState.BUFFER_SIZE, new AsyncCallback(ReadCallBack), requestState);
                }
                else
                {
                    if (requestState.requestData.Length > 1)
                    {
                        string stringContent;
                        stringContent = requestState.requestData.ToString();
                        string result = "";
                        string state = "-1";
                        AnalysisXML(stringContent, ref state, ref result);
                        System.Windows.Deployment.Current.Dispatcher.BeginInvoke(() =>
                        {
                            _timer.Stop();

                            if (PostRegisterInfoSuccess != null)
                                PostRegisterInfoSuccess(this, state, result);
                        });
                    }
                    responseStream.Close();
                }
            }
            catch (Exception ex)
            {
                System.Windows.Deployment.Current.Dispatcher.BeginInvoke(() =>
                {
                    _timer.Stop();

                    if (PostRegisterInfoFailed != null)
                        PostRegisterInfoFailed(this, ex);
                });
            }
        }

        private void AnalysisXML(string xmlContent, ref string state, ref string result)
        {
            XmlReader reader = XmlReader.Create(new StringReader(xmlContent));
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element && reader.Name == "State")
                {
                    reader.Read();
                    state = reader.Value;
                }
                if (reader.NodeType == XmlNodeType.Element && reader.Name == "Msg")
                {
                    reader.Read();
                    result = reader.Value;
                    break;
                }
            }
        }
    }

    public class RequestState
    {
        public const int BUFFER_SIZE = 512;
        public StringBuilder requestData;
        public byte[] BufferRead;
        public HttpWebRequest request;
        public HttpWebResponse response;
        public Stream streamResponse;
        public RequestState()
        {
            BufferRead = new byte[BUFFER_SIZE];
            requestData = new StringBuilder("");
            request = null;
            streamResponse = null;
        }
    }
}
