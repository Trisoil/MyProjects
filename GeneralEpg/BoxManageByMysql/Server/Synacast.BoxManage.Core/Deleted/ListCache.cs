//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Synacast.BoxManage.Core.Deleted
//{
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Utils;
//    using Synacast.BoxManage.Core.Cache.Internal;
//    using Synacast.BoxManage.Core.Cache.Internal.Key;
//    using Synacast.BoxManage.Core.Cache.Entity;
//    using Synacast.BoxManage.Core.Cache.Custom;
//    using Synacast.BoxManage.Core.LuceneNet;
//    using Synacast.LuceneNetSearcher;

//    public class ListCache : CacheBase<Dictionary<VideoNodeKey, List<VideoNode>>>
//    {
//        #region Fields

//        private string[] _spaceSplit = new string[] { " " };
//        private string[] _dhSplit = new string[] { "," };
//        private List<VideoNode> _list = new List<VideoNode>();
//        private Dictionary<int, VideoNode> _dic = new Dictionary<int, VideoNode>();

//        #endregion

//        #region Constructor

//        private ListCache()
//        { }

//        #endregion

//        #region Propeties

//        public readonly static ListCache Instance = new ListCache();

//        public List<VideoNode> List
//        {
//            get {
//                return _list;
//            }
//        }

//        public Dictionary<int, VideoNode> Dictionary
//        {
//            get {
//                return _dic;
//            }
//        }

//        #endregion

//        /// <summary>
//        /// 服务启动序列化刷新行为
//        /// </summary>
//        public override void SerializeInit()
//        {
//            base.SerializeInit();
//            LuceneEngine engine = new LuceneEngine();
//            engine.Init();
//            EpgLuceneNet.Instance.Init();
//            FormateVideo();
//        }

//        /// <summary>
//        /// 自刷新行为
//        /// </summary>
//        public override void RefreshCache()
//        {
//            EpgLuceneNet.Instance.Init();
//            var channels = ChannelCache.Instance.Items;
//            var roots = from channel in channels where channel.ParentChannelID == 0 let bkid = string.Format("{0}/{1}", BKTypeCache.Instance[channel.BKType], channel.BKID) select new VideoNode() { Main = channel, BKInfo = BKInfoCache.Instance[bkid], Type = BoxTypeCache.Instance[channel.BKType], SpellName = SpellUtils.GetChineseSpell(channel.Name) };
//            var query = from root in roots where root.BKInfo != null && root.Type != 0 join channel in channels on root.Main.ChannelID equals channel.ParentChannelID into vices let x = LoadChildren(root, vices) where FilterVideo(root) select root;
//            var videos = query.Distinct().ToList();
//            VideoNodeContainer container = new VideoNodeContainer();
//            List<VideoNode> list = new List<VideoNode>(videos.Count);
//            Dictionary<int, VideoNode> dic = new Dictionary<int, VideoNode>(videos.Count);
//            foreach (VideoNode video in videos)
//            {
//                list.Add(video);
//                dic.Add(video.Main.ChannelID, video);
//                container.Fun(video);
//            }
//            foreach (var l in container.Dictionary)
//            {
//                if (l.Key.Type == 0)
//                {
//                    EpgLuceneNet.Instance[l.Key.PlatForm, l.Key.Auth].AddVideo(l.Value);
//                    EpgLuceneNet.Instance[l.Key.PlatForm, l.Key.Auth].CompleteIndex();
//                }
//            }
//            _cache = container.Dictionary;
//            _list = list;
//            _dic = dic;
//        }

//        /// <summary>
//        /// 分布式刷新下扩展行为
//        /// </summary>
//        public override void RefreshExtension()
//        {
//            EpgLuceneNet.Instance.Init();
//            FormateVideo();
//        }

//        /// <summary>
//        /// 加载分词库，重新建立索引
//        /// </summary>
//        public void RefreshListIndex()
//        {
//            LuceneEngine engine = new LuceneEngine();
//            engine.Init();
//            EpgLuceneNet.Instance.Init();
//            foreach (var l in _cache)
//            {
//                if (l.Key.Type == 0)
//                {
//                    EpgLuceneNet.Instance[l.Key.PlatForm, l.Key.Auth].AddVideo(l.Value);
//                    EpgLuceneNet.Instance[l.Key.PlatForm, l.Key.Auth].CompleteIndex();
//                }
//            }
//        }

//        #region Internal

//        /// <summary>
//        /// 格式化频道集合为相应类型
//        /// </summary>
//        private void FormateVideo()
//        {
//            List<VideoNode> list = new List<VideoNode>();
//            if (_cache == null) return;
//            foreach (var l in _cache)
//            {
//                list.AddRange(l.Value);
//                if (l.Key.Type == 0)
//                {
//                    EpgLuceneNet.Instance[l.Key.PlatForm, l.Key.Auth].AddVideo(l.Value);
//                    EpgLuceneNet.Instance[l.Key.PlatForm, l.Key.Auth].CompleteIndex();
//                }
//            }
//            _list = list.Distinct().ToList();
//            _dic = _list.ToDictionary(v => v.Main.ChannelID);
//        }

//        /// <summary>
//        /// 过滤不符合的频道
//        /// </summary>
//        /// <param name="video"></param>
//        /// <returns></returns>
//        private bool FilterVideo(VideoNode video)
//        {
//            if (video.Type == 1)
//                return video.Duration > 10;
//            else if (video.Type == 2 || video.Type == 3 || video.Type == 4 || video.Type == 6)
//                return video.State != 1;
//            return true;
//        }

//        /// <summary>
//        /// 加载合集
//        /// </summary>
//        /// <param name="video"></param>
//        /// <param name="children"></param>
//        /// <returns></returns>
//        private object LoadChildren(VideoNode video, IEnumerable<ChannelInfo> children)
//        {
//            List<ChannelInfo> cs = null;
//            if (IsTV(video))
//                cs = children.OrderBy(channel => channel.Rank).ToList();  //电视合集时，从第一集开始
//            else
//                cs = children.OrderByDescending(channel => channel.Rank).ToList();  //综艺、专题合集时，将最近更新的放在最前
//            video.ViceChannels = cs.ConvertAll<ViceNode>(channel =>
//            {
//                ViceNode vice = new ViceNode();
//                string title = channel.Name;
//                vice.Name = title;
//                if (IsTV(video))
//                    title = string.Format("第{0}集", title);
//                vice.Title = title;
//                vice.PlayLink = PlayLinkUtils.MakePlayLink(channel.FileName);
//                vice.TimeLength = channel.TimeLength;
//                return vice;
//            });
//            if (video.ViceChannels.Count > 0)
//            {
//                video.PlayLink = video.ViceChannels[0].PlayLink;
//                int count = video.ViceChannels.Count;
//                video.State = count;
//                video.StateCount = count;
//                if (IsTV(video))
//                {
//                    int state = 0;
//                    if (int.TryParse(video.ViceChannels[count - 1].Name, out state))
//                        video.State = state;
//                }
//                video.Duration = video.ViceChannels.Sum(vice => vice.TimeLength) / 60;
//            }
//            else
//            {
//                video.State = 1;
//                video.StateCount = 1;
//                video.Duration = video.Main.TimeLength / 60;
//                video.PlayLink = PlayLinkUtils.MakePlayLink(video.Main.FileName);
//            }
//            SetFlags(video);
//            SetTags(video);
//            return null;
//        }

//        /// <summary>
//        /// 频道是否是电视合集
//        /// </summary>
//        /// <param name="video"></param>
//        /// <returns></returns>
//        private bool IsTV(VideoNode video)
//        {
//            return video.Main.SetType == 1;
//        }

//        /// <summary>
//        /// 设置频道标记
//        /// </summary>
//        /// <param name="video"></param>
//        private void SetFlags(VideoNode video)
//        {
//            List<string> flags = new List<string>();
//            if (video.Main.BitRate > 650)
//                flags.Add("h");  //高清
//            if (video.Main.BitRate > 950)
//                flags.Add("b");  //蓝光
//            if (video.Main.RecommendType == 2)
//                flags.Add("r");  //推荐
//            if (video.Main.RecommendType == 1)
//                flags.Add("n");  //最新
//            if (video.Main.Name.ToLower().Contains("3d版") && video.Type == 1)
//                flags.Add("d");  //3D
//            flags.Sort();
//            video.Flags = flags;
//            video.ResFlags = string.Join("|", flags.ToArray());
//            video.ResResolution = string.Format("{0}|{1}", video.Main.VideoWidth, video.Main.VideoHeight);
//            //video.ResImage = video.BKInfo.CoverPic;
//            //video.ChannelName = video.Main.Name;
//        }

//        /// <summary>
//        /// 设置频道Tag
//        /// </summary>
//        /// <param name="video"></param>
//        private void SetTags(VideoNode video)
//        { 
//            //分类
//            string[] tagNames = video.BKInfo.BKType.Split(_spaceSplit, StringSplitOptions.RemoveEmptyEntries);
//            List<string> catalogs = new List<string>();
//            foreach (string tagName in tagNames)
//            {
//                BoxTagCacheKey key = new BoxTagCacheKey(tagName, TagTypeEnum.Catalog, video.Main.BKType);
//                BoxTagInfo tag = BoxTagCache.Instance[key];
//                if (tag != null && !catalogs.Contains(tag.TagName))
//                    catalogs.Add(tag.TagName);
//            }
//            video.CatalogBoxTags = catalogs;
//            video.CatalogTags = string.Join(",", catalogs.ToArray());
//            //地区
//            string[] areaNames = video.BKInfo.Region.Split(_spaceSplit, StringSplitOptions.RemoveEmptyEntries);
//            List<string> areas = new List<string>();
//            foreach (string areaName in areaNames)
//            {
//                BoxTagCacheKey key = new BoxTagCacheKey(areaName, TagTypeEnum.Area, video.Main.BKType);
//                BoxTagInfo tag = BoxTagCache.Instance[key];
//                if (tag != null && !areas.Contains(tag.TagName))
//                    areas.Add(tag.TagName);
//            }
//            video.AreaBoxTags = areas;
//            video.AreaTags = string.Join(" ", areas.ToArray());
//            //年份
//            BoxTagCacheKey ykey = new BoxTagCacheKey(video.BKInfo.BKYear, TagTypeEnum.Year, video.Main.BKType);
//            BoxTagInfo ytag = BoxTagCache.Instance[ykey];
//            List<string> years = new List<string>();
//            if (ytag != null)
//                years.Add(ytag.TagName);
//            video.YearBoxTags = years;
//            video.YearTags = string.Join(" ", years.ToArray());
//            //演员
//            video.ActBkTags = new List<string>(video.BKInfo.Actors.Split(_dhSplit, StringSplitOptions.RemoveEmptyEntries));
//            //导演
//            video.DirectorBKTags = new List<string>(video.BKInfo.Directors.Split(_dhSplit, StringSplitOptions.RemoveEmptyEntries));
//        }

//        #endregion
//    }
//}
