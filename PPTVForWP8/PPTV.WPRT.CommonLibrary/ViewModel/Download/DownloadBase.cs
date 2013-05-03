using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.IsolatedStorage;

using Microsoft.Phone.BackgroundTransfer;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Download
{
    using PPTV.WPRT.CommonLibrary.DataModel.Download;

    public abstract class DownloadBase
    {
        protected bool _isDownloading;

        protected abstract void DownloadCompleted(DownloadInfo info);
        protected abstract void DownloadProgress(BackgroundTransferRequest request, DownloadInfo info);
        protected abstract void DownloadFailture(DownloadInfo info, string errMessage);

        /// <summary>
        /// 开始下载一个新任务，此方法为后台线程调用
        /// </summary>
        /// <param name="info"></param>
        protected void StartOneDownload(DownloadInfo info)
        {
            _isDownloading = true;

            var destinationPath = string.Format("/shared/transfers/{0}", info.LocalFileName);
            var destinationFile = new Uri(destinationPath, UriKind.Relative);
            var transferRequest = new BackgroundTransferRequest(new Uri(info.DownloadUri), destinationFile);
            info.LocalFileName = transferRequest.DownloadLocation.OriginalString;
            HandleDownload(transferRequest, info, true);
        }

        /// <summary>
        /// 暂停一个下载任务
        /// </summary>
        /// <param name="info"></param>
        protected void PauseOneDownload(DownloadInfo info)
        {
            if (string.IsNullOrEmpty(info.RequestId)) return;

            var transferRequest = BackgroundTransferService.Find(info.RequestId);
            if (transferRequest != null 
                && transferRequest.TransferStatus == TransferStatus.Transferring)
            {
                BackgroundTransferService.Remove(transferRequest);
                transferRequest.Dispose();
                info.DownloadState = DownloadState.Pause;
            }
        }

        /// <summary>
        /// 继续一个下载任务
        /// </summary>
        /// <param name="info"></param>
        protected bool ResumeOneDownload(DownloadInfo info)
        {
            if (string.IsNullOrEmpty(info.RequestId)) return false;

            var transferRequest = BackgroundTransferService.Find(info.RequestId);
            if (transferRequest != null
                && transferRequest.TransferStatus == TransferStatus.Transferring)
            {
                try
                {
                    _isDownloading = true;
                    info.DownloadState = DownloadState.Downloading;
                    HandleDownload(transferRequest, info, false);
                    return true;
                }
                catch (Exception ex)
                {
                    DownloadFailture(info, ex.Message);
                    _isDownloading = false;
                }
            }
            else
            {
                _isDownloading = true;
                info.DownloadState = DownloadState.Downloading;
                transferRequest = new BackgroundTransferRequest(new Uri(info.DownloadUri), new Uri(info.LocalFileName, UriKind.Relative));
                HandleDownload(transferRequest, info, true);
                return true;
            }
            return false;
        }

        /// <summary>
        /// 重新启动一个下载任务
        /// </summary>
        /// <param name="info"></param>
        /// <returns></returns>
        protected bool RestartOneDonwload(DownloadInfo info)
        {
            if (string.IsNullOrEmpty(info.RequestId)) return false;

            var transferRequest = BackgroundTransferService.Find(info.RequestId);
            if (transferRequest != null
                && transferRequest.TransferStatus == TransferStatus.Transferring)
            {
                try
                {
                    _isDownloading = true;
                    info.DownloadState = DownloadState.Downloading;
                    HandleDownload(transferRequest, info, false);
                    return true;
                }
                catch (Exception ex)
                {
                    DownloadFailture(info, ex.Message);
                    _isDownloading = false;
                }
            }
            return false;
        }

        protected void CancelDownload(DownloadInfo info)
        {
            if (string.IsNullOrEmpty(info.RequestId)) return;
            
            var transferRequest = BackgroundTransferService.Find(info.RequestId);
            if (transferRequest != null)
            {
                BackgroundTransferService.Remove(transferRequest);
                transferRequest.Dispose();
            }
        }

        protected void CancelAll()
        {
            var requests = BackgroundTransferService.Requests;
            foreach (var request in requests)
            {
                BackgroundTransferService.Remove(request);
                request.Dispose();
            }
        }

        private void HandleDownload(BackgroundTransferRequest transferRequest, DownloadInfo info, bool isAddQueue)
        {
            transferRequest.TransferStatusChanged += (o, e) =>
            {
                switch(e.Request.TransferStatus)
                {
                    case TransferStatus.Completed:
                        if (e.Request.StatusCode == 200 || e.Request.StatusCode == 206)
                        {
                            DownloadCompleted(info);
                            BackgroundTransferService.Remove(e.Request);
                            e.Request.Dispose();
                        }
                        break;
                    case TransferStatus.WaitingForWiFi:
                    case TransferStatus.WaitingForNonVoiceBlockingNetwork:
                        info.DownloadState = DownloadState.WaitingForWiFi;
                        Utils.TileUtils.CreateBasicToast("需要在WiFi环境下才能下载");
                        break;
                    case TransferStatus.WaitingForExternalPower:
                    case TransferStatus.WaitingForExternalPowerDueToBatterySaverMode:
                        info.DownloadState = DownloadState.WaitingForExternalPower;
                        Utils.TileUtils.CreateBasicToast("需要外接电源才能下载");
                        break;
                    default:
                        break;
                }
            };
            transferRequest.TransferProgressChanged += (o, e) =>
            {
                DownloadProgress(e.Request, info);
            };

            if (isAddQueue)
            {
                transferRequest.Method = "GET";
                info.RequestId = transferRequest.RequestId;

                try
                {
                    if (BackgroundTransferService.Find(info.RequestId) == null)
                        BackgroundTransferService.Add(transferRequest);
                }
                catch (Exception ex)
                {
                    _isDownloading = false;
                    DownloadFailture(info, ex.Message);
                }
            }
        }
    }
}
