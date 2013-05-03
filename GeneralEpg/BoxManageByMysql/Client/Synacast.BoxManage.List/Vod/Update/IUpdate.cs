using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;
using System.Xml.Linq;

namespace Synacast.BoxManage.List.Update
{
    [ServiceContract]
    public interface IUpdate
    {
        [OperationContract]
        string AllList(UpdateFilter filter);

        [OperationContract]
        string TodayUpdate(UpdateFilter filter);
    }
}
