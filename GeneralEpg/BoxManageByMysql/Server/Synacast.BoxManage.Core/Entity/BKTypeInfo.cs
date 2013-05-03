using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;
    
    [Serializable]
    [ActiveRecord("bktypes")]
    public class BKTypeInfo : DbObject<BKTypeInfo>
    {
        [PrimaryKey]
        public int BkTypePkID { get; set; }

        [Property]
        public int BkTypeId { get; set; }

        [Property]
        public string Name { get; set; }

        [Property]
        public string Title { get; set; }

        [Property]
        public int OrderWeight { get; set; }
    }
}
