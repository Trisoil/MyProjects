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

namespace PPTVData.Entity.Social
{
    [Table]
    public class SocialDataInfo
    {
        [Column(IsPrimaryKey = true, IsDbGenerated = true)]
        public int Id { get; set; }

        [Column]
        public SocialType Type { get; set; }

        [Column]
        public bool IsBinded { get; set; }

        [Column]
        public string AcessToken { get; set; }

        [Column]
        public string RefreshToken { get; set; }

        [Column]
        public long Expire { get; set; }
    }

    public enum SocialType
    {
        SINA,
        TENCENT,
        RENREN
    }
}
