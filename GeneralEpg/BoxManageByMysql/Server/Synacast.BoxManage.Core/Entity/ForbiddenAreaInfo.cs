using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("forbiddenareas")]
    public class ForbiddenAreaInfo : DbObject<ForbiddenAreaInfo>
    {
        [PrimaryKey]
        public int ID { get; set; }

        [Property]
        public string ForbiddenAreaName { get; set; }

        [Property]
        public string ForbiddenAreaCityCode { get; set; }
    }
}
