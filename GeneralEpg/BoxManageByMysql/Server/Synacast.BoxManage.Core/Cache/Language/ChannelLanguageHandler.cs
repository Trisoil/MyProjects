using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Language
{
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Internal;

    public static class ChannelLanguageHandler
    {
        public static void ProcessLang(VideoNode video)
        {
            video.Language = new Dictionary<string, ChannelLanguageNode>(AppSettingCache.LanguageNum);
            video.YearTags = VideoUtils.FormatBoxTag(video.TagsRank, CustomArray.YearDimension, 15);
            var cnode = new ChannelLanguageNode();
            cnode.ChannelName = video.ChannelName;
            cnode.SubTitle = video.Main.AliasName;
            cnode.Mark = ListUtils<VideoNode>.SetMark(video);
            cnode.AreaTags = VideoUtils.FormatBoxTag(video.TagsRank, CustomArray.AreaDimension);
            cnode.CatalogTags = VideoUtils.FormatBoxTag(video.TagsRank, CustomArray.CatalogDimension);
            cnode.Actors = video.BKInfo.Actors;
            cnode.Directors = video.BKInfo.Directors;
            cnode.Introduction = video.BKInfo.Introduction;
            video.Language[CustomArray.LanguageArray[0]] = cnode;
            ListUtils<VideoNode>.SetViceMaxNum(video, CustomArray.LanguageArray[0]);
            if (AppSettingCache.Instance["LanauageTW"] == "1")
            {
                var node = new TraditionChannelNode(cnode);
                video.Language[CustomArray.LanguageArray[1]] = node;
                ListUtils<VideoNode>.SetViceMaxNum(video, CustomArray.LanguageArray[1]);

                //var node = new ChannelLanguageNode();
                //node.ChannelName = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].ChannelName);
                //node.SubTitle = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].SubTitle);
                //node.Mark = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].Mark);
                //node.AreaTags = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].AreaTags);
                //node.CatalogTags = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].CatalogTags);
                //node.Actors = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].Actors);
                //node.Directors = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].Directors);
                //node.Introduction = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].Introduction);
                //video.Language[CustomArray.LanguageArray[1]] = node;
                //ListUtils<VideoNode>.SetViceMaxNum(video, CustomArray.LanguageArray[1]);
            }
            if (AppSettingCache.Instance["LanauageEN"] == "2")
            {
                var node = new ChannelLanguageNode();
                node.ChannelName = video.Main.NameEng;
                node.SubTitle = "unknow";
                node.Mark = "unknow";
                //node.AreaTags = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].AreaTags);
                //node.CatalogTags = LanguageUtils.FormatTraditionChinese(video.Language[CustomArray.LanguageArray[0]].CatalogTags);
                node.Actors = video.BKInfo.ActorsEng;
                node.Directors = video.BKInfo.DirectorsEng;
                node.Introduction = video.BKInfo.IntroductionEng;
                video.Language[CustomArray.LanguageArray[2]] = node;
                ListUtils<VideoNode>.SetViceMaxNum(video, CustomArray.LanguageArray[2]);
            }
        }
    }
}
