using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal.Container
{
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;

    /// <summary>
    /// 频道容器基类
    /// </summary>
    public abstract class ContainerBase
    {

        protected void FormateImage(AuthNode plat, VideoBase video)
        {
            if (!string.IsNullOrEmpty(plat.FormateImage))
            {
                video.ResImage = video.BKInfo.CoverPic.Replace("75X100", plat.FormateImage);
            }
            else
            {
                video.ResImage = video.BKInfo.CoverPic;
            }
        }

        protected void FormateImage(VideoBase video)
        {
            video.ResImage = video.BKInfo.CoverPic;
        }

        protected void FormateChannelName(AuthNode plat, VideoBase video)
        {
            if (!string.IsNullOrEmpty(plat.FormateName))
            {
                int index = video.Main.Name.LastIndexOf(plat.FormateName);
                if (index >= 0)
                    video.ChannelName = video.Main.Name.Substring(0, index);
                else
                    video.ChannelName = video.Main.Name;
            }
            else
            {
                video.ChannelName = video.Main.Name;
            }
            var v = video as VideoNode;
            if (v != null)  //是点播
            {
                if (!string.IsNullOrEmpty(plat.ReplaceName))
                {
                    string[] names = plat.ReplaceName.Split(SplitArray.DHArray, StringSplitOptions.RemoveEmptyEntries);
                    foreach (string n in names)
                    {
                        video.ChannelName = video.ChannelName.Replace(n, "");
                    }
                }
            }
        }

        protected void FormateChannelName(VideoBase video)
        {
            video.ChannelName = video.Main.Name;
        }
    }
}
