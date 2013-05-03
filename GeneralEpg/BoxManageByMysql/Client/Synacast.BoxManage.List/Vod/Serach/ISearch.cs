using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Vod.Serach
{
    using Synacast.BoxManage.List.Lists;

    [ServiceContract]
    public interface ISearch 
    {
        [OperationContract]
        string VideoSearch(ListFilter filter);

        [OperationContract]
        string VideoSearchEx(ListFilter filter);

        [OperationContract]
        string VideoSearchCH(ListFilter filter);

        [OperationContract]
        string VideoSearchCHEx(ListFilter filter);

        [OperationContract]
        string VideoSearchSmart(ListFilter filter);

        [OperationContract]
        string VideoSearchSmartEx(ListFilter filter);

        [OperationContract]
        string HotSearchWord(SimpleSearchFilter filter);
    }
}
