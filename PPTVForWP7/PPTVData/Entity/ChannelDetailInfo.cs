using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PPTVData.Entity
{
    public class ChannelDetailInfo : ChannelInfo, System.ComponentModel.INotifyPropertyChanged
    {
        public ChannelDetailInfo() : base() { }

        public ChannelDetailInfo(
            int vid,
            string title,
            string playLink,
            int typeID,
            string imgUrl,
            string catalogTags,
            string directorTags,
            string actTags,
            string yearTags,
            string areaTags,
            int state,
            string note,
            double mark,
            int bitrate,
            int resolutionWidth,
            int resolutionHeight,
            bool isHD,
            bool isRecommend,
            bool isNew,
            bool is3D,
            double duration,
            string content,
            string sloturl,
            List<ProgramInfo> programInfos,
            System.Windows.Visibility isDown)
            : base(vid, title, playLink, typeID, imgUrl, catalogTags, directorTags, actTags, yearTags, areaTags, state, note, mark, bitrate, resolutionWidth, resolutionHeight, isHD, isRecommend, isNew, is3D, duration, content, sloturl)
        {
            _programInfos = programInfos;
            _content = content;
            _isDown = isDown;
        }

        public bool IsNumber { get; set; }

        private List<ProgramInfo> _programInfos = new List<ProgramInfo>();
        private string _content;
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;

        public ChannelDetailInfo Clone()
        {
            ChannelDetailInfo newChannelDetailInfo = new ChannelDetailInfo(VID, Title, PlayLink, TypeID, ImgUrl, CatalogTags, DirectorTags, ActTags, YearTags, AreaTags, State, Note, Mark, Bitrate, ResolutionWidth, ResolutionHeight, IsHD, IsRecommend, IsNew, Is3D, Duration, Content, SlotUrl, new List<ProgramInfo>(), IsDown);
            foreach (var item in ProgramInfos)
            {
                ProgramInfo programInfo = new ProgramInfo(item.ChannelVID, item.Title, item.PlayLink, item.SlotUrl, item.Index, item.IsDown);
                newChannelDetailInfo.ProgramInfos.Add(programInfo);
            }
            return newChannelDetailInfo;
        }

        private void NotifyPropertyChanged(String info)
        {
            if (PropertyChanged != null)
            {
                System.Windows.Deployment.Current.Dispatcher.BeginInvoke(PropertyChanged, new object[] { this, new System.ComponentModel.PropertyChangedEventArgs(info) });
            }
        }
        public System.Windows.Visibility IsDown
        {
            get
            {
                return _isDown;
            }
            set
            {
                _isDown = value;

                NotifyPropertyChanged("IsDown");

            }
        }
        public List<ProgramInfo> ProgramInfos 
        { 
            get 
            { 
                return _programInfos; 
            } 
        }
        private string ContentStr { get { return _content; } }
        private System.Windows.Visibility _isDown;
    }
}
