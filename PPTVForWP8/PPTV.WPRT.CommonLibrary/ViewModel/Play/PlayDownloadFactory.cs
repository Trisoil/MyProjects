using System;
using System.Collections.Generic;
using System.IO.IsolatedStorage;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Play
{
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Factory.WAY;
    using PPTVData.Entity.Cloud;
    using PPTVData.Factory.Cloud;

    using PPTV.WPRT.CommonLibrary.DataModel.Play;
    using PPTV.WPRT.CommonLibrary.DataModel.Download;
    using PPTV.WPRT.CommonLibrary.ViewModel.Download;

    public class PlayDownloadFactory : PlayFactoryBase
    {
        DownloadInfo _downloadInfo;
        public PlayDownloadFactory(int channelId)
        {
            _downloadInfo = DownloadViewModel.Instance.DownloadedItems.FirstOrDefault(v => v.ChannelId == channelId);
        }

        public override void Play()
        {
            try
            {
                _playMessage.Title = _downloadInfo.Title;
                _playMessage.ShareInfo = new PlayShareInfo(_downloadInfo.ChannelId, _playMessage.Title, _downloadInfo.ImageUri);

                using (var isoStore = IsolatedStorageFile.GetUserStoreForApplication())
                {
                    var stream = isoStore.OpenFile(_downloadInfo.LocalFileName, System.IO.FileMode.Open);
                    _mediaPlayer.SetSource(stream);
                }

                if (PlayMessageAction != null)
                    PlayMessageAction(_playMessage);
            }
            catch (Exception ex)
            {
                if (PlayErrorAction != null)
                    PlayErrorAction(ex.Message);
            }
        }

        protected override void SavePlayHistory(int duration, int position)
        {
            var parentId = _downloadInfo.ParentId;
            var parentName = _downloadInfo.ParentName;
            var image = _downloadInfo.ImageUri;
            var subId = _downloadInfo.ChannelId;
            var subName = _downloadInfo.Title;
            var index = _downloadInfo.ProgramIndex;

            var localInfo = new CloudDataInfo();
            localInfo.ProgramIndex = index;
            localInfo.ImageUri = image;
            localInfo.Id = parentId;
            localInfo.Name = parentName;
            localInfo.SubId = subId;
            localInfo.SubName = subName;
            localInfo.VideoType = 3;
            localInfo.Pos = position;
            localInfo.Duration = duration;

            LocalRecentFactory.Instance.InsertLocalRecord(localInfo);
        }

        protected override void SavePlayDAC(int playTime, int duration)
        {
            _dacPlayInfo.title = _downloadInfo.Title;
            _dacPlayInfo.type = _downloadInfo.TypeId;
            _dacPlayInfo.allTime = duration;
            _dacPlayInfo.programSource = (int)DACPageType.LOCALPLAY;

            _dacPlayInfo.vid = _downloadInfo.ChannelId;
            _dacPlayInfo.playTime = playTime;
            DACFactory.Instance.Play(_dacPlayInfo, 3);
        }

        protected override void SavePlayWAY(int playTime, int duration)
        {
            Task.Run(() =>
            {
                WAYLocalFactory.Instance.AddVodWatchTime(
                    _downloadInfo.TypeId, _downloadInfo.ChannelId, playTime, duration, true);
            });
        }
    }
}
