using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("channels")]
    public class ViceChannelInfo : DbObject<ViceChannelInfo>
    {
        [PrimaryKey]
        public int ChannelPkID { get; set; }

        [Property]
        public int ParentChannelID { get; set; }

        [Property]
        public int ChannelID { get; set; }

        [Property]
        public string Name { get; set; }

        [Property]
        public string FileName { get; set; }

        [Property]
        public int Rank { get; set; }

        [Property]
        public string NameEng { get; set; }

        [Property]
        public bool IsOnline { get; set; }

        [Property]
        public bool IsPublish { get; set; }

        [Property]
        public int ChannelType { get; set; }

        [Property]
        public int TimeLength { get; set; }
    }
}
