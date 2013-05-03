using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Feedback
{
    using Synacast.BoxManage.List.Feedback;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Utils;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class FeedbackList : IFeedback 
    {
        #region IFeedback 成员

        public string SubmitMark(FeedbackFilter filter)
        {
            try
            {
                if (!ListCache.Instance.Dictionary.ContainsKey(filter.vid))
                    return NoVideos();
                var root = new XElement("succed",
                    new XElement("mark", filter.mark)
                    );
                return root.ToString(SaveOptions.DisableFormatting);

            }
            catch (KeyNotFoundException)
            {
                return NoVideos();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        #endregion

        private static string NoVideos()
        {
            return BoxUtils.FormatErrorMsg("不存在该影片");
        }
    }
}
