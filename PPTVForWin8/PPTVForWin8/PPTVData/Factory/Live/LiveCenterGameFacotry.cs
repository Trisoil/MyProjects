using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Factory.Live
{
    public class LiveCenterGameFacotry : LiveCenterFactoryBase
    {
        protected override string Id
        {
            get { return "game"; }
        }

        protected override Entity.Live.LiveType Type
        {
            get { return Entity.Live.LiveType.Games; }
        }
    }
}
