using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace PPTVData.Entity.Cloud
{
    public class CloudDataInfo : INotifyPropertyChanged
    {
        public CloudDataInfo()
        {
            this.DurationInfo = new CloudDurationInfo()
            {
                Pos = this.Pos,
                ProgramIndex = this.ProgramIndex
            };
        }

        public string UUID { get; set; }

        public int Device { get; set; }

        public int DeviceHistory { get; set; }

        public int Id { get; set; }

        public int ClId { get; set; }

        public string Name { get; set; }

        public string SubName { get; set; }

        public int SubId { get; set; }

        public int Duration { get; set; }

        public long ModifyTime { get; set; }

        public int Property { get; set; }

        public int VideoType { get; set; }

        public int Bt { get; set; }

        public int Mode { get; set; }

        public CloudDurationInfo DurationInfo { get; set; }

        public CloudInfoEnum DataType { get; set; }

        private int _programIndex;
        public int ProgramIndex
        {
            get { return _programIndex; }
            set
            {
                _programIndex = value;
                if (this.DurationInfo.ProgramIndex == 0)
                    this.DurationInfo.ProgramIndex = _programIndex;
            }
        }

        private int _pos;
        public int Pos
        {
            get { return _pos; }
            set
            {
                _pos = value;
                if (this.DurationInfo.Pos == 0)
                    this.DurationInfo.Pos = _pos;
            }
        }

        private string _imageUri;
        /// <summary>
        /// 频道缩略图
        /// </summary>
        public string ImageUri
        {
            get { return _imageUri; }
            set {
                _imageUri = value;
                NotifyPropertyChanged("ImageUri");
            }
        }

        private double _mark;
        /// <summary>
        /// 频道评分
        /// </summary>
        public double Mark
        {
            get { return _mark; }
            set {
                _mark = value;
                NotifyPropertyChanged("Mark");
            }
        }

        public override bool Equals(object obj)
        {
            var o = obj as CloudDataInfo;
            if (o == null) return false;
            if (o == this) return true;
            return o.Id == Id;
        }

        public override int GetHashCode()
        {
            return Id + Name.GetHashCode();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    public enum CloudInfoEnum
    {
        Normal = 0,
        Add,
        Update,
        Delete
    }

    public class CloudDurationInfo
    {
        public int ProgramIndex { get; set; }

        public int Pos { get; set; }
    }
}
