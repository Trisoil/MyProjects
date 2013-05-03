using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Live.Nav
{
    [ServiceContract]
    public interface ILiveNav
    {
        [OperationContract]
        string LiveRecommandNav(LiveNavFilter filter);

        [OperationContract]
        string LiveRecommandNavEx(LiveNavFilter filter);

        [OperationContract]
        string LiveRecommandList(LiveNavFilter filter);

        [OperationContract]
        string LiveRecommandListEx(LiveNavFilter filter);
    }
}
