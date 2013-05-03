using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.Factory.Play
{
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Entity.Cloud;
    using PPTVData.Factory.Cloud;
    using PPTVData.Factory.WAY;

    using PPTV.WinRT.CommonLibrary.Utils;
    using PPTV.WinRT.CommonLibrary.DataModel.Play;

    public class PlayVodFactory : PlayFactoryBase
    {
        PlayInfoHelp _playInfo;
        PlayFileType _playFileType;

        ChannelDetailFactory _channelDetailFactory;
        Play2InfoFactory _playInfoFactory;

        public PlayVodFactory(object parameter)
        {
            _playFileType = PlayFileType.HD;
            _playInfo = parameter as PlayInfoHelp;

            _channelDetailFactory = new ChannelDetailFactory();
            _channelDetailFactory.HttpSucessHandler += _channelDetailFactory_HttpSucceed;
            _channelDetailFactory.HttpFailorTimeOut += httpHandler_HttpTimeoutorFail;

            _playInfoFactory = new Play2InfoFactory();
            _playInfoFactory.HttpSucessHandler += _playInfoFactory_HttpSucceed;
            _playInfoFactory.HttpFailorTimeOut = httpHandler_HttpTimeoutorFail;
        }

        public override void Play()
        {
            if (_playInfo.ChannelDetailInfo == null)
                BeginGetDetailInfo();
            else
                BeginGetPlayInfo();
        }

        private void BeginGetDetailInfo()
        {
            _channelDetailFactory.DownLoadDatas(_playInfo.ChannelVID);
        }

        private async void BeginGetPlayInfo()
        {
            if (_playInfo != null
                && _playInfo.ChannelDetailInfo != null
                && _playInfo.ChannelDetailInfo.ProgramInfos != null)
            {
                var historyWatch = await LocalRecentFactory.Instance.GetRecord(_playInfo.ChannelVID);
                if (historyWatch != null
                    && ChannelUtils.JudgeProgreamIndex(_playInfo.ProgramIndex, historyWatch.ProgramIndex, _playInfo.ChannelDetailInfo))
                    _playMessage.StartPosition = historyWatch.Pos;

                var vvid = Guid.NewGuid().ToString();
                _dacPlayInfo.vvid = vvid;
                _playMessage.Title = ChannelUtils.CreateChannelTitle(_playInfo.ProgramIndex, _playInfo.ChannelDetailInfo, false);
                _playInfoFactory.DownLoadDatas(ChannelUtils.CreateChannelId(_playInfo.ProgramIndex, _playInfo.ChannelDetailInfo), vvid);
            }
            else
            {
                if (PlayErrorAction != null)
                    PlayErrorAction("无法获取影片播放地址");
            }
        }

        private void Play(PlayInfo item)
        {
            _mediaPlayer.Source = new Uri(_playInfoFactory.CreatePlayUri(item.ft));

            _dacPlayInfo.fn = item.Bitrate;
            _dacPlayInfo.ft = item.ft;
            _dacPlayInfo.mp4Name = item.Rid;
        }

        #region HTTPHandler

        void _channelDetailFactory_HttpSucceed(object sender, HttpFactoryArgs<ChannelDetailInfo> args)
        {
            _playInfo.UpdateChannelDetailInfo(args.Result);
            BeginGetPlayInfo();
        }

        void _playInfoFactory_HttpSucceed(object sender, HttpFactoryArgs<Object> args)
        {
            _playMessage.PlayInfos = _playInfoFactory.PlayInfos;
            _playMessage.HasPreProgram = ChannelUtils.CreatePreviousIndex(_playInfo.ProgramIndex, _playInfo.ChannelDetailInfo) >= 0 ? true : false;
            _playMessage.HasNextProgram = ChannelUtils.CreateNextIndex(_playInfo.ProgramIndex, _playInfo.ChannelDetailInfo) >= 0 ? true : false;
            _playMessage.ShareInfo = new PlayShareInfo(
                ChannelUtils.CreateChannelId(_playInfo.ProgramIndex, _playInfo.ChannelDetailInfo),
                _playMessage.Title, _playInfo.ChannelDetailInfo.ImageUri);

            var info = _playInfoFactory.CreatePlayInfo(_playFileType);
            if (info != null)
            {
                _playMessage.CurrentPlayInfo = info;
                Play(info);
                if (PlayMessageAction != null)
                    PlayMessageAction(_playMessage);
            }
            else
            {
                if (PlayErrorAction != null)
                    PlayErrorAction("无法获取影片播放地址");
            }
        }

        void httpHandler_HttpTimeoutorFail()
        {
            if (HttpFailOrTimeoutAction != null)
                HttpFailOrTimeoutAction();
        }

        #endregion

        public override void End()
        {
            _channelDetailFactory.Cancel();
            _playInfoFactory.Cancel();

            if (_playInfo == null
                || _playInfo.ChannelDetailInfo == null
                || _playInfo.ChannelDetailInfo.ProgramInfos == null
                || _playInfo.ChannelDetailInfo.ProgramInfos.Count <= 0)
            {
                return;
            }

            base.End();
        }

        public override void PreProgram()
        {
            var preIndex = ChannelUtils.CreatePreviousIndex(_playInfo.ProgramIndex, _playInfo.ChannelDetailInfo);
            if (preIndex >= 0)
            {
                End();
                InitInformation();
                _playInfo.UpdateProgramIndex(preIndex);
                BeginGetPlayInfo();
            }
        }

        public override void NextProgram()
        {
            int nextIndex = ChannelUtils.CreateNextIndex(_playInfo.ProgramIndex, _playInfo.ChannelDetailInfo);
            if (nextIndex >= 0)
            {
                End();
                InitInformation();
                _playInfo.UpdateProgramIndex(nextIndex);
                BeginGetPlayInfo();
            }
        }

        public override void BtirateSelect(PlayFileType fileType)
        {
            _playFileType = fileType;
            _playMessage.StartPosition = (int)_mediaPlayer.Position.TotalSeconds;

            var item = _playInfoFactory.CreatePlayInfo(_playFileType);
            if (item != null)
                Play(item);
        }

        protected override void SavePlayDAC(int playTime, int duration)
        {
            _dacPlayInfo.title = _playInfo.ChannelDetailInfo.Title;
            _dacPlayInfo.type = _playInfo.ChannelDetailInfo.Type;
            _dacPlayInfo.allTime = duration;

            _dacPlayInfo.vid = _playInfo.ChannelDetailInfo.Id;
            _dacPlayInfo.playTime = playTime;
            DACFactory.Instance.Play(_dacPlayInfo, 3);
        }

        protected override void SavePlayHistory(int duration, int position)
        {
            var parentId = _playInfo.ChannelDetailInfo.Id;
            var parentName = _playInfo.ChannelDetailInfo.Title;
            var image = _playInfo.ChannelDetailInfo.ImageUri;
            var subId = ChannelUtils.CreateChannelId(_playInfo.ProgramIndex, _playInfo.ChannelDetailInfo);
            var subName = ChannelUtils.CreateChannelTitle(_playInfo.ProgramIndex, _playInfo.ChannelDetailInfo, true);
            var index = ChannelUtils.CreateComonIndex(_playInfo.ProgramIndex, _playInfo.ChannelDetailInfo);

            if (parentId <= 0) return;

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

        protected override void SavePlayWAY(int playTime, int duration)
        {
            Task.Run(() => 
            {
                WAYLocalFactory.Instance.AddVodWatchTime(
                    _playInfo.ChannelDetailInfo.Type, _playInfo.ChannelDetailInfo.Id, playTime, duration, false); 
            });
        }
    }
}
