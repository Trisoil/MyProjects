using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Entity.WAY
{
    [DataContract]
    public class WAYGetInfo
    {
        public WAYGetInfo()
        {
            UserType = -1;
        }

        [DataMember(Name = "iscool")]
        public byte IsCool { get; set; }

        [DataMember(Name = "daytype")]
        public byte DayType { get; set; }

        [DataMember(Name = "areacode")]
        public string AreaCode { get; set; }

        [DataMember(Name = "time")]
        public long Time { get; set; }

        [DataMember(Name = "nightbeg")]
        public int NightBegin { get; set; }

        [DataMember(Name = "nightend")]
        public int NightEnd { get; set; }

        [DataMember(Name = "usertype")]
        public int UserType { get; set; }
    }
}
