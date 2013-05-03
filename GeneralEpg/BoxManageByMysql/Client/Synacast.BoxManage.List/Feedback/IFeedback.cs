using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Feedback
{
    [ServiceContract]
    public interface IFeedback
    {
        [OperationContract]
        string SubmitMark(FeedbackFilter filter);
    }
}
