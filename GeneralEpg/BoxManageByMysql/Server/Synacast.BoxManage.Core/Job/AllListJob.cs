using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.BoxManage.Core.Job
{
    using Quartz;
    using log4net;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;

    public class AllListJob : IJob 
    {
        #region Fields

        private static string _buffername = "YesterdayListCacheBuffer";
        private static readonly ILog log = BoxUtils.GetLogger("quartz.job.alllist.log");

        #endregion

        #region IJob 成员

        public void Execute(JobExecutionContext context)
        {
            try
            {
                log.Info("AllListJob开始更新");
                var videos = ListCache.Instance.Items[new VideoNodeKey(null, 0, null)].Ver1;
                var root = new XElement("vlist");
                int num = 0;
                foreach (var video in videos)
                {
                    root.Add(ResponseUtils.ResponseBack(ListCache.Instance.Dictionary[video]));
                    num++;
                }
                root.AddFirst(new XElement("count", num));
                ResponseUtils.SaveXml("all.xml", root);
                YesterdayListCache.Instance.Refresh(videos);
                SerizlizerUtils<YesterdayVideoNode>.SerializeSplitCache(YesterdayListCache.Instance.Items, _buffername);
                //SerizlizerUtils<VideoNode>.SerializeSplitCache(videos, _buffername);
            }
            catch (Exception ex)
            {
                log.Error(ex);
            }
            finally
            {
                log.Info("AllListJob结束更新");
            }
        }

        #endregion
    }
}
