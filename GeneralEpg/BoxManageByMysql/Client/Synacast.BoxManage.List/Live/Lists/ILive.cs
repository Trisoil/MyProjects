using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Live
{
    [ServiceContract]
    public interface ILive
    {
        [OperationContract]
        string LiveTypes(LiveFilter filter);

        [OperationContract]
        string LiveTypesEx(LiveFilter filter);

        [OperationContract]
        string LiveLists(LiveFilter filter);

        [OperationContract]
        string LiveListsEx(LiveFilter filter);

        [OperationContract]
        string LiveParade(LiveFilter filter);
    }
}
