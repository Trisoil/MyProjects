using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;
    using Synacast.BoxManage.Core.Cache.Internal;

    [Serializable]
    [ActiveRecord("bkinfos")]
    public class BKInfo : DbObject<BKInfo>
    {
        [PrimaryKey]
        public int ID { get; set; }

        [Property]
        public string BKTypeId { get; set; }

        [Property]
        public string Episode { get; set; }

        [Property]
        public string Directors { get; set; }

        [Property]
        public string Actors { get; set; }

        [Property]
        public string CoverPic { get; set; }

        [Property]  //(Formula = "left(Introduction,300)")
        public string Introduction { get; set; }

        [Property]
        public string Score { get; set; }

        [Property]
        public string DirectorsEng { get; set; }

        [Property]
        public string ActorsEng { get; set; }

        [Property]
        public string IntroductionEng { get; set; }
    }
}
