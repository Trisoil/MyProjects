using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Update
{
    using Synacast.BoxManage.Core;
    using Synacast.BoxManage.List.Update;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Job;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Key;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;

    [ServiceBehavior( ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode=InstanceContextMode.PerCall)]
    public class Update : IUpdate
    {
        #region IUpdate 成员

        public string AllList(UpdateFilter filter)
        {
            try
            {
                var videos = ListCache.Instance.Items[new VideoNodeKey(null, 0, null)].Ver1;
                var xml = new XElement("vlist");
                var count = videos.BackPageList(filter.c,filter.s,xml);
                xml.AddFirst(new XElement("count", count),
                    new XElement("page_count", PageUtils.PageCount(count, filter.c))
                    );
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string TodayUpdate(UpdateFilter filter)
        {
            try
            {
                var nowvideos = VideoUtils.ConvertVodIenum(ListCache.Instance.Items[new VideoNodeKey(null, 0, null)].Ver1);
                var updatevideos = nowvideos.Where(video => video.ModifyTime >= filter.lasttime);//.ToList();
                var deletevideos = YesterdayListCache.Instance.Items.Except(nowvideos, new YesterdayVidCompare());//.ToList();
                int count = updatevideos.Count();
                string lasttime = filter.lasttime.ToString("HH:mm:ss");
                if (count > 0)
                    lasttime = updatevideos.Max(video => video.ModifyTime).ToString("HH:mm:ss");
                //var currentvideos = updatevideos.Skip(filter.c * (filter.s - 1)).Take(filter.c);
                var xml = new XElement("vlist",
                    //new XElement("count", count),
                    //new XElement("page_count", PageUtils.PageCount(count, filter.c)),
                    new XElement("latest_time", lasttime),
                    new XElement("v-update", new XAttribute("count", count),
                        from video in updatevideos select ResponseUtils.ResponseBack(ListCache.Instance.Dictionary[video.ChannelId])
                        ),
                    new XElement("v-delete", new XAttribute("count", deletevideos.Count()),
                        from dv in deletevideos select new XElement("vid", dv.ChannelId)
                        )
                    );
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        #endregion

    }

}
