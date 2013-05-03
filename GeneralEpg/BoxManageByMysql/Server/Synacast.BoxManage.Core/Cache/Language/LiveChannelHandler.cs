using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Language
{
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Utils;

    public static class LiveChannelHandler
    {
        public static void ProcessLang(LiveVideoNode video)
        {
            video.Language = new Dictionary<string, LiveChannelLanguageNode>(AppSettingCache.LanguageNum);
            var cnode = new LiveChannelLanguageNode();
            cnode.ChannelName = video.ChannelName;
            cnode.Mark = ListUtils<LiveVideoNode>.SetMark(video);
            cnode.Introduction = video.BKInfo.Introduction;
            video.Language[CustomArray.LanguageArray[0]] = cnode;
            if (AppSettingCache.Instance["LanauageTW"] == "1")
            {
                var node = new TraditionLiveChannelNode(cnode);
                //var node = new LiveChannelLanguageNode();
                //node.ChannelName = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].ChannelName);
                //node.Mark = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].Mark);
                //node.Introduction = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].Introduction);
                video.Language[CustomArray.LanguageArray[1]] = node;
            }
            if (AppSettingCache.Instance["LanauageEN"] == "2")
            {
                var node = new LiveChannelLanguageNode();
                node.ChannelName = video.Main.NameEng;
                node.Mark = "unknow";
                node.Introduction = video.BKInfo.IntroductionEng;
                video.Language[CustomArray.LanguageArray[2]] = node;
            }
        }
    }
}
