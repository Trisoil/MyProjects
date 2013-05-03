using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("boxtypes")]
    public class BoxTypeInfo : DbObject<BoxTypeInfo>
    {
        [PrimaryKey]
        public int ID { get; set; }

        [Property]
        public string TypeName { get; set; }

        [Property]
        public int TypeID { get; set; }

        [Property]
        public string BKTypes { get; set; }

        [Property]
        public int TypeRank { get; set; }

        [Property]
        public string PicLink { get; set; }

        [Property]
        public bool IsTreeSupported { get; set; }

        [Property]
        public string TypeNameEng { get; set; }

    }
}
