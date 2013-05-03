using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Vod.Relevance
{
    [ServiceContract]
    public interface IRelevance
    {
        [OperationContract]
        string Relevance(RelevanceFilter filter);

        [OperationContract]
        string RelevanceEx(RelevanceFilter filter);
    }
}
