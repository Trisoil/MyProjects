using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.IsolatedStorage;
using System.Windows;
using System.Windows.Threading;

using Microsoft.Phone.BackgroundTransfer;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Download
{
    using PPTVData.Factory;
    using PPTVData.Factory.WAY;

    using PPTV.WPRT.CommonLibrary.DataModel.Download;
    using PPTV.WPRT.CommonLibrary.ViewModel.Channel;

    public class DownloadViewModel : DownloadBase
    {
        private Dispatcher _dispatcher;

        public static readonly DownloadViewModel Instance = new DownloadViewModel();

        public Action DownloadingCountChannge;

        public DownloadViewModel()
        {
            _dispatcher = Deployment.Current.Dispatcher;
        }

        public ObservableCollection<DownloadInfo> DownloadingItems
        {
            get { return DownloadingViewModel.Instance.DataInfos; }
        }

        public ObservableCollection<DownloadInfo> DownloadedItems
        {
            get { return DownloadedViewModel.Instance.DataInfos; }
        }

        public async Task Init()
        {
            await DownloadingViewModel.Instance.Init();
            await DownloadedViewModel.Instance.Init();
            using (var isoStore = IsolatedStorageFile.GetUserStoreForApplication())
            {
                if (!isoStore.DirectoryExists("/shared/transfers"))
                    isoStore.CreateDirectory("/shared/transfers");
                if (!isoStore.DirectoryExists("/PersonalCenter/MyDownload"))
                    isoStore.CreateDirectory("/PersonalCenter/MyDownload");
            }
            RestartDownload();
        }

        /// <summary>
        /// 批量添加下载
        /// </summary>
        /// <param name="infos"></param>
        public void AddDownloads(IList<DownloadInfo> infos)
        {

            foreach (var info in infos)
            {
                var downloaded = DownloadedItems.FirstOrDefault(v => v.ChannelId == info.ChannelId);
                if (downloaded != null)
                    DownloadedViewModel.Instance.DeleteRecord(downloaded);

                if (!_isDownloading)
                {
                    ReadyToDownload(info);
                }

                DownloadingViewModel.Instance.InsertRecord(info);
            }
            DownloadingChange();
        }

        /// <summary>
        /// 批量删除正在下载任务
        /// </summary>
        /// <param name="infos"></param>
        public void DeletedDownloadings(IList infos)
        {
            bool isDownloading = false;
            while (infos.Count > 0)
            {
                var info = infos[0] as DownloadInfo;
                if (info != null)
                {
                    if (info.DownloadState == DownloadState.Downloading)
                        isDownloading = true;
                    CancelDownload(info);
                    DeletedFile(info.LocalFileName);
                    DownloadingViewModel.Instance.DataInfos.Remove(info);
                }
            }
            DownloadingViewModel.Instance.SubmitChange();
            if (isDownloading)
                ReadyToNextDownload();
            DownloadingChange();
        }

        /// <summary>
        /// 删除全部正在下载任务
        /// </summary>
        public void DeletedAllDownloading()
        {
            foreach (var info in DownloadingItems)
                DeletedFile(info.LocalFileName);
            CancelAll();
            DownloadingViewModel.Instance.RemoveAll();
            _isDownloading = false;
            DownloadingChange();
        }

        /// <summary>
        /// 批量删除已下载
        /// </summary>
        /// <param name="infos"></param>
        public void DeletedDownloadeds(IList infos)
        {
            while (infos.Count > 0)
            {
                var info = infos[0] as DownloadInfo;
                if (info != null)
                {
                    DownloadedViewModel.Instance.DataInfos.Remove(info);
                    DeletedFile(info.LocalFileName);
                }
            }
            DownloadedViewModel.Instance.SubmitChange();
        }

        /// <summary>
        /// 删除全部已下载
        /// </summary>
        public void DeletedAllDownloaded()
        {
            foreach (var down in DownloadedItems)
            {
                DeletedFile(down.LocalFileName);
            }
            DownloadedViewModel.Instance.RemoveAll();
        }

        /// <summary>
        /// 继续一个下载
        /// </summary>
        /// <param name="info"></param>
        public void ResumeDownload(DownloadInfo info)
        {
            if (_isDownloading)
            {
                info.DownloadState = DownloadState.Await;
            }
            else
            {
                ReadyToDownload(info);
            }
        }

        /// <summary>
        /// 暂停一个下载
        /// </summary>
        /// <param name="info"></param>
        public void PauseDownload(DownloadInfo info)
        {
            PauseOneDownload(info);
            ReadyToNextDownload();
        }

        /// <summary>
        /// 全部暂停
        /// </summary>
        public void PauseAllDownload()
        {
            foreach (var down in DownloadingItems)
            {
                if (down.DownloadState == DownloadState.Downloading)
                    PauseOneDownload(down);
                else
                    down.DownloadState = DownloadState.Pause;
            }
            _isDownloading = false;
        }

        /// <summary>
        /// 重启下载
        /// </summary>
        public void RestartDownload()
        {
            foreach (var download in DownloadingItems)
            {
                if (download.DownloadState != DownloadState.Downloading)
                    download.DownloadState = DownloadState.Pause;
                else
                    //_isDownloading = true;
                    RestartOneDonwload(download);
            }
            //DiscoverActiveDownloadsAsync(DownloadingItems);
            Task.Run(() =>
            {
                CheckRubbishDownload();
            });
        }

        /// <summary>
        /// 处理一个下载
        /// </summary>
        /// <param name="downInfo"></param>
        public void HandleDownload(DownloadInfo downInfo)
        {
            switch (downInfo.DownloadState)
            {
                case DownloadState.Downloading:
                    DownloadViewModel.Instance.PauseDownload(downInfo);
                    break;
                case DownloadState.Pause:
                case DownloadState.PlayInfoError:
                case DownloadState.DownloadError:
                    DownloadViewModel.Instance.ResumeDownload(downInfo);
                    break;
                case DownloadState.Await:
                    downInfo.DownloadState = DownloadState.Pause;
                    break;
                case DownloadState.WaitingForWiFi:
                case DownloadState.WaitingForExternalPower:
                    DownloadViewModel.Instance.RestartOneDonwload(downInfo);
                    break;
            }
        }

        protected override void DownloadProgress(BackgroundTransferRequest request, DownloadInfo info)
        {
            var handler = _dispatcher.BeginInvoke(() =>
            {
                double percent = 0;
                if (request.TotalBytesToReceive > 0)
                    percent = request.BytesReceived * 100 / request.TotalBytesToReceive;
                if (request.TotalBytesToReceive != info.TotalBytesReceive)
                    info.TotalBytesReceive = request.TotalBytesToReceive;
                info.BytesReceived = request.BytesReceived;
                info.DownloadPercent = percent;
                DownloadingViewModel.Instance.SubmitChange();
            });
        }

        protected override void DownloadCompleted(DownloadInfo info)
        {
            WAYLocalFactory.Instance.AddDownloadCount();
            try
            {
                using (var isoStore = IsolatedStorageFile.GetUserStoreForApplication())
                {
                    var localFile = string.Format("/PersonalCenter/MyDownload/{0}.mp4", info.Title);
                    if (isoStore.FileExists(localFile))
                        isoStore.DeleteFile(localFile);
                    isoStore.MoveFile(info.LocalFileName, localFile);
                    info.LocalFileName = localFile;
                }
                
            }
            catch
            { }
            var handler = _dispatcher.BeginInvoke(() =>
            {
                info.DownloadState = DownloadState.Downloaded;
                DownloadingViewModel.Instance.DeleteRecord(info);
                DownloadedViewModel.Instance.InsertRecord(info);
                DownloadingChange();
                ReadyToNextDownload();
            });
        }

        protected override void DownloadFailture(DownloadInfo info, string errMessage)
        {
            var handler = _dispatcher.BeginInvoke(() =>
            {
                info.DownloadState = DownloadState.DownloadError;
                ReadyToNextDownload();
            });
        }

        private void DownloadingChange()
        {
            ChannelTypeFactory.Instance.LocalTypes[2].Count = DownloadingItems.Count;
            if (DownloadingCountChannge != null)
                DownloadingCountChannge();
        }

        private void ReadyToNextDownload()
        {
            var downInfo = DownloadingItems.FirstOrDefault(v => v.DownloadState == DownloadState.Await);
            if (downInfo != null)
            {
                ReadyToDownload(downInfo);
            }
            else
            {
                _isDownloading = false;
            }
        }

        private void ReadyToDownload(DownloadInfo downInfo)
        {
            var result = ResumeOneDownload(downInfo);
            if (!result)
            {
                _isDownloading = true;
                downInfo.DownloadState = DownloadState.Downloading;
                var playInfoFactory = new Play2InfoFactory(true, true);
                playInfoFactory.HttpSucessHandler += (o, e) =>
                {
                    downInfo.DownloadUri = playInfoFactory.CreatePlayUri();
                    StartOneDownload(downInfo);
                    DownloadingViewModel.Instance.SubmitChange();
                };
                playInfoFactory.HttpFailorTimeOut = () =>
                {
                    var handler = _dispatcher.BeginInvoke(() =>
                    {
                        downInfo.DownloadState = DownloadState.PlayInfoError;
                        ReadyToNextDownload();
                    });
                };
                playInfoFactory.DownLoadDatas(downInfo.ChannelId);
            }
        }

        private void DeletedFile(string fileName)
        {
            try
            {
                using (var isoStore = IsolatedStorageFile.GetUserStoreForApplication())
                {
                    if (isoStore.FileExists(fileName))
                        isoStore.DeleteFile(fileName);
                }
            }
            catch { }
        }

        /// <summary>
        /// 删除不存在关联的本地文件
        /// </summary>
        private void CheckRubbishDownload()
        {
            try
            {
                using (var isoStore = IsolatedStorageFile.GetUserStoreForApplication())
                {
                    var locals = isoStore.GetFileNames("/PersonalCenter/MyDownload/*");
                    var files = locals.Where(v => v.Contains(".mp4")).Select(v => v);
                    var rubbishs = files.Except(DownloadingItems.Select(v => v.LocalFileName)).Except(DownloadedItems.Select(v => v.LocalFileName));
                    foreach (var rubbish in rubbishs)
                    {
                        isoStore.DeleteFile(rubbish);
                    }
                }
            }
            catch { }
        }

    }
}
