using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Live.Search
{
    [ServiceContract]
    public interface ILiveSearch
    {
        [OperationContract]
        string LiveSearch(LiveSearchFilter filter);

        [OperationContract]
        string LiveSearchEx(LiveSearchFilter filter);

        [OperationContract]
        string LiveSearchCh(LiveSearchFilter filter);

        [OperationContract]
        string LiveSearchChEx(LiveSearchFilter filter);

        [OperationContract]
        string LiveParadeSearch(LiveSearchFilter filter);

        [OperationContract]
        string LiveParadeSearchEx(LiveSearchFilter filter);

        [OperationContract]
        string LiveParadeSearchCh(LiveSearchFilter filter);

        [OperationContract]
        string LiveParadeSearchChEx(LiveSearchFilter filter);

    }
}
