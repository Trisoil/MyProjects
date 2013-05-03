using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Media.Imaging;
using System.Collections.ObjectModel;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;
using System.IO.IsolatedStorage;
using PPTVForWP7.Utils;
using PPTVData.Entity;
using PPTVData.Factory;
using System.Threading;

namespace PPTVForWP7
{
    public partial class DownPage : PhoneApplicationPage
    {
        public static DBMyDownloadFactory DownFactory = new DBMyDownloadFactory();
        public static ObservableCollection<DownInfo> MyDownloading = null;
        public static ObservableCollection<DownInfo> MyDownloaded = null;

        private static DownPage _instance = null;
        private bool _isDowningEdit = false;
        private bool _isDownedEdit = false;
        private static double _minDBOperationInterval = 5;
        private static DateTime _lastWriteDB = DateTime.Now;
        private static DateTime _lastUpdateProgress = DateTime.Now;
        private string navSource = string.Empty;
        public static object syncUpdateTask = new object();
        public class DownInfo : System.ComponentModel.INotifyPropertyChanged
        {
            public DownInfo()
            {
                _vid = "";
                _titleImgUrl = ProgressText = Title = String.Empty;
                _progressNumeric = 0;
                _statusImage = "/PPTVForWP7;component/Images/Pause.png";
                _rate = 0;
            }
            public DownInfo(string vid, string titleImgUrl, string progressText, double progressNumeric, string title, string subTitle, string statusImage, int rate,string localPathName)
            {
                _vid = vid;
                _titleImgUrl = titleImgUrl;
                _progressText = progressText;
                _progressNumeric = progressNumeric;
                _title = String.Format("{0}{1}",title,subTitle);
                _statusImage = statusImage;
                _rate = rate;
                _subTitle = subTitle;
                _localPathName = localPathName;
            }
            public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;

            private void NotifyPropertyChanged(String info)
            {
                if (PropertyChanged != null)
                {
                    System.Windows.Deployment.Current.Dispatcher.BeginInvoke(PropertyChanged, new object[] { this, new System.ComponentModel.PropertyChangedEventArgs(info) });
                }
            }

            private string _vid;
            private string _titleImgUrl;
            private string _progressText;
            private double _progressNumeric;
            private string _title;
            private string _statusImage;
            private int _rate;
            private string _subTitle;
            private Visibility _showDelete = Visibility.Collapsed;
            private Visibility _showAction = Visibility.Visible;
            private string _localPathName = String.Empty;
            public string LocalPathName
            {
                get
                {
                    return _localPathName;
                }
                set
                {
                    _localPathName = value;
                }
            }
            public Visibility ShowDelete
            {
                get
                {
                    return _showDelete;
                }
                set
                {
                    _showDelete = value;
                    NotifyPropertyChanged("ShowDelete");
                }
            }
            public Visibility ShowAction
            {
                get
                {
                    return _showAction;
                }
                set
                {
                    _showAction = value;
                    NotifyPropertyChanged("ShowAction");
                }
            }
            public int Rate
            {
                get
                {
                    return _rate;
                }
                set
                {
                    _rate = value;
                    NotifyPropertyChanged("Rate");
                }
            }
            public string VID
            {
                get
                {
                    return _vid;
                }
                set
                {
                    _vid = value;
                    NotifyPropertyChanged("VID");
                }
            }
            public string SubTitle
            {
                get
                {
                    return _subTitle;
                }
                set
                {
                    _subTitle = value;
                    NotifyPropertyChanged("SubTitle");
                }
            }
            public string TitleImgUrl
            {
                get
                {
                    return _titleImgUrl;
                }

                set
                {
                    _titleImgUrl = value;
                    NotifyPropertyChanged("TitleImgUrl");
                }
            }
            public string ProgressText
            {
                get
                {
                    return _progressText;
                }
                set
                {
                    _progressText = value;
                    NotifyPropertyChanged("ProgressText");
                }
            }
            public double ProgressNumeric
            {
                get
                {
                    return _progressNumeric;
                }
                set
                {
                    _progressNumeric = value;
                    NotifyPropertyChanged("ProgressNumeric");
                }
            }
            public string Title
            {
                get
                {
                    return _title;
                }
                set
                {
                    _title = value;
                    NotifyPropertyChanged("Title");
                }
            }
            public string StatusImage
            {
                get
                {
                    return _statusImage;
                }
                set
                {
                    _statusImage = value;
                    NotifyPropertyChanged("StatusImage");
                }
            }
        }
        private string _rid = string.Empty;
        private string _vid = string.Empty;
        private string _downUrl = string.Empty;
        private ChannelDetailInfo _channelDetailInfo = null;

        static DownPage()
        {
            BuildDownInfos();
        }

        public DownPage()
        {
            InitializeComponent();
            _instance = this;
        }
        public static void UpdateTask(string vid, string downUrl)
        {
            
            MediaFileDownload.Find(vid).MediaFileUrl = new Uri(downUrl, UriKind.RelativeOrAbsolute);
            string fileName = MediaFileDownload.Find(vid).MediaFileUrl.AbsolutePath;
            if (fileName[0] == '/')
                fileName = fileName.Remove(0, 1);
            MediaFileDownload.Find(vid).FileName = fileName;
            DownFactory.UpdateFileNameRecord(vid,MediaFileDownload.Find(vid).PathName);
            foreach (DownInfo info in MyDownloading)
            {
                if (info.VID == vid)
                {
                    info.LocalPathName = MediaFileDownload.Find(vid).PathName;
                    break;
                }
            }
            MediaFileDownload.Find(vid).Start();
        }
        public static bool AddTask(ChannelDetailInfo channelDetailInfo, ProgramInfo programInfo)
        {

            MediaFileDownload downloader = new MediaFileDownload();                        
            string index = (programInfo != null) ? programInfo.Index.ToString() : string.Empty;
            string channelVid = (programInfo != null) ? programInfo.ChannelVID.ToString() : string.Empty;
            downloader.Tag = channelDetailInfo.VID.ToString() + "|" + index + "|" + channelVid;
            if (DownFactory.IsExistVid(downloader.Tag as string))
                return false;
            Utils.MediaFileDownload.Add(downloader);
            int rate = (int)channelDetailInfo.Mark;
            string subTitle = string.Empty;
            if (channelDetailInfo.TypeID != 1 && programInfo != null)
            {
                subTitle = "(第" + (programInfo.Index + 1).ToString() + "集)";
            }
            DownFactory.InsertRecord(downloader.Tag as string, channelDetailInfo.ImgUrl, channelDetailInfo.Title, "", downloader.PathName, 0, 0, (int)DownloadStatus.Downloading, rate);
            DownInfo info = new DownInfo(downloader.Tag as string, channelDetailInfo.ImgUrl, "正在下载:0M", 0, channelDetailInfo.Title, subTitle, "/PPTVForWP7;component/Images/Pause.png", rate,String.Empty);
            MyDownloading.Add(info);

            foreach (MediaFileDownload down in Utils.MediaFileDownload.DownloadQueue)
            {
                down.DownloadStatusChange += OnDownloadStatusChange;
                down.DownloadProgressChange += OnDownloadProgressChange;
            }

            return true;
        }
        public static void BuildDownInfos()
        {

            if (DownFactory == null)
                DownFactory = new DBMyDownloadFactory();

            if (MyDownloading == null)
                MyDownloading = new ObservableCollection<DownInfo>();
            if (MyDownloaded == null)
                MyDownloaded = new ObservableCollection<DownInfo>();

            ObservableCollection<DBMyDownloadInfo> downRecords = DownFactory.GetAllRecords();
            MyDownloading.Clear();
            MyDownloaded.Clear();
            foreach (DBMyDownloadInfo record in downRecords)
            {
                string progressText = String.Format("正在下载:{0}M/{1}M", ((double)record.Progress / (double)(1024 * 1024)).ToString("f2"), ((double)record.FileLen / (double)(1024 * 1024)).ToString("f2"));
                double progressNumeric = 0;
                if (record.FileLen == 0)
                    progressNumeric = 0;
                else
                    progressNumeric = (double)record.Progress / (double)record.FileLen * 100;

                string subTitle = string.Empty;
                string[] id = record.VID.Split('|');
                if (!string.IsNullOrEmpty(id[1]))
                {
                    subTitle = "(第" + (Convert.ToInt32(id[1]) + 1).ToString() + "集)";
                }


                MediaFileDownload downloader = new MediaFileDownload();
                downloader.MediaFileUrl = new Uri(record.FileUrl, UriKind.RelativeOrAbsolute);
                downloader.Tag = record.VID;
                downloader.PathName = record.LocalPathName;
                downloader.DownloadProgressChange += OnDownloadProgressChange;
                downloader.DownloadStatusChange += OnDownloadStatusChange;
                string statusImage = string.Empty;
                if (record.Status != (int)DownloadStatus.Completed)
                {
                    Utils.MediaFileDownload.Add(downloader);
                    switch (Utils.MediaFileDownload.Find(record.VID).Status)
                    {
                        case DownloadStatus.Paused:
                        case DownloadStatus.Error:
                        case DownloadStatus.Timeout:
                            statusImage = "/PPTVForWP7;component/Images/pause.png";
                            break;

                        case DownloadStatus.Downloading:
                            statusImage = "/PPTVForWP7;component/Images/Download.png";
                            break;
                        case DownloadStatus.Waiting:
                            statusImage = "/PPTVForWP7;component/Images/waiting.png";
                            break;
                    }

                }

                DownInfo info = new DownInfo(record.VID, record.TitleImgUrl, progressText, progressNumeric, record.Title, subTitle, statusImage, record.Rate,record.LocalPathName);
                if (record.Status != (int)DownloadStatus.Completed)
                {
                    MyDownloading.Add(info);
                }
                else
                {
                    MyDownloaded.Add(info);
                }
            }

            if (System.IO.IsolatedStorage.IsolatedStorageFile.GetUserStoreForApplication().DirectoryExists("/PersonalCenter/MyDownload"))
            {
                string[] files = System.IO.IsolatedStorage.IsolatedStorageFile.GetUserStoreForApplication().GetFileNames("/PersonalCenter/MyDownload/*.*");
                foreach (string file in files)
                {
                    if (!DownFactory.IsExistFile(file))
                    {
                        System.IO.IsolatedStorage.IsolatedStorageFile.GetUserStoreForApplication().DeleteFile(
                            System.IO.Path.Combine("/PersonalCenter/MyDownload", file));
                    }
                }
            }
           
        }

        private static void OnDownloadStatusChange(object sender, DownloadStatusChangeEventArgs e)
        {
            MediaFileDownload downloader = sender as MediaFileDownload;
            string statusImage = string.Empty;
            foreach (DownInfo info in MyDownloading)
            {
                if (info.VID == downloader.Tag as string)
                {
                    switch (e.Status)
                    {
                        case DownloadStatus.Paused:
                        case DownloadStatus.Error:
                        case DownloadStatus.Timeout:
                            foreach (DBMyDownloadInfo downInfo in DownFactory.GetAllRecords())
                            {
                                if (downInfo.VID == downloader.Tag as string)
                                {
                                    if ((DateTime.Now.Second - _lastWriteDB.Second) >= _minDBOperationInterval)
                                    {
                                        DownFactory.UpdateRecord(downloader.Tag as string, (int)e.Status, downloader.BytesReceived);
                                        _lastWriteDB = DateTime.Now;
                                    }
                                    break;
                                }
                            }
                            info.StatusImage = "/PPTVForWP7;component/Images/pause.png";
                            break;
                        case DownloadStatus.Downloading:
                            info.StatusImage = "/PPTVForWP7;component/Images/Download.png";
                            break;
                        case DownloadStatus.Completed:
                            MyDownloaded.Add(info);
                            MyDownloading.Remove(info);

                            break;
                        case DownloadStatus.Waiting:
                            info.StatusImage = "/PPTVForWP7;component/Images/waiting.png";
                            break;
                        case DownloadStatus.InsufficientDiskSpace:
                            MessageBox.Show("空间不足,请删除一些下载任务", "提示", MessageBoxButton.OK);
                            break;
                        default:
                            break;
                    }
                    break;
                }
            }
            
            DownFactory.UpdateRecord(downloader.Tag as string, (int)e.Status, -1);


        }

        private static void OnDownloadProgressChange(object sender, DownloadProgressChangeEventArgs e)
        {
            if ((DateTime.Now - _lastUpdateProgress).TotalSeconds < 2)
            {
                _lastUpdateProgress = DateTime.Now;
                return;
            }
            MediaFileDownload downloader = sender as MediaFileDownload;
            DownFactory.UpdateFileLenRecord(downloader.Tag as string, downloader.FileLength);
            foreach (DownInfo info in MyDownloading)
            {
                if (downloader.Tag as string == info.VID)
                {
                    info.ProgressText = String.Format("正在下载:{0}M/{1}M", ((double)e.Progress / (double)(1024 * 1024)).ToString("f2"), ((double)e.TotalBytes / (double)(1024 * 1024)).ToString("f2"));
                    info.ProgressNumeric = (double)e.Progress / (double)e.TotalBytes * 100;
                    break;
                }
            }
            foreach (DBMyDownloadInfo downInfo in DownFactory.GetAllRecords())
            {
                if (downInfo.VID == downloader.Tag as string)
                {
                    if ((DateTime.Now.Second - _lastWriteDB.Second) >= _minDBOperationInterval)
                    {   
                        DownFactory.UpdateRecord(downloader.Tag as string, -1, e.Progress);
                        _lastWriteDB = DateTime.Now;
                    }
                    break;
                }
            }


        }
        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs args)
        {
            if (NavigationContext.QueryString.ContainsKey("source"))
            {
                navSource = NavigationContext.QueryString["source"];
                if (navSource == "splash")
                {
                    NavigationService.RemoveBackEntry();
                    BuildDownInfos();
                    xMyDown.Items.Remove(xMyDowning);
                }
            }
            else
            {
                navSource = string.Empty;
            }
        }
        private void xAction_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            Grid grid = sender as Grid;
            MediaFileDownload downloader = MediaFileDownload.Find(grid.Tag as string);

            if (downloader != null)
            {
                switch (downloader.Status)
                {
                    case DownloadStatus.Downloading:
                    case DownloadStatus.Waiting:
                        downloader.Pause();
                        break;
                    case DownloadStatus.Error:
                    case DownloadStatus.Paused:
                    case DownloadStatus.Timeout:
                    case DownloadStatus.InsufficientDiskSpace:
                        {
                            _vid = downloader.Tag as string;
                            foreach (DownInfo info in MyDownloading)
                            {
                                if (info.VID == _vid)
                                {
                                    info.StatusImage = "/PPTVForWP7;component/Images/Download.png";
                                    break;
                                }
                            }
                            string[] id = _vid.Split('|');
                            if (string.IsNullOrEmpty(id[1]))
                            {
                                //电影
                                StartDownloadProgram(Convert.ToInt32(id[0]));
                            }
                            else
                            {
                                //剧集
                                StartDownloadProgram(Convert.ToInt32(id[2]));
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        private void OnDeleteSingleTask(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            string id = button.Tag as string;
            if (navSource == "splash")
            {
                foreach (MediaFileDownload downloader in Utils.MediaFileDownload.DownloadQueue)
                {
                    if (id == downloader.Tag as string)
                    {
                        downloader.Pause();
                        downloader.Dispose();
                        try
                        {
                            if (IsolatedStorageFile.GetUserStoreForApplication().FileExists(downloader.PathName))
                                IsolatedStorageFile.GetUserStoreForApplication().DeleteFile(downloader.PathName);
                        }
                        catch (IsolatedStorageException)
                        {
                            MessageBox.Show("删除失败，请稍后重试!");
                            return;
                        }
                        break;
                    }
                }
                DownFactory.DeleteRecord(id);
                foreach (DownInfo down in MyDownloaded)
                {
                    if (down.VID == id)
                    {
                        MyDownloaded.Remove(down);
                        break;
                    }

                }
                return;
            }
            
            if (xMyDown.SelectedIndex == 0)
            {
                foreach (MediaFileDownload downloader in Utils.MediaFileDownload.DownloadQueue)
                {
                    if (id == downloader.Tag as string)
                    {
                        downloader.Pause();
                        downloader.Dispose();
                        try
                        {
                            if (IsolatedStorageFile.GetUserStoreForApplication().FileExists(downloader.PathName))
                                IsolatedStorageFile.GetUserStoreForApplication().DeleteFile(downloader.PathName);
                        }
                        catch (IsolatedStorageException)
                        {
                            MessageBox.Show("删除失败，请稍后重试!");
                            return;
                        }
                        Utils.MediaFileDownload.Remove(downloader);
                        break;
                    }
                }
                DownFactory.DeleteRecord(id);
                foreach (DownInfo down in MyDownloading)
                {
                    if (down.VID == id)
                    {
                        MyDownloading.Remove(down);
                        break;
                    }
                }
            }

            if (xMyDown.SelectedIndex == 1)
            {
                foreach (DownInfo down in MyDownloaded)
                {
                    if (down.VID == id)
                    {
                        if (IsolatedStorageFile.GetUserStoreForApplication().FileExists(down.LocalPathName)) 
                            IsolatedStorageFile.GetUserStoreForApplication().DeleteFile(down.LocalPathName);
                        MyDownloaded.Remove(down);
                        break;
                    }

                }               
                DownFactory.DeleteRecord(id);
            }
        }
        private void OnEditClick(object sender, EventArgs e)
        {
            if (navSource != "splash")
            {
                if (xMyDown.SelectedIndex == 0)
                {
                    _isDowningEdit = !_isDowningEdit;
                }
                if (xMyDown.SelectedIndex == 1)
                {
                    _isDownedEdit = !_isDownedEdit;
                }
            }
            else
            if (navSource == "splash")
            {
                _isDownedEdit = !_isDownedEdit;
            }
            SwitchEditMode(_isDowningEdit,_isDownedEdit);
        }

        private void OnDeleteClick(object sender, EventArgs e)
        {
            foreach (ApplicationBarIconButton button in ApplicationBar.Buttons)
            {
                button.IsEnabled = false;
            }
            if (navSource == "splash")
            {
                ClearAllTask(false);
                return;
            }

            if (xMyDown.SelectedIndex == 0)
            {
                //清除正在下载
                ClearAllTask(true);
                return;
            }


            if (xMyDown.SelectedIndex == 1)
            {
                //清除已下载
                ClearAllTask(false);
                return;
            }
        }
        private void ClearAllTask(bool Downing)
        {
            if (Downing)
            {
                if (System.Windows.MessageBox.Show("清除全部正在下载内容?", "提示", MessageBoxButton.OKCancel) == MessageBoxResult.Cancel)
                {
                    foreach (ApplicationBarIconButton button in ApplicationBar.Buttons)
                    {
                        button.IsEnabled = true;
                    }
                    return;
                }
                var deletedInfos = new List<DownInfo>(MyDownloading.Count);
                var deletedMedias = new List<MediaFileDownload>(Utils.MediaFileDownload.DownloadQueue.Count);
                foreach (DownInfo down in MyDownloading)
                {
                    foreach (MediaFileDownload downloader in Utils.MediaFileDownload.DownloadQueue)
                    {
                        if (down.VID == downloader.Tag as string)
                        {
                            downloader.Pause();
                            try
                            {
                                if (IsolatedStorageFile.GetUserStoreForApplication().FileExists(downloader.PathName))
                                    IsolatedStorageFile.GetUserStoreForApplication().DeleteFile(downloader.PathName);
                            }
                            catch (IsolatedStorageException)
                            {
                                foreach (var info in deletedInfos)
                                    MyDownloading.Remove(info);
                                foreach (var media in deletedMedias)
                                    Utils.MediaFileDownload.Remove(media);
                                MessageBox.Show("删除失败，请稍后重试!");
                                return;
                            }
                            deletedMedias.Add(downloader);
                            break;
                        }
                    }
                    deletedInfos.Add(down);
                    DownFactory.DeleteRecord(down.VID);
                    LocalPlayFactory locFac = new LocalPlayFactory();
                    locFac.DeleteRecord(down.VID);
                }
                MyDownloading.Clear();
                Utils.MediaFileDownload.Clear();

                foreach (ApplicationBarIconButton button in ApplicationBar.Buttons)
                {
                    button.IsEnabled = true;
                }
            }
            else
            {
                if (System.Windows.MessageBox.Show("清除全部已下载内容?", "提示", MessageBoxButton.OKCancel) == MessageBoxResult.Cancel)
                {
                    foreach (ApplicationBarIconButton button in ApplicationBar.Buttons)
                    {
                        button.IsEnabled = true;
                    }
                    return;
                }
                foreach (DownInfo down in MyDownloaded)
                {
                    if (IsolatedStorageFile.GetUserStoreForApplication().FileExists(down.LocalPathName))
                    IsolatedStorageFile.GetUserStoreForApplication().DeleteFile(down.LocalPathName);
                    DownFactory.DeleteRecord(down.VID);
                    LocalPlayFactory locFac = new LocalPlayFactory();
                    locFac.DeleteRecord(down.VID);
                }
                MyDownloaded.Clear();

                foreach (ApplicationBarIconButton button in ApplicationBar.Buttons)
                {
                    button.IsEnabled = true;
                }
            }
        }
        private void OnPlayLocalFile(object sender, System.Windows.Input.GestureEventArgs e)
        {
            var grid = sender as Grid;
            var border = grid.Children[0] as Border;
            if (border != null)
            {
                var image = border.Child as Image;
                if (image == null) return;
                string ID = image.Tag as string;
                string Path = string.Empty;
                string Title = string.Empty;

                foreach (DBMyDownloadInfo info in DownFactory.GetAllRecords())
                {
                    if (info.VID == ID)
                    {
                        Path = info.LocalPathName;
                        break;
                    }
                }

                foreach (DownInfo info in MyDownloaded)
                {
                    if (info.VID == ID)
                    {
                        Title = info.Title;
                        break;
                    }
                }

                NavigationService.Navigate(new Uri(String.Format("/PlayPage.xaml?path={0}&title={1}&id={2}&localplay=1", Path, Title, ID), UriKind.RelativeOrAbsolute));
            }
        }
        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
            _isDowningEdit = false;
            _isDownedEdit = false;

            xMyDownloading.ItemsSource = MyDownloading;
            xMyDownloaded.ItemsSource = MyDownloaded;
            SwitchEditMode(_isDowningEdit, _isDownedEdit);

        }
        private void SwitchEditMode(bool downingEdit, bool downedEdit)
        {
            if (!downingEdit)
            {
                if (MyDownloading != null)
                foreach (DownInfo info in MyDownloading)
                {
                    info.ShowDelete = Visibility.Collapsed;
                    info.ShowAction = Visibility.Visible;
                }
            }

            else
            {
                if (MyDownloading != null)
                foreach (DownInfo info in MyDownloading)
                {
                    info.ShowDelete = Visibility.Visible;
                    info.ShowAction = Visibility.Collapsed;

                }


            }


            if (!downedEdit)
            {
                if (MyDownloaded != null)
                foreach (DownInfo info in MyDownloaded)
                {
                    info.ShowDelete = Visibility.Collapsed;
                }
            }
            else
            {
                if (MyDownloaded != null)
                foreach (DownInfo info in MyDownloaded)
                {
                    info.ShowDelete = Visibility.Visible;
                }
            }


        }
        private void OnGetChannelDetailSucceeded(object sender, ChannelDetailInfo channelDetailInfo)
        {
            _channelDetailInfo = channelDetailInfo;

        }
        private void StartDownloadProgram(int Vid)
        {
            PlayInfoFactory playFac = new PlayInfoFactory();
            playFac.HttpSucessHandler += playFac_GetPlayInfoSucceed;
            playFac.HttpTimeOutHandler += playFac_GetPlayInfoTimeout;
            playFac.HttpFailHandler += playFac_GetPlayInfoFailed;
            playFac.DownLoadDatas(Vid);
        }
        private void playFac_GetPlayInfoTimeout(object sender, HttpFactoryArgs<PPTVData.Entity.PlayInfo> args)
        {
            foreach (DownInfo info in MyDownloading)
            {
                if (info.VID == _vid)
                {
                    info.StatusImage = "/PPTVForWP7;component/Images/pause.png";
                    break;
                }
            }
        }
        private void playFac_GetPlayInfoFailed(object sender, HttpFactoryArgs<PPTVData.Entity.PlayInfo> args)
        {
            foreach (DownInfo info in MyDownloading)
            {
                if (info.VID == _vid)
                {
                    info.StatusImage = "/PPTVForWP7;component/Images/png.png";
                    break;
                }
            }
        }


        void playFac_GetPlayInfoSucceed(object sender, HttpFactoryArgs<PPTVData.Entity.PlayInfo> args)
        {
            PPTVData.Entity.PlayInfo.Item item = args.Result.GetItemByFileType(5);
            if (item == null)
                return;
            _rid = item.Rid;
            if (item.DtInfo != null)
            {

                DownPage.UpdateTask(_vid, item.url);

            }
            else
            {
                string jump_url = "http://wp7.jump.synacast.com/" + item.Rid + "dt?type=mwp7";
                PPTVData.HttpWebClient jumpWebClient = new PPTVData.HttpWebClient();
                jumpWebClient.DownloadStringCompleted += new PPTVData.DownloadStringCompletedEventHandler(JumpWebClientDownloadStringCompleted);
                jumpWebClient.DownloadStringAsync(jump_url);
            }
        }

        private void JumpWebClientDownloadStringCompleted(object sender, PPTVData.DownloadStringCompletedEventArgs e)
        {
            string mediaServerHost = String.Empty;
            string serverTime = String.Empty;
            DateTime serverBaseTime;
            DateTime localBaseTime;
            if (e.Error == null)
            {
                try
                {
                    using (System.Xml.XmlReader reader = System.Xml.XmlReader.Create(new System.IO.StringReader(e.Result)))
                    {
                        if (true == reader.ReadToFollowing("server_host"))
                        {
                            mediaServerHost = reader.ReadElementContentAsString("server_host", "");
                        }
                        else
                        {

                        }

                        reader.ReadToFollowing("server_time");
                        serverTime = reader.ReadElementContentAsString("server_time", "");

                        System.Globalization.CultureInfo ci = new System.Globalization.CultureInfo("en-US");
                        serverBaseTime = DateTime.ParseExact(serverTime, "ddd MMM dd HH:mm:ss yyyy UTC", ci);
                        localBaseTime = DateTime.Now;

                        string downUrl = String.Format("http://{0}/{1}?type=wp7&w=1&key={2}", mediaServerHost, _rid, PPTVData.KeyGenerator.GetKey(serverBaseTime));


                        UpdateTask(_vid, downUrl);

                    }
                }
                catch
                {
                }
            }
        }

        private void PhoneApplicationPage_BackKeyPress(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (navSource == "splash")
            {
                var vc = Microsoft.Devices.VibrateController.Default;
                vc.Start(TimeSpan.FromMilliseconds(100));
                if (MessageBox.Show("确定退出吗?", "提示", MessageBoxButton.OKCancel) != MessageBoxResult.OK)
                {
                    e.Cancel = true;
                }
            }
        }
    }
}