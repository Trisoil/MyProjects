using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.BoxManage.Core.Job
{
    using Quartz;
    using log4net;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache.Key;
    using Synacast.BoxManage.Core.Cache.Custom;

    public class UpdateListJob : IJob 
    {
        #region Fields

        static readonly ILog log = BoxUtils.GetLogger("quartz.job.updatelist.log");

        #endregion

        #region IJob 成员

        public void Execute(JobExecutionContext context)
        {
            try
            {
                log.Info("UpdateListJob开始更新");
                var yesterday = YesterdayListCache.Instance.Items;
                var todaylist = ListCache.Instance.Items[new VideoNodeKey(null, 0, null)].Ver1;
                var today = VideoUtils.ConvertVodIenum(todaylist);
                var root = new XElement("vlist");
                root.Add(new XElement("count", todaylist.Count()));
                XElement updatexml;
                XElement deletexml;
                if (yesterday != null)
                {
                    var updatevideo = today.Intersect(yesterday, new YesterdayModifyCompare());
                    var addvideo = today.Except(yesterday, new YesterdayVidCompare());
                    var deletevideo = yesterday.Except(today, new YesterdayVidCompare());
                    updatexml = new XElement("v-update", new XAttribute("count", updatevideo.Count() + addvideo.Count()));
                    foreach (var uv in updatevideo)
                    {
                        updatexml.Add(ResponseUtils.ResponseBack(ListCache.Instance.Dictionary[uv.ChannelId]));
                    }
                    foreach (var av in addvideo)
                    {
                        updatexml.Add(ResponseUtils.ResponseBack(ListCache.Instance.Dictionary[av.ChannelId]));
                    }
                    deletexml = new XElement("v-delete", new XAttribute("count", deletevideo.Count()));
                    foreach (var dv in deletevideo)
                    {
                        var v = new XElement("vid", dv.ChannelId);
                        deletexml.Add(v);
                    }
                }
                else
                {
                    updatexml = new XElement("v-update", new XAttribute("count", 0));
                    deletexml = new XElement("v-delete", new XAttribute("count", 0));

                }
                root.Add(updatexml);
                root.Add(deletexml);
                string fileName = string.Format("delta_{0}.xml", DateTime.Now.AddDays(-1).ToString("yyyy-MM-dd"));
                ResponseUtils.SaveXml(fileName, root);
            }
            catch (Exception ex)
            {
                log.Error(ex);
            }
            finally
            {
                log.Info("UpdateListJob结束更新");
            }
        }

        #endregion

    }
}
