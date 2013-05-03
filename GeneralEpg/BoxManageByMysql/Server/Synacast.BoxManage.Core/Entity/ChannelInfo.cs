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
    public class ChannelInfo : DbObject<ChannelInfo>
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
        public int Hot { get; set; }

        [Property]
        public int ChannelType { get; set; }

        [Property]
        public int RecommendType { get; set; }

        [Property]
        public string ForbiddenArea { get; set; }

        [Property]
        public int TimeLength { get; set; }

        [Property]
        public int BKType { get; set; }

        [Property]
        public int BKID { get; set; }

        [Property]
        public int SetType { get; set; }

        /// <summary>
        /// 1代表VIP节目
        /// </summary>
        [Property]
        public int VipFlag { get; set; }

        [Property]
        public DateTime CreateTime { get; set; }

        [Property]
        public DateTime ModifyTime { get; set; }

        [Property]
        public string Md5 { get; set; }

        [Property]
        public int Rank { get; set; }

        [Property]
        public bool IsOnline { get; set; }

        [Property]
        public bool IsVisible { get; set; }

        [Property]
        public int BitRate { get; set; }

        [Property]
        public int VideoWidth { get; set; }

        [Property]
        public int VideoHeight { get; set; }

        [Property]
        public bool IsPublish { get; set; }

        [Property]
        public int CorrectHot { get; set; }

        [Property]
        public string PlayLink { get; set; }

        [Property]
        public string PlatformIDs { get; set; }

        [Property]
        public string NameEng { get; set; }

        [Property]
        public string AliasName { get; set; }

        /// <summary>
        /// 表类型(0:原表,1:虚拟频道,2:组)
        /// </summary>
        [Property]
        public int TableType { get; set; }
    }
}
