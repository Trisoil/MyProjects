using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("livetypes")]
    public class LiveTypeInfo : DbObject<LiveTypeInfo>
    {
        [PrimaryKey]
        public int ID { get; set; }

        [Property]
        public string TypeName { get; set; }

        [Property]
        public int Rank { get; set; }

        //[Property]
        //public int ParentID { get; set; }

        [Property]
        public string PicLink { get; set; }

        //[Property]
        //public int CustomerID { get; set; }

        [Property]
        public string TypeNameEng { get; set; }
    }
}
