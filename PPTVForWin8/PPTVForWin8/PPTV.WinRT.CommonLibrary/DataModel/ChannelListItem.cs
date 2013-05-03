using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.DataModel
{
    using PPTVData.Entity;

    public class ChannelListItem : BindableBase
    {
        public int ChannelId { get; set; }

        public int ChannelFlag { get; set; }

        public double Mark { get; set; }

        public ChannelTypeItem TypeInfo { get; set; }

        private string _title;
        public string Title
        {
            get { return _title; }
            set { this.SetProperty(ref _title, value); }
        }

        private string _imageUri;
        public string ImageUri
        {
            get { return _imageUri; }
            set { this.SetProperty(ref _imageUri, value); }
        }
    }
}
