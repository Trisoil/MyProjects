using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Vod.Lists
{
    using Synacast.BoxManage.List.Lists;

    [ServiceContract]
    public interface IListByTags 
    {
        [OperationContract]
        string ListByTags(ListFilter filter);

        [OperationContract]
        string ListByTagsEx(ListFilter filter);
    }

}
