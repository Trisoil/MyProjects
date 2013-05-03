using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Nav
{
    [ServiceContract]
    public interface INav
    {
        [OperationContract]
        string Nav(NavFilter filter);

        [OperationContract]
        string NavEx(NavFilter filter);

        [OperationContract]
        string NavLists(NavFilter filter);

        [OperationContract]
        string NavListsEx(NavFilter filter);
    }
}
