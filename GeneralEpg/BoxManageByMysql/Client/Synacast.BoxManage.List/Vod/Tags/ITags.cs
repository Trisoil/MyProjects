using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Tags
{
    [ServiceContract]
    public interface ITags
    {
        [OperationContract]
        string VideoTags(TagsFilter filter);

        [OperationContract]
        string TagsEx(TagsFilter filter);

        [OperationContract]
        string MultiTags(TagsFilter filter);

        [OperationContract]
        string MultiTagsEx(TagsFilter filter);

        [OperationContract]
        string VideoTypes(TagsFilter filter);

        [OperationContract]
        string VideoTypesEx(TagsFilter filter);
    }
}
