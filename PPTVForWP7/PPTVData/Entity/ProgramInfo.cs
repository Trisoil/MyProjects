using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

namespace PPTVData.Entity
{
    public class ProgramInfo:System.ComponentModel.INotifyPropertyChanged
    {
        private int _index;
        private int _channelVID;
        private string _title;
        private string _playLink;
        private System.Windows.Visibility _isDown;
        public ProgramInfo() { }
        private string _slotUrl;
        public ProgramInfo(int channelVID, string title, string playLink, string slotUrl,int index,System.Windows.Visibility isDown)
        {
            _channelVID = channelVID;
            _title = title;
            _playLink = playLink;
            _slotUrl = slotUrl;
            _index = index;
            _isDown = isDown;
        }
        public int Index { get { return _index; } }
        public int ChannelVID { get { return _channelVID; } }
        public string Title { get { return _title; } }
        public string PlayLink { get { return _playLink; } }
        public string SlotUrl { get { return _slotUrl; } }

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

        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }
   
}
