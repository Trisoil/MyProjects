using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("treenodes")]
    public class TreeNodeInfo : DbObject<TreeNodeInfo>
    {
        [PrimaryKey]
        public int NodeID { get; set; }

        [Property]
        public string NodeName { get; set; }

        [Property]
        public string NodeNote { get; set; }

        [Property]
        public int ParentNodeID { get; set; }

        [Property]
        public int BoxTypeID { get; set; }

        [Property]
        public int Rank { get; set; }

        [Property]
        public string NodeNameEng { get; set; }
    }
}
