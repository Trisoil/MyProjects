using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Factory.Live
{
    public class LiveCenterSportsFactory : LiveCenterFactoryBase
    {

        protected override string Id
        {
            get { return "sports"; }
        }

        protected override Entity.Live.LiveType Type
        {
            get { return Entity.Live.LiveType.Sports; }
        }
    }
}
