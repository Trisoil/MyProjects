using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Entity
{
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord;

    [Serializable]
    [ActiveRecord("authentications")]
    public class AuthenticationInfo : DbObject<AuthenticationInfo>
    {
        [PrimaryKey]
        public int ID { get; set; }

        [Property]
        public string AuthLicence { get; set; }

        [Property]
        public string AuthKey { get; set; }

        [Property]
        public int PlatformID { get; set; }

        [Property]
        public string PlatformName { get; set; }

        [Property]
        public string NotContainsNames { get; set; }

        [Property]
        public string FormateName { get; set; }

        [Property]
        public string ReplaceName { get; set; }

        [Property]
        public string FormateImage { get; set; }

        [Property]
        public string SearchTypeIndex { get; set; }

        [Property]
        public bool CurrentSupport { get; set; }

        [Property]
        public string IsVisable { get; set; }

        [Property]
        public string IsForbiddenSupport { get; set; }
    }
}
