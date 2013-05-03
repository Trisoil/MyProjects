using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("recommands")]
    public class RecommandInfo : DbObject<RecommandInfo>
    {
        [PrimaryKey]
        public int ID { get; set; }

        [Property]
        public string Name { get; set; }

        /// <summary>
        /// -1代表蓝光
        /// </summary>
        [Property]
        public int Mode { get; set; }

        [Property]
        public string PicLink { get; set; }

        [Property]
        public int Rank { get; set; }

        [Property]
        public string NameEng { get; set; }
    }
}
