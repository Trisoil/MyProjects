using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Synacast.BoxManage.Core.Cache
{
    #region Namespaces

    using log4net;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Cache.Internal.Container;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.LuceneNet;
    using Synacast.LuceneNetSearcher;
    using Synacast.BoxManage.Core.Cache.Language;

    #endregion

    /// <summary>
    /// 点播频道缓存
    /// </summary>
    public class ListCache : CacheBase<Dictionary<VideoNodeKey,VodContent>>
    {
        #region Fields

        private Dictionary<int, VideoNode> _dic;//= new Dictionary<int, VideoNode>();
        private static MutexLock _indexlock = new MutexLock();
        private static string _buffername = "ListCacheBuffer";
        private static readonly ILog _log = BoxUtils.GetLogger("lucenenet.queue.log");

        #endregion

        #region Constructor

        private ListCache()
        { }

        #endregion

        #region Propeties

        public readonly static ListCache Instance = new ListCache();

        public Dictionary<int, VideoNode> Dictionary
        {
            get
            {
                return _dic;
            }
        }

        #endregion

        /// <summary>
        /// 服务启动序列化刷新行为
        /// </summary>
        public override void SerializeInit()
        {
            var list = SerizlizerUtils<VideoNode>.DeserializeSplitCache(_buffername);
            if (list == null)
                return;
            LoadPlatForm(list);
            list.Clear();
        }

        /// <summary>
        /// 自刷新行为
        /// </summary>
        public override void RefreshCache()
        {
            var channels = ChannelCache.Instance.Items;
            var vicecs = ViceChnanelCache.Instance.Items;

            //过滤BKInfo为空的频道
            var bkinfoquery = from channel in channels let bkid = VideoUtils.FormatStrArray(16, BKTypeCache.Instance[channel.BKType], "/", channel.BKID) let bkinfo = BKInfoCache.Instance[bkid] where bkinfo != null select new VideoNode() { Main = channel, BKInfo = bkinfo };
            
            //加载Tag信息
            var tagquery = from channel in bkinfoquery join tagrel in ChannelTagRelCache.Instance.Items on channel.Main.ChannelID equals tagrel.ChannelID into tagrels let x = LoadTags(channel, tagrels) where channel.Type != 0 select channel;
            
            //加载合集
            var vicequery = from channel in tagquery join vice in vicecs on channel.Main.ChannelID equals vice.ParentChannelID into vices let x = LoadChildren(channel, vices) where x && FilterVideo(channel) select channel;
            
            //根据平台建立索引
            LoadPlatForm(vicequery.OrderByDescending(v => v.Main.Rank).ThenByDescending(v => v.Main.CreateTime));
        }

        public override void RefreshExtension()
        {
            SerizlizerUtils<VideoNode>.SerializeSplitCache(_dic, _buffername);
        }

        /// <summary>
        /// 外部命令调取使用
        /// </summary>
        public void RefreshListIndex()
        {
            LuceneInitIndex(_dic);
        }

        #region Private

        /// <summary>
        /// 分配频道的平台
        /// </summary>
        private void LoadPlatForm(IEnumerable<VideoNode> list)
        {
            var count = list.Count();
            var container = new VodContainer();
            var dic = new Dictionary<int, VideoNode>(count);
            foreach (var video in list)
            {
                dic[video.Main.ChannelID] = video;
                container.Process(video);
                ChannelLanguageHandler.ProcessLang(video);
            }
            container.Complete(dic);
            LuceneInitIndex(dic);
            _cache = container.Dictionary;
            _dic = dic;
            GC.Collect();
        }

        /// <summary>
        /// LuceneNet建立索引
        /// </summary>
        private void LuceneInitIndex(Dictionary<int,VideoNode> videos)
        {
            if (bool.Parse(AppSettingCache.Instance["VodSearchAct"]))
            {
                //ThreadPool.QueueUserWorkItem(state =>
                //{
                    if (!_indexlock.TryEnter())
                        return;
                    try
                    {
                        _log.Info("ListCache开始建立LuceneNet索引");
                        var dic = EpgLuceneNet.Instance.Init();
                        foreach (var video in videos.Values)
                        {
                            EpgIndex.AddVideo(video, dic);
                        }
                        EpgLuceneNet.Instance.CompleteIndex(dic);
                        _log.Info("ListCache索引建立完成");
                    }
                    catch (Exception ex)
                    {
                        _log.Error(ex);
                    }
                    finally
                    {
                        _indexlock.Exit();
                    }
                //});
            }
        }

        /// <summary>
        /// 加载频道Tag信息
        /// </summary>
        private object LoadTags(VideoNode video, IEnumerable<ChannelTagRelInfo> tagRels)
        {
            if (tagRels.Count() == 0)
                return null;
            var acts = video.BKInfo.Actors.FormatStrToArray(SplitArray.DHArray).Select(v => VideoUtils.FormatStrArray(v.Length + 8, v, ":", CustomArray.ActDimension));
            var directors = video.BKInfo.Directors.FormatStrToArray(SplitArray.DHArray).Select(v => VideoUtils.FormatStrArray(v.Length + 10, v, ":", CustomArray.DirectorDimension));
            var tags = new Dictionary<string, int>(tagRels.Count() + acts.Count() + directors.Count());
            foreach (var act in acts)
            {
                tags[act] = 0;
            }
            foreach (var director in directors)
            {
                tags[director] = 0;
            }
            foreach (var tagrel in tagRels)
            {
                var tag = BoxTagCache.Instance[tagrel.TagID];
                if (tag == null)
                    continue;
                video.Type = tag.BoxTypeID;
                var tagtype = TagTypeCache.Instance[tag.TagType];
                if (tagtype != null)
                {
                    string dimension = VideoUtils.FormatStrArray(tag.TagName.Length + tagtype.TagTypeLimitName.Length + 2, tag.TagName, ":", tagtype.TagTypeLimitName);
                    tags[dimension] = tagrel.Rank;
                }
            }
            video.TagsRank = tags;
            return null;
        }

        /// <summary>
        /// 加载合集
        /// </summary>
        private bool LoadChildren(VideoNode video, IEnumerable<ViceChannelInfo> children)
        {
            bool result = true;
            IEnumerable<ViceChannelInfo> cs = null;
            if (IsTV(video))
                cs = children.OrderBy(channel => channel.Rank);  //电视合集时，从第一集开始
            else
                cs = children.OrderByDescending(channel => channel.Rank);  //综艺、专题合集时，将最近更新的放在最前
            var isnumber = true;
            if (video.Main.TableType == 0)
            {
                video.Groups = new List<int>(1);
                video.Groups.Add(video.Main.ChannelID);
                video.ViceChannels = cs.Select(v =>
                {
                    isnumber = isnumber && SpellUtils.IsNumber(v.Name);
                    return new ViceNode()
                    {
                        Language = SimpleLanguageHandler.ProcessLang(v.Name, v.NameEng),
                        ChannelId = v.ChannelID,
                        Duration = v.TimeLength,
                        PlayLink = PlayLinkUtils.MakePlayLink(v.FileName)
                    };
                }).ToPerfectList();
                if (video.ViceChannels.Count > 0)
                {
                    video.PlayLink = video.ViceChannels[0].PlayLink;
                    int count = video.ViceChannels.Count;
                    video.State = video.StateCount = count;
                    if (IsTV(video))
                    {
                        int state = 0;
                        if (int.TryParse(video.ViceChannels[count - 1].Language[CustomArray.LanguageArray[0]].Title, out state))
                            video.State = state;
                    }
                }
                else
                {
                    isnumber = SpellUtils.IsNumber(video.ChannelName);
                    video.State = video.StateCount = 1;
                    video.PlayLink = PlayLinkUtils.MakePlayLink(video.Main.FileName);
                }
            }
            else if (video.Main.TableType == 2)
            {
                video.Groups = cs.Select(v => v.ChannelID).ToPerfectList();
                if (video.Groups.Count <= 0)
                    return false;
            }
            else
            {
                video.Virtuals = cs.Select(v => v.ChannelID).ToPerfectList();
                if (video.Virtuals.Count <= 0)
                    return false;
                video.Groups = new List<int>(1);
                video.Groups.Add(video.Virtuals[0]);
            }
            video.IsNumber = Convert.ToInt32(isnumber);
            return result;
        }

        /// <summary>
        /// 过滤不符合的频道
        /// </summary>
        private bool FilterVideo(VideoNode video)
        {
            var result = (video.ViceChannels == null ? 2 : video.ViceChannels.Count) >= AppSettingCache.VodFilterState[video.Type - 1] && video.Main.TimeLength / 60 >= AppSettingCache.VodFilterDuration[video.Type - 1];
            if (result)
            {
                ListUtils<VideoNode>.SetFlags(video, v => v.Type == 1);
                ListUtils<VideoNode>.SetPV(video);
                ListUtils<VideoNode>.SetSlotImage(video);
                ListUtils<VideoNode>.SetForbidden(video);
                video.SpellName = SpellUtils.GetChineseFrist(video.Main.Name);
            }
            return result;
        }

        /// <summary>
        /// 频道是否是电视合集
        /// </summary>
        private bool IsTV(VideoNode video)
        {
            return video.Main.SetType == 1 || video.Main.SetType == 99;
        }

        #endregion
    }
}
