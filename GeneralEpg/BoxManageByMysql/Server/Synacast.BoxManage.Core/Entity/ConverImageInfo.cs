using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("coverimages")]
    public class ConverImageInfo : DbObject<ConverImageInfo>
    {
        [PrimaryKey]
        public int ImageID { get; set; }

        [Property]
        public string Title { get; set; }

        [Property]
        public string Note { get; set; }

        [Property]
        public int ChannelID { get; set; }

        [Property]
        public int ChannelType { get; set; }

        [Property]
        public string ImageUrl { get; set; }

        [Property]
        public DateTime OnlineTime { get; set; }

        [Property]
        public bool IsPublish { get; set; }

        [Property]
        public int Rank { get; set; }

        [Property]
        public int NavID { get; set; }
    }
}
