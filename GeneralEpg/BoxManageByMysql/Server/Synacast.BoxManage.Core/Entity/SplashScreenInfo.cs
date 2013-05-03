using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("splashscreens")]
    public class SplashScreenInfo : DbObject<SplashScreenInfo>
    {
        [PrimaryKey]
        public int ID { get; set; }

        [Property]
        public int Mode { get; set; }

        [Property]
        public string Photo { get; set; }

        [Property]
        public DateTime Expires { get; set; }

        [Property]
        public string ETag { get; set; }
    }
}
