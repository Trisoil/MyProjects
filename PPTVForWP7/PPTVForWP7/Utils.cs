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
using System.Collections.Generic;
using System.Windows.Threading;
using System.Windows.Media.Imaging;
using System.IO.IsolatedStorage;
using Microsoft.Phone.BackgroundTransfer;
using PPTVData.Factory;
using Microsoft.Phone.Info;
using System.Text;
using Microsoft.Phone.Net.NetworkInformation;
using System.IO;

using PPTVData.Entity;
using System.Threading;
namespace PPTVForWP7
{
    /// <summary>
    /// 应用程序公共方法
    /// </summary>

    namespace Utils
    {
        #region HTTP下载

        public enum DownloadStatus
        {
            Paused,
            Downloading,
            Completed,
            Error,
            Timeout,
            Waiting,
            InsufficientDiskSpace
        }
        public class DownloadStatusChangeEventArgs : EventArgs
        {
            public DownloadStatusChangeEventArgs(DownloadStatus status)
            {
                Status = status;
            }
            private DownloadStatusChangeEventArgs()
            {

            }
            public DownloadStatus Status
            {
                get;
                private set;
            }
        }

        public class DownloadProgressChangeEventArgs : EventArgs
        {
            public DownloadProgressChangeEventArgs(long progress, long totalBytes)
            {
                Progress = progress;
                TotalBytes = totalBytes;
            }
            private DownloadProgressChangeEventArgs()
            {

            }
            public long Progress
            {
                get;
                private set;
            }
            public long TotalBytes
            {
                get;
                private set;
            }
        }
        public class MediaFileDownload : IDisposable
        {

            DispatcherTimer _timerTimeout = null;
            HttpWebRequest _webRequest = null;
            IsolatedStorageFileStream _fileStream = null;
            private string _pathName = string.Empty;
            private byte[] _buffer = null;
            const int BUFFER_SIZE = 256 * 1024;
            private object _tag = null;
            private long _fileLen = 0;
            private bool is_disposed = false;
            private Uri _mediaFileUrl = null;
            private string _path = string.Empty;
            private long _bytesReceived = 0;
            private long _lastByteReceived = 0;
            private string _fileName = string.Empty;
            private static List<MediaFileDownload> _downloadQueue = null;

            static MediaFileDownload()
            {
                if (_downloadQueue == null)
                    _downloadQueue = new List<MediaFileDownload>();
            }

            public MediaFileDownload()
            {
                _buffer = new byte[BUFFER_SIZE];
                Status = DownloadStatus.Paused;
                TransferMode = NetworkSelectionCharacteristics.NonCellular;
                EnableTimerDelegate += EnableTimeoutTimer;
                _path = "/PersonalCenter/MyDownload";
            }


            public event EventHandler<DownloadStatusChangeEventArgs> DownloadStatusChange;
            public event EventHandler<DownloadProgressChangeEventArgs> DownloadProgressChange;
            public static List<MediaFileDownload> DownloadQueue
            {
                get
                {
                    return _downloadQueue;
                }
                private set
                {
                    _downloadQueue = value;
                }

            }
            public long BytesReceived
            {
                get
                {
                    return _bytesReceived;
                }
                private set
                {
                    _bytesReceived = value;
                }
            }
            public long FileLength
            {
                get
                {
                    return _fileLen;
                }
                private set
                {
                    _fileLen = value;
                }
            }
            public DownloadStatus Status
            {
                get;
                private set;
            }
            public object Tag
            {
                get
                {
                    return _tag;
                }
                set
                {
                    _tag = value;
                }
            }
            public string FileName
            {
                get { return _fileName; }
                set
                {
                    _fileName = Utils.UrlDecode(value);

                    if (!string.IsNullOrEmpty(_path))
                    {
                        _pathName = System.IO.Path.Combine(_path, _fileName);
                    }
                }

            }
            public string PathName
            {
                get
                {
                    return _pathName;
                }
                set
                {
                    _pathName = value;
                }
            }
            public string Path
            {
                get
                {
                    if (!IsolatedStorageFile.GetUserStoreForApplication().DirectoryExists(_path))
                    {
                        IsolatedStorageFile.GetUserStoreForApplication().CreateDirectory(_path);

                    }
                    return _path;
                }
                set
                {
                    if (!IsolatedStorageFile.GetUserStoreForApplication().DirectoryExists(value))
                    {
                        IsolatedStorageFile.GetUserStoreForApplication().CreateDirectory(value);
                    }
                    _fileName = Utils.UrlDecode(_fileName);
                    if (!string.IsNullOrEmpty(_fileName))
                    {
                        _pathName = System.IO.Path.Combine(value, _fileName);
                    }
                    _path = value;
                }
            }

            public Uri MediaFileUrl
            {
                get
                {
                    return _mediaFileUrl;
                }
                set
                {
                    _mediaFileUrl = value;
                }
            }
            private void PauseWithoutMessage()
            {
                if (_webRequest != null)
                {
                    _webRequest.Abort();
                    if (_fileStream != null)
                        _fileStream.Dispose();

                }
                StartNextTask(this);
            }
            public void Pause()
            {
                Status = DownloadStatus.Paused;
                if (DownloadStatusChange != null)
                {
                    System.Windows.Deployment.Current.Dispatcher.BeginInvoke(DownloadStatusChange, this, new DownloadStatusChangeEventArgs(DownloadStatus.Paused));
                }
                if (_webRequest != null)
                {
                    _webRequest.Abort();
                }
                if (_fileStream != null)
                {
                    _fileStream.Dispose();
                    _fileStream = null;
                }

                StartNextTask(this);
            }

            public NetworkSelectionCharacteristics TransferMode { get; set; }

            private static void StartNextTask(MediaFileDownload curTask)
            {
                foreach (MediaFileDownload downloader in DownloadQueue)
                {
                    if (downloader.Status == DownloadStatus.Waiting)
                    {
                        downloader.Start();
                        return;
                    }
                }
            }
            public static void Add(MediaFileDownload downloader)
            {
                if (IsExistsInQueue(downloader))
                {
                    return;
                }
                _downloadQueue.Add(downloader);
            }
            public static MediaFileDownload Find(string tag)
            {
                foreach (MediaFileDownload downloader in DownloadQueue)
                {
                    if (tag == downloader.Tag as string)
                        return downloader;
                }
                return null;
            }
            public static void Remove(MediaFileDownload downloader)
            {
                _downloadQueue.Remove(downloader);

            }
            public static void Clear()
            {
                _downloadQueue.Clear();
            }
            private static bool IsExistsInQueue(MediaFileDownload downloader)
            {
                foreach (MediaFileDownload down in DownloadQueue)
                {
                    if (downloader == down)
                        return true;
                }
                return false;
            }
            private static bool HaveDownloadingTask(MediaFileDownload curTask)
            {
                foreach (MediaFileDownload downloader in DownloadQueue)
                {
                    if (downloader.Status == DownloadStatus.Downloading && downloader != curTask)
                        return true;
                }
                return false;
            }
            public static void StopAllTask()
            {
                foreach (MediaFileDownload downloader in DownloadQueue)
                {
                    if (downloader.Status != DownloadStatus.Completed)
                        downloader.Pause();
                }
            }
            public void Start()
            {
                string fileName = _mediaFileUrl.AbsolutePath;
                if (fileName[0] == '/')
                    fileName = fileName.Remove(0, 1);
                PathName = System.IO.Path.Combine(Path, Utils.UrlDecode(fileName));

                if (HaveDownloadingTask(this))
                {
                    Status = DownloadStatus.Waiting;
                    if (DownloadStatusChange != null)
                    {
                        DownloadStatusChange(this, new DownloadStatusChangeEventArgs(Status));
                    }
                    return;
                }
                Uri newAddress = new Uri(_mediaFileUrl.AbsoluteUri, UriKind.RelativeOrAbsolute);
                _webRequest = (HttpWebRequest)HttpWebRequest.Create(newAddress.AbsoluteUri);
                _webRequest.SetNetworkPreference(TransferMode);


                try
                {
                    if (_fileStream != null)
                        _fileStream.Dispose();
                    _fileStream = new IsolatedStorageFileStream(_pathName, FileMode.OpenOrCreate, IsolatedStorageFile.GetUserStoreForApplication());
                }
                catch (IsolatedStorageException)
                {
                    if (DownloadStatusChange != null)
                    {
                        Status = DownloadStatus.Error;
                        DownloadStatusChange(this, new DownloadStatusChangeEventArgs(DownloadStatus.Error));
                    }
                }

                _fileStream.Seek(0, SeekOrigin.End);
                long fileOffset = _fileStream.Position;
                _bytesReceived = 0;
                _bytesReceived += fileOffset;
                _webRequest.Headers["Range"] = "bytes=" + fileOffset.ToString() + "-";
                _webRequest.AllowReadStreamBuffering = false;
                _webRequest.BeginGetResponse(RespCallback, _tag);


                Status = DownloadStatus.Downloading;

                if (DownloadStatusChange != null)
                {
                    DownloadStatusChange(this, new DownloadStatusChangeEventArgs(DownloadStatus.Downloading));
                }


            }
            public delegate void EnableTimer(bool enabled);
            EnableTimer EnableTimerDelegate;
            private void EnableTimeoutTimer(bool enabled)
            {
                if (_timerTimeout == null)
                    _timerTimeout = new DispatcherTimer();
                _timerTimeout.Interval = TimeSpan.FromSeconds(10);
                if (enabled)
                    _timerTimeout.Start();
                else
                    _timerTimeout.Stop();
            }
            private void RespCallback(IAsyncResult asynchronousResult)
            {
                try
                {
                    HttpWebResponse response = (HttpWebResponse)_webRequest.EndGetResponse(asynchronousResult);

                    _fileLen = _bytesReceived + response.ContentLength;

                    long freeSpace = System.IO.IsolatedStorage.IsolatedStorageFile.GetUserStoreForApplication().AvailableFreeSpace;
                    if (freeSpace < _fileLen - _bytesReceived)
                    {
                        Pause();
                        Status = DownloadStatus.InsufficientDiskSpace;
                        if (DownloadStatusChange != null)
                        {
                            System.Windows.Deployment.Current.Dispatcher.BeginInvoke(DownloadStatusChange, this, new DownloadStatusChangeEventArgs(Status));
                        }
                        return;
                    }

                    Stream responseStream = response.GetResponseStream();
                    responseStream.BeginRead(_buffer, 0, BUFFER_SIZE, new AsyncCallback(ReadCallback), responseStream);
                }
                catch (Exception e)
                {
                    string message = e.Message;
                    Status = DownloadStatus.Error;
                    if (DownloadStatusChange != null)
                    {
                        System.Windows.Deployment.Current.Dispatcher.BeginInvoke(DownloadStatusChange, new object[] { this, new DownloadStatusChangeEventArgs(DownloadStatus.Error) });
                    }
                    PauseWithoutMessage();
                }
            }
            private void ReadCallback(IAsyncResult asyncResult)
            {
                try
                {
                    Stream stream = (Stream)asyncResult.AsyncState;
                    int read = stream.EndRead(asyncResult);

                    _fileStream.BeginWrite(_buffer, 0, read, WriteCallback, null);
                    _bytesReceived += read;
                    if (read > 0)
                    {
                        stream.BeginRead(_buffer, 0, BUFFER_SIZE, new AsyncCallback(ReadCallback), stream);
                    }
                    else
                    {
                        _fileStream.Dispose();
                        Status = DownloadStatus.Completed;
                        Remove(this);
                        if (DownloadStatusChange != null)
                        {
                            System.Windows.Deployment.Current.Dispatcher.BeginInvoke(DownloadStatusChange, this, new DownloadStatusChangeEventArgs(DownloadStatus.Completed));
                        }
                        StartNextTask(this);
                    }
                }
                catch (System.Exception ex)
                {
                    string message = ex.Message;
                    Status = DownloadStatus.Error;
                    if (DownloadStatusChange != null)
                    {
                        System.Windows.Deployment.Current.Dispatcher.BeginInvoke(DownloadStatusChange, this, new DownloadStatusChangeEventArgs(DownloadStatus.Error));
                    }
                    PauseWithoutMessage();
                }
            }

            private void WriteCallback(IAsyncResult asyncResult)
            {
                try
                {
                    _fileStream.EndWrite(asyncResult);
                    if ((_bytesReceived - _lastByteReceived) > 512 * 1024)
                    {
                        _lastByteReceived = _bytesReceived;
                        if (DownloadProgressChange != null)
                        {
                            System.Windows.Deployment.Current.Dispatcher.BeginInvoke(DownloadProgressChange, new object[] { this, new DownloadProgressChangeEventArgs(_bytesReceived, _fileLen) });
                        }
                    }
                }
                catch (System.Exception ex)
                {
                    string message = ex.Message;
                    Status = DownloadStatus.Error;
                    if (DownloadStatusChange != null)
                    {
                        System.Windows.Deployment.Current.Dispatcher.BeginInvoke(DownloadStatusChange, new object[] { this, new DownloadStatusChangeEventArgs(DownloadStatus.Error) });
                    }
                    PauseWithoutMessage();
                }

            }
            protected virtual void Dispose(bool disposing)
            {
                if (!is_disposed)
                {
                    if (disposing)
                    {

                    }
                    if (_fileStream != null)
                        _fileStream.Dispose();
                }
                is_disposed = true;
            }
            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }
            ~MediaFileDownload()
            {
                Dispose(false);
            }

            private void OnTimeout(object sender, EventArgs e)
            {
                _timerTimeout.Stop();
                Status = DownloadStatus.Timeout;
                if (DownloadStatusChange != null)
                {
                    System.Windows.Deployment.Current.Dispatcher.BeginInvoke(DownloadStatusChange, new object[] { this, new DownloadStatusChangeEventArgs(DownloadStatus.Timeout) });
                }
            }
        }


        #endregion

        #region 后台下载
        public class BackgroundDownload : IDisposable
        {
            private BackgroundTransferRequest _curRequest;
            private bool is_disposed = false;
            private TransferStatus _transferStatus = TransferStatus.None;
            public BackgroundDownload()
            {
                using (IsolatedStorageFile isoStore = IsolatedStorageFile.GetUserStoreForApplication())
                {
                    if (!isoStore.DirectoryExists("/shared/transfers"))
                    {
                        isoStore.CreateDirectory("/shared/transfers");
                    }

                }
                DownloadUrl = string.Empty;
            }
            public string DownloadUrl { get; set; }
            public TransferStatus TransferStatus
            {
                get
                {
                    return _transferStatus;
                }
                private set
                {
                    _transferStatus = value;
                }
            }


            public event EventHandler<BackgroundTransferEventArgs> BackgroundDownloadStatusChange;
            public event EventHandler<BackgroundTransferEventArgs> BackgroundDownloadProgressChange;
            private void OnBackgroundTransferStatusChange(object sender, BackgroundTransferEventArgs args)
            {
                if (args.Request.TransferStatus == TransferStatus.Completed)
                {
                    if (BackgroundTransferService.Find(args.Request.RequestId) != null)
                    {
                        BackgroundTransferService.Remove(args.Request);
                        if (BackgroundDownloadStatusChange != null)
                        {
                            _transferStatus = args.Request.TransferStatus;
                            BackgroundDownloadStatusChange(this, args);

                        }
                    }
                }
                else
                {
                    if (BackgroundDownloadStatusChange != null)
                    {
                        BackgroundDownloadStatusChange(this, args);
                    }
                }


            }
            private void OnBackgroundProgressChange(object sender, BackgroundTransferEventArgs args)
            {
                if (BackgroundDownloadProgressChange != null)
                {
                    BackgroundDownloadProgressChange(this, args);
                }
            }
            public int GetFreeRequestSlotCount()
            {
                int i = 0;
                foreach (BackgroundTransferRequest request in BackgroundTransferService.Requests)
                {
                    i++;
                }
                return 4 - i;
            }
            public bool Start()
            {
                if (GetFreeRequestSlotCount() == 0)
                {
                    TransferStatus = TransferStatus.Waiting;
                    return false;
                }

                System.Text.RegularExpressions.Regex regex = new System.Text.RegularExpressions.Regex("[a-zA-z]+://[^\\s]*");
                if (!regex.IsMatch(DownloadUrl))
                    return false;
                int lastIndex = DownloadUrl.LastIndexOf('/');
                string fileName = DownloadUrl.Substring(lastIndex + 1, DownloadUrl.Length - lastIndex - 1);
                Uri savedUri = new Uri("shared/transfers/" + fileName, UriKind.RelativeOrAbsolute);
                _curRequest = new BackgroundTransferRequest(new Uri(Uri.EscapeUriString(DownloadUrl), UriKind.RelativeOrAbsolute), savedUri);
                _curRequest.TransferPreferences = TransferPreferences.None;
                _curRequest.TransferProgressChanged += OnBackgroundProgressChange;
                _curRequest.TransferStatusChanged += OnBackgroundTransferStatusChange;

                BackgroundTransferService.Add(_curRequest);
                BackgroundTransferEventArgs args = new BackgroundTransferEventArgs(_curRequest);
                _transferStatus = TransferStatus.Transferring;
                if (BackgroundDownloadStatusChange != null)
                {
                    BackgroundDownloadStatusChange(this, args);
                }
                return true;
            }
            public static void EmptySlots()
            {
                foreach (BackgroundTransferRequest request in BackgroundTransferService.Requests)
                {
                    BackgroundTransferService.Remove(request);
                }
            }
            public void Abort()
            {
                if (_curRequest == null)
                    return;
                BackgroundTransferService.Remove(_curRequest);
                BackgroundTransferEventArgs args = new BackgroundTransferEventArgs(_curRequest);
                _transferStatus = TransferStatus.Paused;
                if (BackgroundDownloadStatusChange != null)
                {
                    BackgroundDownloadStatusChange(this, args);
                }

            }
            protected virtual void Dispose(bool disposing)
            {
                if (!is_disposed)
                {
                    if (disposing)
                    {

                    }
                    //  Abort();
                }
                is_disposed = true;
            }
            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }
            ~BackgroundDownload()
            {
                Dispose(false);
            }
        }
        #endregion

        #region 动画类

        public class CustomAnimation
        {
            #region 代理
            public delegate void DownloadCompletedEventHandler(object sender, EventArgs e);
            #endregion

            private static FrameworkElement _obj;
            private static void DownAnimaCompleted(object sender, EventArgs e)
            {
                if (_obj != null)
                    _obj.Visibility = Visibility.Collapsed;
            }
            public static void DownloadAnimation(FrameworkElement obj)
            {
                _obj = obj;
                obj.Visibility = Visibility.Visible;
                TransformGroup transformGroup = new TransformGroup();

                TranslateTransform XTransform = new TranslateTransform();
                TranslateTransform YTransform = new TranslateTransform();


                transformGroup.Children.Add(XTransform);
                transformGroup.Children.Add(YTransform);

                obj.RenderTransform = transformGroup;

                DoubleAnimation XAnima = new DoubleAnimation();
                XAnima.From = 0;
                XAnima.To = 175;
                XAnima.Duration = new Duration(TimeSpan.FromSeconds(0.25));
                Storyboard.SetTarget(XAnima, XTransform);
                Storyboard.SetTargetProperty(XAnima, new PropertyPath(TranslateTransform.XProperty));
                Storyboard storyBoard = new Storyboard();
                storyBoard.Children.Add(XAnima);

                DoubleAnimation YAnima = new DoubleAnimation();
                YAnima.From = 0;
                YAnima.To = 510;
                YAnima.Duration = new Duration(TimeSpan.FromSeconds(0.25));
                Storyboard.SetTarget(YAnima, YTransform);
                Storyboard.SetTargetProperty(YAnima, new PropertyPath(TranslateTransform.YProperty));

                storyBoard.Children.Add(YAnima);
                storyBoard.Begin();

                storyBoard.Completed += DownAnimaCompleted;

            }

            public CustomAnimation()
            {
                ImagesSource = new List<string>();
                Images = new List<BitmapImage>();
                imageShowIndex = 0;

            }
            //淡入淡出效果函数

            //fadeIn 为true表示淡入,false表示淡出
            //fadeObj 为淡入淡出对象
            //duration 为动画持续时间
            //completedHandler 为完成动画时的回调函数,如果不用可以设为null
            public UIElement animationObj { get; private set; }

            public void SetAnimationObject(UIElement obj)
            {
                animationObj = obj;
            }

            public void FadeInOut(bool fadeIn, double duration, EventHandler completedHandler)
            {
                DoubleAnimation animaFadeInOut = new DoubleAnimation();
                if (fadeIn)
                {
                    animaFadeInOut.From = 0;
                    animaFadeInOut.To = 1.0;
                }
                else
                {
                    animaFadeInOut.From = 1.0;
                    animaFadeInOut.To = 0.0;
                }
                animaFadeInOut.Duration = new Duration(TimeSpan.FromSeconds(duration));

                Storyboard.SetTarget(animaFadeInOut, animationObj);
                Storyboard.SetTargetProperty(animaFadeInOut, new PropertyPath(UIElement.OpacityProperty));

                Storyboard storyboard = new Storyboard();
                if (completedHandler != null)
                {
                    storyboard.Completed += completedHandler;
                }
                storyboard.Children.Add(animaFadeInOut);
                storyboard.Begin();


            }

            //翻转轴
            public enum FlipAxis
            {
                X,
                Y,
                Z
            }
            //翻转点
            public enum FlipCenter
            {
                Top,
                Left,
                Right,
                Bottom,
                Center
            }
            public bool randomFlipDirection { get; set; }
            public bool randomImages { get; set; }
            private int imageShowIndex;
            private void OnFlipTimer(object sender, EventArgs e)
            {
                Random rand = new Random();

                if (randomImages)
                    imageShowIndex = rand.Next(ImagesSource.Count);
                else
                    imageShowIndex = (imageShowIndex + 1) % ImagesSource.Count;

                FlipAxis flipAxis;
                if (randomFlipDirection)
                {
                    flipAxis = (FlipAxis)rand.Next(1);
                }
                else
                {
                    flipAxis = FlipAxis.X;
                }

                if (ImagesSource.Count == 0)
                {
                    Flip3D(flipAxis, FlipCenter.Center, 0.075, true, 90, true);
                }
                else
                {

                    if (animationObj.GetType().Name == "Image")
                    {
                        Flip3D(flipAxis, FlipCenter.Center, 0.075, true, 90, true);
                    }
                }

            }
            public List<string> ImagesSource { get; set; }
            private List<BitmapImage> Images;
            public void PeriodicallyFlip3D(double interval)
            {
                DispatcherTimer timer = new DispatcherTimer();
                timer.Interval = TimeSpan.FromSeconds(interval);
                timer.Tick += OnFlipTimer;
                timer.Start();
            }
            private void OnFlipCompleted(object sender, EventArgs e)
            {
                if (animationObj.GetType().Name == "Image")
                {
                    Image image = animationObj as Image;
                    BitmapImage[] images = Images.ToArray();
                    image.Source = images[imageShowIndex];
                    Flip3D(FlipAxis.X, FlipCenter.Center, 0.075, false, 90, false);
                }
            }
            public void Flip3D(FlipAxis flipAxis, FlipCenter flipPoint, double duration, bool startFromZero, double angle, bool switchImage)
            {
                DoubleAnimation flipAnimation = new DoubleAnimation();
                if (startFromZero)
                {
                    flipAnimation.From = 0;
                    flipAnimation.To = angle;
                }
                else
                {
                    flipAnimation.From = angle;
                    flipAnimation.To = 0;
                }
                flipAnimation.Duration = new Duration(TimeSpan.FromSeconds(duration));
                if (switchImage)
                {
                    flipAnimation.Completed += OnFlipCompleted;
                }
                else
                {
                    flipAnimation.Completed -= OnFlipCompleted;
                }


                PlaneProjection planeProjection = new PlaneProjection();
                switch (flipPoint)
                {
                    case FlipCenter.Top:
                        {
                            planeProjection.CenterOfRotationX = 0.5;
                            planeProjection.CenterOfRotationY = 0;
                            planeProjection.CenterOfRotationZ = 0;
                        }
                        break;
                    case FlipCenter.Left:
                        {
                            planeProjection.CenterOfRotationX = 0;
                            planeProjection.CenterOfRotationY = 0.5;
                            planeProjection.CenterOfRotationZ = 0;
                        }
                        break;
                    case FlipCenter.Right:
                        {
                            planeProjection.CenterOfRotationX = 1.0;
                            planeProjection.CenterOfRotationY = 0.5;
                            planeProjection.CenterOfRotationZ = 0;
                        }
                        break;

                    case FlipCenter.Bottom:
                        {
                            planeProjection.CenterOfRotationX = 0.5;
                            planeProjection.CenterOfRotationY = 1.0;
                            planeProjection.CenterOfRotationZ = 0;
                        }
                        break;
                    case FlipCenter.Center:
                        {
                            planeProjection.CenterOfRotationX = 0.5;
                            planeProjection.CenterOfRotationY = 0.5;
                            planeProjection.CenterOfRotationZ = 0;
                        }
                        break;

                }

                animationObj.Projection = planeProjection;
                Storyboard.SetTarget(flipAnimation, animationObj.Projection);
                switch (flipAxis)
                {
                    case FlipAxis.X:
                        Storyboard.SetTargetProperty(flipAnimation, new PropertyPath(PlaneProjection.RotationXProperty));
                        break;
                    case FlipAxis.Y:
                        Storyboard.SetTargetProperty(flipAnimation, new PropertyPath(PlaneProjection.RotationYProperty));
                        break;
                    case FlipAxis.Z:
                        Storyboard.SetTargetProperty(flipAnimation, new PropertyPath(PlaneProjection.RotationZProperty));
                        break;
                }

                Storyboard storyboard = new Storyboard();
                storyboard.Children.Add(flipAnimation);
                storyboard.Begin();

            }

        }
        #endregion

        #region DAC日志
        public class PlayInfo
        {
            public int castType;
            public int playTime;
            public int programSource;
            public int prepareTime;
            public int bufferTime;
            public int allBufferCount;
            public int dragCount;
            public int dragBufferTime;
            public int playBufferCount;
            public int connType;
            public int isPlaySucceeded;
            public int averageDownSpeed;
            public int stopReason;
        }
        public class DAC
        {
            public static void DACDownload(PPTVData.Entity.ChannelDetailInfo channelInfo)
            {
                string content = "Action=0&A=5&B=1&C=6&D=" + GetUserId() + "&E=" + Utils.GetClientVersion();
                content += "&F=" + channelInfo.TypeID.ToString();
                content += "&G=" + channelInfo.VID.ToString();
                content += "&H=" + Encoding.UTF8.GetString(Encoding.UTF8.GetBytes(channelInfo.Title), 0, channelInfo.Title.Length);

            }
            public static void DACAd()
            {

            }
            private static string GetUserId()
            {
                string userId = null;

                if (System.IO.IsolatedStorage.IsolatedStorageSettings.ApplicationSettings.Contains("UserId"))
                {
                    userId = System.IO.IsolatedStorage.IsolatedStorageSettings.ApplicationSettings["UserId"] as string;

                }


                if (userId == null)
                {
                    Guid guid = Guid.NewGuid();
                    System.IO.IsolatedStorage.IsolatedStorageSettings.ApplicationSettings.Add("UserId", guid.ToString());
                    System.IO.IsolatedStorage.IsolatedStorageSettings.ApplicationSettings.Save();

                    return guid.ToString();
                }
                else
                    return userId;
            }
        }
        #endregion

        public static class Utils
        {
            public static bool IsDebug = false;
            private static long _startTime;
            private static IsolatedStorageSettings settings = IsolatedStorageSettings.ApplicationSettings;
            private static string _volumeDataFile = "volume.db";
            private static string _userNaFile = "userNa.db";
            private static string _userpdFile = "userpd.db";
            private static string _isSaveUserInfoFile = "isSaveUserInfo.db";
            private static string _isAutoLoginInfoFile = "isAutoLoginInfo.db";
            private static string _enableHighDefinition = "enableHighDefinition";
            public static UserStateInfo _userStateInfo;
            public static bool _isValidatedFromServer = false;
            public static bool _isLogOff = true;
            public static int _lanchCount = 0;
            public static string NetExceptInfo = "网络异常，稍后再试";
            public static void ProgramStart()
            {
                _startTime = DateTime.Now.Ticks;
            }

            public static void LogOff()
            {
                _isLogOff = false;
                _isValidatedFromServer = false;
                _userStateInfo = null;
                if (GetUserInfoSaveSetting() == false)
                {
                    SetUserNaToIOS(string.Empty);
                    SetUserpdToIOS(string.Empty);
                    // SetAutoLoginToIOS(false);
                }
                //else
                //{
                //    SetAutoLoginToIOS(false);
                //}
            }

            public static void GetUserInfo()
            {
                if (Utils.GetAutoLoginSetting() && string.IsNullOrEmpty(Utils.GetUserNa()) != true && string.IsNullOrEmpty(Utils.GetUserpd()) != true)
                {
                    LoginFactory _loginFactory = new LoginFactory();
                    _loginFactory.HttpFailHandler += _loginFactory_GetUserInfoFailed;
                    _loginFactory.HttpSucessHandler += _loginFactory_GetUserInfoSuccess;
                    _loginFactory.HttpTimeOutHandler += _loginFactory_GetUserInfoTimeout;
                    _loginFactory.DownLoadDatas(new UserInfo(Utils.GetUserNa(), Utils.GetUserpd(), string.Empty));
                }
            }

            static void _loginFactory_GetUserInfoTimeout(object sender, HttpFactoryArgs<UserStateInfo> args)
            {
            }

            static void _loginFactory_GetUserInfoSuccess(object sender, HttpFactoryArgs<UserStateInfo> args)
            {
                Utils._userStateInfo = args.Result;
                _isValidatedFromServer = true;
            }

            static void _loginFactory_GetUserInfoFailed(object sender, HttpFactoryArgs<UserStateInfo> args)
            {
            }

            public static int GetAppLifeTime()
            {
                return TimeSpan.FromTicks(DateTime.Now.Ticks - _startTime).Seconds;
            }

            public static Storyboard GenerateStoryboardByTwoHPoint(UIElement uc, double originalPoint, double currentPoint, TimeSpan timeSpan, bool hasOriginalPoint = true)
            {
                DoubleAnimationUsingKeyFrames daukf = new DoubleAnimationUsingKeyFrames();
                if (hasOriginalPoint)
                    daukf.KeyFrames.Add(new EasingDoubleKeyFrame() { KeyTime = new TimeSpan(0, 0, 0), Value = originalPoint });
                daukf.KeyFrames.Add(new EasingDoubleKeyFrame() { KeyTime = timeSpan, Value = currentPoint });
                StoryboardBindTargetAndProperty(daukf, uc, "(UIElement.RenderTransform).(TransformGroup.Children)[0].(TranslateTransform.X)");
                Storyboard sb = new Storyboard();
                sb.Children.Add(daukf);
                return sb;
            }

            public static Storyboard GenerateStoryboardForTicker(UIElement uc, double currentPoint, TimeSpan timeSpan)
            {
                DoubleAnimationUsingKeyFrames daukf = new DoubleAnimationUsingKeyFrames();
                daukf.KeyFrames.Add(new EasingDoubleKeyFrame() { KeyTime = new TimeSpan(0, 0, 0, 0, 2000), Value = 0 });
                daukf.KeyFrames.Add(new EasingDoubleKeyFrame() { KeyTime = timeSpan.Add(TimeSpan.FromMilliseconds(2000)), Value = currentPoint });
                daukf.KeyFrames.Add(new EasingDoubleKeyFrame() { KeyTime = timeSpan.Add(TimeSpan.FromMilliseconds(4000)), Value = currentPoint });
                StoryboardBindTargetAndProperty(daukf, uc, "(UIElement.RenderTransform).(TransformGroup.Children)[0].(TranslateTransform.X)");
                Storyboard sb = new Storyboard();
                sb.AutoReverse = true;
                sb.RepeatBehavior = RepeatBehavior.Forever;
                sb.Children.Add(daukf);
                return sb;
            }

            public static void StoryboardBindTargetAndProperty(Timeline timeline, DependencyObject target, string path)
            {
                Storyboard.SetTarget(timeline, target);
                Storyboard.SetTargetProperty(timeline, new PropertyPath(path));
            }

            public static void SetIOSData(string key, string value)
            {
                if (settings.Contains(key))
                {
                    settings[key] = value;
                }
                else
                {
                    settings.Add(key, value);
                }
            }

            public static string GetIOSData(string key, string defValue)
            {
                if (settings.Contains(key))
                {
                    return settings[key].ToString();
                }
                else
                {
                    return defValue;
                }
            }

            public static void SetUserNaToIOS(string name)
            {
                try
                {
                    SetIOSData(_userNaFile, name);
                }
                catch (Exception)
                {

                }
            }

            public static string GetUserNa()
            {
                string tem = string.Empty;
                try
                {

                    tem = GetIOSData(_userNaFile, string.Empty);
                }
                catch (Exception)
                {
                    tem = string.Empty;
                }
                return tem;
            }

            public static void SetUserpdToIOS(string pwd)
            {
                try
                {
                    SetIOSData(_userpdFile, pwd);
                }
                catch (Exception)
                {

                }
            }

            public static string GetUserpd()
            {
                string tem = string.Empty;
                try
                {
                    tem = GetIOSData(_userpdFile, string.Empty);
                }
                catch (Exception)
                {

                }
                return tem;
            }

            public static void SetAutoLoginToIOS(bool isAutoLogin)
            {
                try
                {
                    if (isAutoLogin)
                    {
                        SetIOSData(_isAutoLoginInfoFile, "true");
                    }
                    else
                        SetIOSData(_isAutoLoginInfoFile, "false");
                }
                catch (Exception)
                {

                }
            }

            public static bool GetAutoLoginSetting()
            {
                bool tem = false;
                try
                {
                    tem = Convert.ToBoolean(GetIOSData(_isAutoLoginInfoFile, "false"));
                }
                catch (Exception)
                {
                    tem = false;
                }
                return tem;
            }

            public static void SetUserInfoSaveSwitchToIOS(bool isSave)
            {
                try
                {
                    if (isSave)
                    {
                        SetIOSData(_isSaveUserInfoFile, "true");
                    }
                    else
                        SetIOSData(_isSaveUserInfoFile, "false");
                }
                catch (Exception)
                {

                }
            }

            public static bool GetUserInfoSaveSetting()
            {
                bool tem = false;
                try
                {
                    tem = Convert.ToBoolean(GetIOSData(_isSaveUserInfoFile, "false"));
                }
                catch (Exception)
                {
                    tem = false;
                }
                return tem;
            }

            public static void SaveVolumeToIOS(double value)
            {
                try
                {
                    string tem = value.ToString();
                    SetIOSData(_volumeDataFile, tem);
                }
                catch (Exception)
                {
                    SetIOSData(_volumeDataFile, "0.85");
                }
            }

            public static double GetVolume()
            {
                double tem = 0.85;
                try
                {
                    tem = Convert.ToDouble(GetIOSData(_volumeDataFile, "0.85"));
                }
                catch (Exception)
                {
                    tem = 0.85;
                }
                return tem;
            }

            public static bool GetEnableDefinition()
            {
                try
                {
                    return Convert.ToBoolean(GetIOSData(_enableHighDefinition, "false"));
                }
                catch
                {
                    return false;
                }
            }
            public static void SetEnableDefinition(bool enabled)
            {
                try
                {
                    SetIOSData(_enableHighDefinition, enabled.ToString());
                }
                catch
                {

                }
            }
            public static Version GetClientVersion()
            {
                try
                {
                    System.Reflection.Assembly[] assemblies = System.AppDomain.CurrentDomain.GetAssemblies();
                    foreach (System.Reflection.Assembly assembly in assemblies)
                    {

                        if (assembly.FullName.Contains("PPTVForWP7"))
                        {
                            string assName = assembly.ToString();
                            string version = assName.Split(',')[1];
                            version = version.Substring("Version=".Length + 1);
                            return Version.Parse(version);

                        }
                    }
                }
                catch (Exception exception)
                {
                    string message = exception.Message;
                    return null;
                }
                return null;
            }
            private static NetworkInterfaceType _currentNetworkType;
            public static NetworkInterfaceType CurrentNetworkType
            {
                get
                {
                    return _currentNetworkType;
                }
                private set
                {
                    _currentNetworkType = value;
                }
            }
            public static event EventHandler NetworkInterfaceTypeChanged;
            public static void CheckNetworkType()
            {
                ThreadPool.QueueUserWorkItem((o) =>
                {
                    _currentNetworkType = Microsoft.Phone.Net.NetworkInformation.NetworkInterface.NetworkInterfaceType;

                    if (NetworkInterfaceTypeChanged != null)
                    {
                        System.Windows.Deployment.Current.Dispatcher.BeginInvoke(() =>            
                        {                
                            NetworkInterfaceTypeChanged(null, EventArgs.Empty);            
                        });
                    }

                });
            }

            #region UrlDecode GB2312 Version
            public static string UrlDecode(string url)
            {
                return CustomUrlDecode(url, 0);
            }
            private static string Decode(string encoded)
            {
                int start = 0;
                List<byte> wordArray = new List<byte>();
                while (start < encoded.Length)
                {
                    string word = encoded.Substring(start, 2);
                    start += 2;

                    wordArray.Add(Convert.ToByte(word, 16));
                }

                return new GB2312.GB2312Encoding().GetString(wordArray.ToArray(), 0, wordArray.Count);
            }
            private static string CustomUrlDecode(string url, int start)
            {
                if (string.IsNullOrEmpty(url))
                    return "";
                string converted = "";

                int idx = url.IndexOf('%', start);
                if (idx != -1)
                {
                    converted = url.Substring(start, idx - start);

                    string word = url.Substring(idx + 1, 2);
                    if (idx + 3 >= url.Length)
                    {
                        return converted + Decode(word);
                    }
                    while (url[idx + 3] == '%')
                    {
                        idx += 3;
                        word += url.Substring(idx + 1, 2);
                    }
                    converted += Decode(word);
                    return converted + CustomUrlDecode(url, idx + 3);

                }
                else
                {
                    return url.Substring(start, url.Length - start);
                }
            }
            #endregion

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

            public static void GetImage(Image image, string imageUrl)
            {
                try
                {
                    Uri uri = new Uri(imageUrl, UriKind.RelativeOrAbsolute);
                    Delay.LowProfileImageLoader.SetUriSource(image, uri);
                }
                catch (Exception)
                {

                }

            }

            public static BitmapImage GetImage(string imageUrl)
            {
                return new BitmapImage(new Uri(imageUrl, UriKind.RelativeOrAbsolute));
            }

            public static Color GetColor(string colorName)
            {
                if (colorName.StartsWith("#"))
                    colorName = colorName.Replace("#", string.Empty);
                int v = int.Parse(colorName, System.Globalization.NumberStyles.HexNumber);
                return new Color()
                {
                    A = Convert.ToByte((v >> 24) & 255),
                    R = Convert.ToByte((v >> 16) & 255),
                    G = Convert.ToByte((v >> 8) & 255),
                    B = Convert.ToByte((v >> 0) & 255)
                };
            }

            /// <summary>
            /// X位置位移动画
            /// </summary>
            /// <param name="element">元素</param>
            /// <param name="from">from</param>
            /// <param name="to">to</param>
            /// <param name="delay">开始时间</param>
            /// <param name="duration">持续时间</param>
            /// <returns></returns>
            public static Storyboard XAnimation(FrameworkElement element, double from, double to, double delay, double duration)
            {
                Storyboard sb = new Storyboard();

                TranslateTransform trans = new TranslateTransform() { X = from };
                element.RenderTransform = trans;

                DoubleAnimation db = new DoubleAnimation();
                db.To = to;
                db.From = from;
                db.EasingFunction = new PowerEase() { EasingMode = EasingMode.EaseOut };
                sb.Duration = db.Duration = TimeSpan.FromSeconds(duration);
                sb.BeginTime = TimeSpan.FromSeconds(delay);
                sb.Children.Add(db);

                Storyboard.SetTarget(db, trans);
                Storyboard.SetTargetProperty(db, new PropertyPath("X"));

                return sb;
            }

            /// <summary>
            /// Y位置位移动画
            /// </summary>
            /// <param name="element">元素</param>
            /// <param name="from">from</param>
            /// <param name="to">to</param>
            /// <param name="delay">开始时间</param>
            /// <param name="duration">持续时间</param>
            /// <returns></returns>
            public static Storyboard YAnimation(FrameworkElement element, double from, double to, double delay, double duration)
            {
                Storyboard sb = new Storyboard();

                TranslateTransform trans = new TranslateTransform() { Y = from };
                element.RenderTransform = trans;

                DoubleAnimation db = new DoubleAnimation();
                db.To = to;
                db.From = from;
                db.EasingFunction = new PowerEase() { EasingMode = EasingMode.EaseOut };
                sb.Duration = db.Duration = TimeSpan.FromSeconds(duration);
                sb.BeginTime = TimeSpan.FromSeconds(delay);
                sb.Children.Add(db);

                Storyboard.SetTarget(db, trans);
                Storyboard.SetTargetProperty(db, new PropertyPath("Y"));

                return sb;
            }
        }

        public enum ChannelTypes : int
        {
            VIP尊享 = 75099,
            电影 = 1,
            电视剧 = 2,
            动漫 = 3,
            综艺 = 4,
            体育 = 5,
            新闻 = 6,
            游戏 = 7,
            潮流 = 1269,
            军事 = 75344,
            教育 = 75346,
            娱乐 = 75374,
        }
    }

    public delegate void DownloadDelegateByNotPar();
    public delegate void DownloadDelegateWithPar(bool tem);
    public class GetDataStatusHelp
    {
        public delegate void DownloadFailEventHandler(object sender);
        public event DownloadFailEventHandler DataDownloadFailure;
        private int _countOfLoadFailure = 0;

        private int _maxCountOfTry = 2;
        public int MaxCountOfTry
        {
            get { return _maxCountOfTry; }
            set
            {
                _maxCountOfTry = value;
            }
        }

        private bool _isTry = false;
        public bool IsTry
        {
            get { return _isTry; }
            private set { _isTry = value; }
        }

        public void Clear()
        {
            _countOfLoadFailure = 0;
            IsTry = false;
        }

        public void Do(DownloadDelegateWithPar delegateMethod, bool isStart = true)
        {
            if (Record())
            {
                if (MessageBox.Show("数据访问失败,点击确定按钮刷新页面", "提示", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
                {
                    delegateMethod(isStart);
                }
            }
            else
                Clear();
        }

        public void Do(DownloadDelegateByNotPar delegateMethod)
        {
            if (Record())
            {
                if (MessageBox.Show("数据访问失败,点击确定按钮刷新页面", "提示", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
                {
                    delegateMethod();
                }
            }
            else
                Clear();
        }

        private bool Record()
        {
            _countOfLoadFailure++;
            if (_countOfLoadFailure < _maxCountOfTry)
            {
                IsTry = true;
            }
            else
            {
                Clear();
                if (DataDownloadFailure != null)
                    DataDownloadFailure(this);
            }
            return IsTry;
        }
    }
}
