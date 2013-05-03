using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Vod.Treeleft
{
    [ServiceContract]
    public interface ITreeleft 
    {
        [OperationContract]
        string Treelefts(TreeleftFilter filter);

        [OperationContract]
        string TreeleftsEx(TreeleftFilter filter);
    }
}
