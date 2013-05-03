using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("covernavs")]
    public class CoverNavInfo : DbObject<CoverNavInfo>
    {
        [PrimaryKey]
        public int ID { get; set; }

        [Property]
        public string Name { get; set; }

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
