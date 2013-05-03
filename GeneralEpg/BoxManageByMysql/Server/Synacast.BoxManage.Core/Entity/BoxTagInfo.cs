using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("boxtags")]
    public class BoxTagInfo : DbObject<BoxTagInfo>
    {
        [PrimaryKey]
        public int ID { get; set; }

        [Property]
        public string TagName { get; set; }

        [Property]
        public int TagType { get; set; }

        [Property]
        public int TagRank { get; set; }

        [Property]
        public int BoxTypeID { get; set; }

        [Property]
        public bool IsVisible { get; set; }

        [Property]
        public string TagNameEng { get; set; }
    }
}
