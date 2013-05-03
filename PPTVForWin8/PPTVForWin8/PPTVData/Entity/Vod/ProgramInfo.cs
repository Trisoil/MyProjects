using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

namespace PPTVData.Entity
{
    public class ProgramInfo : INotifyPropertyChanged
    {
        public int Index { get; set; }

        public int ChannelId { get; set; }

        public string Title { get; set; }

        public string SlotUri { get; set; }

        private bool _isSelected;
        public bool IsSelected
        {
            get { return _isSelected; }
            set
            {
                _isSelected = value;
                NotifyPropertyChanged("IsSelected");
            }
        }

        private bool _isDownloading;
        public bool IsDownloading 
        {
            get { return _isDownloading; }
            set 
            {
                _isDownloading = value;
                NotifyPropertyChanged("IsDownloading");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }
   
}
