using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Factory.Cloud
{
    using PPTVData.Entity.Cloud;

    public class LocalRecentFactory : LocalBaseFactory<LocalRecentFactory>
    {
        public override string FileName
        {
            get { return "filerecent.db"; }
        }

        public override string CloudTypeName
        {
            get { return CloudType.Recent; }
        }
    }
}
