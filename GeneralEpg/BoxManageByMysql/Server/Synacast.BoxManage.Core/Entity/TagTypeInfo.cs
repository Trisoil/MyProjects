using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("tagtypes")]
    public class TagTypeInfo : DbObject<TagTypeInfo>
    {
        [PrimaryKey]
        public int TagTypeID { get; set; }

        [Property]
        public int BoxTypeID { get; set; }

        [Property]
        public int Rank { get; set; }

        [Property]
        public string TagTypeLimitName { get; set; }

        [Property]
        public bool IsRecommand { get; set; }
    }
}
