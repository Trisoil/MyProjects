using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Threading.Tasks;

using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.Networking.BackgroundTransfer;

namespace PPTV.WinRT.CommonLibrary.ViewModel.Download
{
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Factory.WAY;
    using PPTVData.Utils;

    using PPTV.WinRT.CommonLibrary.DataModel.Download;

    public class DownloadViewModel : DownloadBase
    {
        private CoreDispatcher _dispatcher;

        public static readonly DownloadViewModel Instance = new DownloadViewModel();

        public Action DownloadingCountChannge;

        public DownloadViewModel()
        {
            _dispatcher = Window.Current.CoreWindow.Dispatcher;
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
            if (DownloadingCountChannge != null)
                DownloadingCountChannge();
        }

        /// <summary>
        /// 批量删除正在下载任务
        /// </summary>
        /// <param name="infos"></param>
        public void DeletedDownloadings(IList<DownloadInfo> infos)
        {
            bool isDownloading = false;
            foreach (var info in infos)
            {
                if (info.DownloadState == DownloadState.Downloading)
                    isDownloading = true;
                CancelDownload(info);
                DownloadingViewModel.Instance.DataInfos.Remove(info);
            }
            DownloadingViewModel.Instance.SubmitChange();
            if (isDownloading)
                ReadyToNextDownload();
            if (DownloadingCountChannge != null)
                DownloadingCountChannge();
        }

        /// <summary>
        /// 删除全部正在下载任务
        /// </summary>
        public void DeletedAllDownloading()
        {
            CancelAll();
            DownloadingViewModel.Instance.RemoveAll();
            _isDownloading = false;
            if (DownloadingCountChannge != null)
                DownloadingCountChannge();
        }

        /// <summary>
        /// 批量删除已下载
        /// </summary>
        /// <param name="infos"></param>
        public void DeletedDownloadeds(IList<DownloadInfo> infos)
        {
            foreach (var info in infos)
            {
                DownloadedViewModel.Instance.DataInfos.Remove(info);
                DeltedFile(info.LocalFileName);
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
                DeltedFile(down.LocalFileName);
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
                    _isDownloading = true;
            }
            DiscoverActiveDownloadsAsync(DownloadingItems);
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
            }
        }

        protected override void DownloadProgress(DownloadOperation download, DownloadInfo info)
        {
            var handler =_dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                double percent = 0;
                if (download.Progress.TotalBytesToReceive > 0)
                    percent = download.Progress.BytesReceived * 100 / download.Progress.TotalBytesToReceive;
                if (download.Progress.TotalBytesToReceive != info.TotalBytesReceive)
                    info.TotalBytesReceive = download.Progress.TotalBytesToReceive;
                info.BytesReceived = download.Progress.BytesReceived;
                info.DownloadPercent = percent;
                DownloadingViewModel.Instance.SubmitChange();
            });
        }

        protected override void DownloadCompleted(DownloadInfo info)
        {
            WAYLocalFactory.Instance.AddDownloadCount();
            var handler = _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                info.DownloadState = DownloadState.Downloaded;
                DownloadingViewModel.Instance.DeleteRecord(info);
                DownloadedViewModel.Instance.InsertRecord(info);
                if (DownloadingCountChannge != null)
                    DownloadingCountChannge();
                ReadyToNextDownload();
            });
        }

        protected override void DownloadFailture(DownloadInfo info, string errMessage)
        {
            var handler = _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                info.DownloadState = DownloadState.DownloadError;
                ReadyToNextDownload();
            });
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
                    var handler = _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        downInfo.DownloadState = DownloadState.PlayInfoError;
                        ReadyToNextDownload();
                    });
                };
                playInfoFactory.DownLoadDatas(downInfo.ChannelId);
            }
        }

        private async void DeltedFile(string fileName)
        {
            try
            {
                var folder = await DownloadBase.GetDownloadFolder();
                var file = await folder.GetFileAsync(fileName);
                await file.DeleteAsync();
            }
            catch { }
        }

        /// <summary>
        /// 删除不存在关联的本地文件
        /// </summary>
        private async void CheckRubbishDownload()
        {
            var folder = ApplicationData.Current.LocalFolder;
            try
            {
                var locals = await folder.GetFilesAsync();
                var files = locals.Where(v => v.Name.Contains(".mp4")).Select(v => v.Name);
                var rubbishs = files.Except(DownloadingItems.Select(v => v.LocalFileName)).Except(DownloadedItems.Select(v => v.LocalFileName));
                foreach (var rubbish in rubbishs)
                {
                    var file = await folder.GetFileAsync(rubbish);
                    await file.DeleteAsync();
                }
            }
            catch { }
        }

        #region Deleted

        ///// <summary>
        ///// 添加一个下载
        ///// </summary>
        ///// <param name="info"></param>
        //public void AddDownload(DownloadInfo info)
        //{
        //    var downloaded = DownloadedItems.FirstOrDefault(v => v.ChannelId == info.ChannelId);
        //    if (downloaded != null)
        //        DownloadedViewModel.Instance.DeleteRecord(downloaded);

        //    if (!_isDownloading)
        //    {
        //        ReadyToDownload(info);
        //    }

        //    DownloadingViewModel.Instance.InsertRecord(info);

        //    if (DownloadingCountChannge != null)
        //        DownloadingCountChannge();

        //}

        ///// <summary>
        ///// 删除一个正在下载任务
        ///// </summary>
        ///// <param name="info"></param>
        //public void DeletedDownloading(DownloadInfo info)
        //{
        //    CancelDownload(info);
        //    DownloadingViewModel.Instance.DeleteRecord(info);
        //    if (info.DownloadState == DownloadState.Downloading)
        //        ReadyToNextDownload();
        //    if (DownloadingCountChannge != null)
        //        DownloadingCountChannge();
        //}

        ///// <summary>
        ///// 删除一个已下载任务
        ///// </summary>
        ///// <param name="info"></param>
        //public void DeletedDownloaded(DownloadInfo info)
        //{
        //    DownloadedViewModel.Instance.DeleteRecord(info);
        //    DataCommonUtils.DeleteEntity(info.LocalFileName);
        //}

        #endregion
    }
}
