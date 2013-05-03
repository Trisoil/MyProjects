using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("livechannelrecommandrelations")]
    public class ChannelLiveRecomRelInfo : DbObject<ChannelLiveRecomRelInfo>
    {
        [PrimaryKey]
        public int ID { get; set; }

        [Property]
        public int LiveRecommandID { get; set; }

        [Property]
        public int ChannelID { get; set; }

        [Property]
        public int Rank { get; set; }
    }
}
