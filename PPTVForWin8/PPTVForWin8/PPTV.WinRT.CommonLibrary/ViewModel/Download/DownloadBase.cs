using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using Windows.Storage;
using Windows.Storage.AccessCache;
using Windows.Networking.BackgroundTransfer;

namespace PPTV.WinRT.CommonLibrary.ViewModel.Download
{
    using PPTV.WinRT.CommonLibrary.DataModel.Download;

    public abstract class DownloadBase
    {
        protected Dictionary<int, CancellationTokenSource> _cts;
        protected Dictionary<int, DownloadOperation> _activeDownloads;

        protected bool _isDownloading;

        public static string DownloadFolderToken = "PPTVDOWNLOADFOLDERTOKEN";

        public DownloadBase()
        {
            _cts = new Dictionary<int, CancellationTokenSource>();
            _activeDownloads = new Dictionary<int, DownloadOperation>();
        }

        protected abstract void DownloadCompleted(DownloadInfo info);
        protected abstract void DownloadProgress(DownloadOperation download, DownloadInfo info);
        protected abstract void DownloadFailture(DownloadInfo info, string errMessage);

        public static async Task<IStorageFolder> GetDownloadFolder()
        {
            IStorageFolder folder = ApplicationData.Current.LocalFolder;
            try
            {
                folder = await StorageApplicationPermissions.FutureAccessList.GetFolderAsync(DownloadFolderToken);
            }
            catch { }
            
            return folder;
        }

        /// <summary>
        /// 恢复已经开始的下载
        /// </summary>
        /// <param name="infos"></param>
        /// <returns></returns>
        public async void DiscoverActiveDownloadsAsync(ObservableCollection<DownloadInfo> infos)
        {
            _activeDownloads.Clear();
            _cts.Clear();

            DownloadInfo info = null;
            try
            {
                var downloads = await BackgroundDownloader.GetCurrentDownloadsAsync();
                if (downloads.Count > 0)
                {
                    await Task.Run(async () =>
                    {
                        var tasks = new List<Task>(downloads.Count);
                        foreach (var download in downloads)
                        {
                            info = infos.FirstOrDefault(v => v.DownloadGuid == download.Guid);
                            if (info != null)
                                tasks.Add(HandleDownloadAsync(download, info, false));
                        }
                        await Task.WhenAll(tasks);
                    });
                }
                else
                {
                    _isDownloading = false;
                    foreach (var downInfo in infos)
                    {
                        downInfo.DownloadState = DownloadState.Pause;
                    }
                }
            }
            catch (Exception ex)
            {
                _isDownloading = false;
                if (info != null)
                    DownloadFailture(info, ex.Message);
            }
        }

        /// <summary>
        /// 开始下载一个新任务，此方法为后台线程调用
        /// </summary>
        /// <param name="info"></param>
        protected async void StartOneDownload(DownloadInfo info)
        {
            try
            {
                _isDownloading = true;

                var folder = await GetDownloadFolder();
                var destinationFile = await folder.
                    CreateFileAsync(info.LocalFileName, CreationCollisionOption.ReplaceExisting);
                info.LocalFileName = destinationFile.Name;

                var downloader = new BackgroundDownloader();
                var download = downloader.CreateDownload(new Uri(info.DownloadUri), destinationFile);
                info.DownloadGuid = download.Guid;
                await HandleDownloadAsync(download, info, true);
            }
            catch (Exception ex)
            {
                _isDownloading = false;
                DownloadFailture(info, ex.Message);
            }
        }

        /// <summary>
        /// 暂停一个下载任务
        /// </summary>
        /// <param name="info"></param>
        protected void PauseOneDownload(DownloadInfo info)
        {
            if (_activeDownloads.ContainsKey(info.ChannelId)
                && (_activeDownloads[info.ChannelId].Progress.Status == BackgroundTransferStatus.Running
                || _activeDownloads[info.ChannelId].Progress.Status == BackgroundTransferStatus.PausedNoNetwork
                || _activeDownloads[info.ChannelId].Progress.Status == BackgroundTransferStatus.PausedCostedNetwork)
                )
            {
                _activeDownloads[info.ChannelId].Pause();
                info.DownloadState = DownloadState.Pause;
            }
        }

        /// <summary>
        /// 继续一个下载任务
        /// </summary>
        /// <param name="info"></param>
        protected bool ResumeOneDownload(DownloadInfo info)
        {
            if (_activeDownloads.ContainsKey(info.ChannelId)
                && _activeDownloads[info.ChannelId].Progress.Status == BackgroundTransferStatus.PausedByApplication)
            {
                _isDownloading = true;
                _activeDownloads[info.ChannelId].Resume();
                info.DownloadState = DownloadState.Downloading;
                return true;
            }
            return false; 
        }

        protected void CancelDownload(DownloadInfo info)
        {
            if (_cts.ContainsKey(info.ChannelId))
            {
                _cts[info.ChannelId].Cancel();
                _cts[info.ChannelId].Dispose();
            }
            _cts.Remove(info.ChannelId);
        }

        protected void CancelAll()
        {
            foreach (var cts in _cts.Values)
            {
                cts.Cancel();
                cts.Dispose();
            }
            _cts.Clear();
        }

        private async Task HandleDownloadAsync(DownloadOperation download, DownloadInfo info, bool start)
        {
            try
            {
                _activeDownloads[info.ChannelId] = download;
                _cts[info.ChannelId] = new CancellationTokenSource();

                var progressCallback = new Progress<DownloadOperation>(downOperation =>
                {
                    DownloadProgress(downOperation, info);
                });
                if (start)
                {
                    await download.StartAsync().AsTask(_cts[info.ChannelId].Token, progressCallback);
                }
                else
                {
                    await download.AttachAsync().AsTask(_cts[info.ChannelId].Token, progressCallback);
                }

                _activeDownloads.Remove(info.ChannelId);
                _cts.Remove(info.ChannelId);
                var response = download.GetResponseInformation();
                _isDownloading = false;
                DownloadCompleted(info);
            }
            catch (System.Threading.Tasks.TaskCanceledException) { }
            catch (Exception ex)
            {
                _isDownloading = false;
                DownloadFailture(info, ex.Message);
            }
        }
    }
}
