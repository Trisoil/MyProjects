using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace PPTVData.Entity.WAY
{
    [DataContract]
    public class WAYPostInfo
    {
        public WAYPostInfo()
        {
            UserName = string.Empty;
            DeviceCode = string.Empty;
        }

        [DataMember(Name = "username")]
        public string UserName { get; set; }

        [DataMember(Name = "devicecode")]
        public string DeviceCode { get; set; }

        [DataMember(Name = "mtime")]
        public int MTime { get; set; }

        [DataMember(Name = "dtime")]
        public int DTime { get; set; }

        [DataMember(Name = "ctime")]
        public int CTime { get; set; }

        [DataMember(Name = "stime")]
        public int STime { get; set; }

        [DataMember(Name = "matime")]
        public int MATime { get; set; }

        [DataMember(Name = "datime")]
        public int DATime { get; set; }

        [DataMember(Name = "catime")]
        public int CATime { get; set; }

        [DataMember(Name = "satime")]
        public int SATime { get; set; }

        [DataMember(Name = "mvv")]
        public int MVV { get; set; }

        [DataMember(Name = "dvv")]
        public int DVV { get; set; }

        [DataMember(Name = "cvv")]
        public int CVV { get; set; }

        [DataMember(Name = "svv")]
        public int SVV { get; set; }

        [DataMember(Name = "nightvv")]
        public int NightVV { get; set; }

        [DataMember(Name = "holidayvv")]
        public int HolidayVV { get; set; }

        [DataMember(Name = "sharenum")]
        public int ShareNum { get; set; }

        [DataMember(Name = "feedbacknum")]
        public int FeedbackNum { get; set; }

        [DataMember(Name = "maxlivetime")]
        public int MaxLiveTime { get; set; }

        [DataMember(Name = "livebacknum")]
        public int LiveBackNum { get; set; }

        [DataMember(Name = "seeknum")]
        public int SeekNum { get; set; }

        [DataMember(Name = "downlaodnum")]
        public int DownloadNum { get; set; }

        [DataMember(Name = "viewdownloadnum")]
        public int ViewDownloadNum { get; set; }

        public bool IsWatch { get; set; }
    }
}
