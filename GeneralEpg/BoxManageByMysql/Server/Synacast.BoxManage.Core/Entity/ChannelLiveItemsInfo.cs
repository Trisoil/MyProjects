using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [ActiveRecord("channelliveitems")]
    [Serializable]
    public class ChannelLiveItemsInfo : DbObject<ChannelLiveItemsInfo>
    {
        [PrimaryKey]
        public int ID { get; set; }

        [Property]
        public int ChannelID { get; set; }

        [Property]
        public string Title { get; set; }

        [Property]
        public DateTime StartTime { get; set; }

        [Property]
        public DateTime EndTime { get; set; }
    }
}
