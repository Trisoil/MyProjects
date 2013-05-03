using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml.Controls;

namespace PPTV.WinRT.CommonLibrary.Factory.Play
{
    using PPTVData.Entity;

    using PPTV.WinRT.CommonLibrary.DataModel.Play;

    public abstract class PlayFactoryBase : IPlayFactory
    {
        private DateTime _beginTime;

        protected MediaElement _mediaPlayer;
        protected DACPlayInfo _dacPlayInfo;
        protected PlayMessageInfo _playMessage;

        public PlayFactoryBase()
        {
            InitInformation();
        }

        public abstract void Play();

        public Action<PlayMessageInfo> PlayMessageAction { get; set; }
        public Action<string> PlayErrorAction { get; set; }
        public Action HttpFailOrTimeoutAction { get; set; }

        public DACPlayInfo DacPlayInfo
        {
            get { return _dacPlayInfo; }
        }

        public void Play(MediaElement mediaPlayer)
        {
            _mediaPlayer = mediaPlayer;
            Play();
        }

        public virtual void End()
        {
            _mediaPlayer.Stop();
            _dacPlayInfo.stopReason = 0;

            var playTime = _beginTime == DateTime.MinValue ? 0 : (int)DateTime.Now.Subtract(_beginTime).TotalSeconds;
            var duration = (int)_mediaPlayer.NaturalDuration.TimeSpan.TotalSeconds;
            var position = (int)_mediaPlayer.Position.TotalSeconds;

            SavePlayDAC(playTime, duration);
            SavePlayHistory(duration, position);
            SavePlayWAY(playTime, duration);
        }

        public virtual bool SupportSeek
        {
            get { return true; }
        }

        public virtual void PreProgram()
        {
        }

        public virtual void NextProgram()
        {
        }

        public virtual void BtirateSelect(PlayFileType fileType)
        {
        }

        protected void InitInformation()
        {
            _beginTime = DateTime.Now;
            _dacPlayInfo = new DACPlayInfo();
            _playMessage = new PlayMessageInfo();
        }

        protected virtual void SavePlayDAC(int playTime, int duration)
        {
        }

        protected virtual void SavePlayHistory(int duration, int position)
        {
        }

        protected virtual void SavePlayWAY(int playTime, int duration)
        {
        }
    }
}
