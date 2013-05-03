using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.Internal
{
    [ServiceContract]
    public interface IList
    {
        [OperationContract]
        List<int> FindChannelIds(int boxTypeId);

        [OperationContract(Name="FindChannelIdsByName")]
        List<int> FindChannelIds(int boxTypeId, BoxTagTypeEnum dimension, string boxTagName);

        [OperationContract]
        void Refresh();

        [OperationContract]
        void Command(string command);
    }
}
