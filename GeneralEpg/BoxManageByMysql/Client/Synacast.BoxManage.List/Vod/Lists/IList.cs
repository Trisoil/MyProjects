using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Lists
{
    [ServiceContract]
    public interface IList
    {
        [OperationContract]
        string VideoList(ListFilter filter);

        [OperationContract]
        string VideoListEx(ListFilter filter);

    }
}
