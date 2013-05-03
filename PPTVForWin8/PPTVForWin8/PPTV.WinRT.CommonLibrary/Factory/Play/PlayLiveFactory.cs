using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.Media;

namespace PPTV.WinRT.CommonLibrary.Factory.Play
{
    using PPTVData.Utils;
    using PPTVData.Factory;
    using PPTVData.Entity.Live;
    using PPTVData.Factory.Live;
    using PPTVData.Factory.WAY;

    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.DataModel.Play;

    public class PlayLiveFactory : PlayFactoryBase
    {
        LiveListItem _liveInfo;
        MediaExtensionManager _mediaManage;

        public PlayLiveFactory(object parameter)
        {
            _liveInfo = parameter as LiveListItem;

            _mediaManage = new MediaExtensionManager();
            _mediaManage.RegisterSchemeHandler("PpboxSource.PpboxSchemeHandler", "ppvod:");
            _mediaManage.RegisterSchemeHandler("PpboxSource.PpboxSchemeHandler", "pplive3:");
        }

        public override bool SupportSeek
        {
            get
            {
                return false;
            }
        }

        public override void Play()
        {
            if (LiveCenterFactoryBase.IsDirectPlay(_liveInfo.ChannelInfo.LiveType))
                _playMessage.Title = _liveInfo.ChannelInfo.NowPlay;
            else
                _playMessage.Title = _liveInfo.ChannelInfo.Title;

            var vvid = Guid.NewGuid().ToString();
            _dacPlayInfo.vvid = vvid;
            _playMessage.ShareInfo = new PlayShareInfo(_liveInfo.ChannelInfo.Id, _playMessage.Title, _liveInfo.ChannelInfo.ImageUri);
            _mediaPlayer.Source = new Uri(string.Format("pplive3://///{0}?auth={1}&platform={2}&type={3}&vvid={4}",
                _liveInfo.ChannelInfo.Id, EpgUtils.Auth, EpgUtils.PlatformName, EpgUtils.CreatePlayType(false), vvid));

            if (PlayMessageAction != null)
                PlayMessageAction(_playMessage);
        }

        protected override void SavePlayDAC(int playTime, int duration)
        {
            _dacPlayInfo.title = _playMessage.Title;
            _dacPlayInfo.vid = _liveInfo.ChannelInfo.Id;
            _dacPlayInfo.playTime = playTime;

            DACFactory.Instance.Play(_dacPlayInfo, 4);
        }

        protected override void SavePlayWAY(int playTime, int duration)
        {
            Task.Run(() =>
            {
                WAYLocalFactory.Instance.AddLiveTime(playTime);
            });
        }
    }
}
