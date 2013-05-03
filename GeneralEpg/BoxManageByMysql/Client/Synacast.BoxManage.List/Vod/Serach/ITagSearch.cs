using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Vod.Serach
{
    [ServiceContract]
    public interface ITagSearch
    {
        [OperationContract]
        string TagSearch(TagSearchFilter filter);

        [OperationContract]
        string TagSearchCH(TagSearchFilter filter);

        [OperationContract]
        string TagSearchSmart(TagSearchFilter filter);
    }
}
