using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Cover
{
    [ServiceContract]
    public interface ICover
    {
        [OperationContract]
        string ImageNav(CoverFilter filter);

        [OperationContract]
        string ImageNavEx(CoverFilter filter);

        [OperationContract]
        string ImageRecommand(CoverFilter filter);

        [OperationContract]
        string ImageRecommandEx(CoverFilter filter);

        [OperationContract]
        string SplashScreeen(CoverFilter filter);
    }
}
