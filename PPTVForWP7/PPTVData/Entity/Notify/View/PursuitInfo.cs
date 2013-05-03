using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Data.Linq.Mapping;
using System.ComponentModel;

namespace PPTVData.Entity.Notify
{
    [Table]
    public class PursuitInfo : INotifyPropertyChanged
    {
        [Column(IsPrimaryKey = true, IsDbGenerated = true)]
        public int Id { get; set; }

        /// <summary>
        /// 频道ID
        /// </summary>
        [Column]
        public int ChannelId { get; set; }

        /// <summary>
        /// 频道名称
        /// </summary>
        [Column]
        public string ChannelName { get; set; }

        /// <summary>
        /// 频道缩略图
        /// </summary>
        [Column]
        public string ImageUrl { get; set; }

        /// <summary>
        /// 评分
        /// </summary>
        [Column]
        public double Mark { get; set; }

        /// <summary>
        /// 已更新到多少集
        /// </summary>
        [Column]
        public int ChannelCount { get; set; }

        private bool _isShowDelte;

        /// <summary>
        /// 是否显示编辑按钮
        /// </summary>
        public bool IsShowDelte
        {
            get
            {
                return _isShowDelte;
            }
            set
            {
                _isShowDelte = value;
                NotifyPropertyChanged("IsShowDelte");
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
