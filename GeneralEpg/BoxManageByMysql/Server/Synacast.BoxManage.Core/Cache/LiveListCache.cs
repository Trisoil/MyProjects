using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Synacast.BoxManage.Core.Cache
{
    #region Namespaces

    using log4net;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Cache.Internal.Container;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.LuceneNet;
    using Synacast.BoxManage.Core.LuceneNet.Live;
    using Synacast.LuceneNetSearcher;
    using Synacast.BoxManage.Core.Cache.Language;

    #endregion

    /// <summary>
    /// 直播频道缓存
    /// </summary>
    public class LiveListCache : CacheBase<Dictionary<VideoNodeKey,List<LiveVideoNode>>>
    {
        #region Fields

        private static readonly string _bufferName = "LiveListCache";
        private Dictionary<int, LiveVideoNode> _dic;//= new Dictionary<int, LiveVideoNode>();
        private static MutexLock _indexlock = new MutexLock();
        private static readonly ILog _log = BoxUtils.GetLogger("lucenenet.queue.log");

        #endregion

        #region Properties

        public static readonly LiveListCache Instance = new LiveListCache();

        private LiveListCache()
        { }

        public Dictionary<int, LiveVideoNode> Dictionary
        {
            get {
                return _dic;
            }
        }


        #endregion

        #region Override

        /// <summary>
        /// 服务启动序列化刷新行为
        /// </summary>
        public override void SerializeInit()
        {
            base.SerializeInit();
            if (_cache == null) return;
            var list = new List<LiveVideoNode>();
            foreach (var l in _cache)
            {
                if (l.Key.Type == 0)
                    list.AddRange(l.Value);
            }
            var dic = list.ToPerfectDictionary(v => v.Main.ChannelID);
            list.Clear();
            LuceneInitIndex(dic);
            _dic = dic;
        }

        /// <summary>
        /// 自刷新行为
        /// </summary>
        public override void RefreshCache()
        {
            //过滤BKInfo为空的频道
            var bkinfoquery = from channel in LiveChannelCache.Instance.Items where channel.ParentChannelID == 0 let bkid = VideoUtils.FormatStrArray(16, BKTypeCache.Instance[channel.BKType], "/", channel.BKID) let bkinfo = BKInfoCache.Instance[bkid] where bkinfo != null select new LiveVideoNode() { Main = channel, BKInfo = bkinfo };

            //加载频道分类
            var typequery = from channel in bkinfoquery join liverel in ChannelLiveRelCache.Instance.Items on channel.Main.ChannelID equals liverel.ChannelID into liverels let x = LoadTypes(channel, liverels) select channel;

            //加载主分类下的Customer的Rank
            var rankquery = from channel in typequery join customerrel in ChannelLiveCustomerRelCache.Instance.Items on channel.Main.ChannelID equals customerrel.ChannelID into cutomerrels let x = LoadCustomer(channel, cutomerrels) where x select channel;

            //加载节目预告
            var itemquery = from channel in rankquery join item in ChannelLiveItemsCache.Instance.Items.Values on channel.Main.ChannelID equals item.ChannelID into items let x = LoadItems(channel, items) select channel;

            //加载分组
            var vicequery = from channel in itemquery join vice in LiveChannelCache.Instance.Items on channel.Main.ChannelID equals vice.ParentChannelID into vices let x = LoadChildren(channel, vices) where x select channel;

            //根据平台建立索引
            var container = new LiveVideoContanier();
            var dic = new Dictionary<int, LiveVideoNode>(vicequery.Count());
            foreach (var video in vicequery)
            {
                dic.Add(video.Main.ChannelID, video);
                container.Process(video);
                LiveChannelHandler.ProcessLang(video);
            }
            LuceneInitIndex(dic);
            _cache = container.Dictionary;
            _dic = dic;

            //过滤掉分组里不存在的频道
            var errors = new List<int>(4);
            foreach (var channel in _dic.Values)
            {
                if (channel.Main.TableType != 2)
                    continue;
                errors.Clear();
                foreach (var gr in channel.Groups)
                {
                    LiveVideoNode v;
                    if (_dic.TryGetValue(gr, out v) && v.HiddenFlag == 0)
                    {
                        v.IsGroup = true;
                    }
                    else
                        errors.Add(gr);
                }
                foreach (var e in errors)
                {
                    channel.Groups.Remove(e);
                }
                if (channel.Groups.Count <= 0)
                {
                    channel.HiddenFlag = 1;
                }
            }
        }

        private bool LoadChildren(LiveVideoNode channel, IEnumerable<ChannelInfo> vices)
        {
            var result = true;
            var cs = vices.OrderByDescending(v => v.Rank);
            if (channel.Main.TableType == 2)
            {
                channel.Groups = cs.Select(v => v.ChannelID).ToPerfectList();
                if (channel.Groups.Count <= 0)
                    return false;
            }
            else
            {
                channel.Groups = new List<int>(1);
                channel.Groups.Add(channel.Main.ChannelID);
            }
            return result;
        }

        protected override string BufferName
        {
            get
            {
                return _bufferName;
            }
        }

        /// <summary>
        /// 外部命令调取使用
        /// </summary>
        public void RefreshListIndex()
        {
            LuceneInitIndex(_dic);
        }

        #endregion

        #region Private

        /// <summary>
        /// LuceneNet建立索引
        /// </summary>
        private void LuceneInitIndex(Dictionary<int, LiveVideoNode> videos)
        {
            if (bool.Parse(AppSettingCache.Instance["LiveSearchAct"]))
            {
                //ThreadPool.QueueUserWorkItem(state =>
                //{
                    if (!_indexlock.TryEnter())
                        return;
                    try
                    {
                        _log.Info("LiveListCache开始建立LuceneNet索引");
                        var dic = LiveEpgLuceneNet.Instance.Init();
                        foreach (var video in videos.Values)
                        {
                            LiveEpgIndex.AddVideo(video, dic);
                        }
                        LiveEpgLuceneNet.Instance.CompleteIndex(dic);
                        _log.Info("LiveListCache索引建立完成");
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
        /// 加载类型
        /// </summary>
        private object LoadTypes(LiveVideoNode video, IEnumerable<ChannelLiveRelInfo> liverels)
        {
            List<int> types = new List<int>();
            Dictionary<string, int> rankdic = new Dictionary<string, int>();
            foreach (var liverel in liverels)
            {
                types.Add(liverel.LiveTypeID);
                rankdic[liverel.LiveTypeID.ToString()] = liverel.Rank;
            }
            video.Types = types;
            video.TagsRank = rankdic;
            return null;
        }

        private bool LoadCustomer(LiveVideoNode video, IEnumerable<ChannelLiveCustomerRelInfo> customerrels)
        {
            bool flag = false;
            foreach (var customerrel in customerrels)
            {
                flag = true;
                video.Rank = customerrel.Rank;
            }
            return flag;
        }

        /// <summary>
        /// 加载节目预告
        /// </summary>
        private object LoadItems(LiveVideoNode video, IEnumerable<ChannelLiveItemsNode> items)
        {
            video.Items = items.OrderBy(i => i.StartTime).ToPerfectList();
            ListUtils<LiveVideoNode>.SetFlags(video, v => true);
            ListUtils<LiveVideoNode>.SetPV(video);
            ListUtils<LiveVideoNode>.SetLiveSlotImage(video);
            ListUtils<VideoNode>.SetForbidden(video);
            video.SpellName = SpellUtils.GetChineseFrist(video.Main.Name);
            return null;
        }

        #endregion
    }
}
