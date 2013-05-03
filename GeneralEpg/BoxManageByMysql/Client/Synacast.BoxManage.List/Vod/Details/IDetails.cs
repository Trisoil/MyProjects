using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Details
{
    [ServiceContract]
    public interface IDetails
    {
        [OperationContract]
        string DetailsList(DetailFilter filter);

        [OperationContract]
        string DetailsListEx(DetailFilter filter);

        [OperationContract]
        string DetailsWithoutSet(DetailFilter filter);

        [OperationContract]
        string DetailsWithoutSetEx(DetailFilter filter);

        [OperationContract]
        string DetailsOnlySet(DetailFilter filter);

        [OperationContract]
        string DetailsOnlySetEx(DetailFilter filter);
    }
}
