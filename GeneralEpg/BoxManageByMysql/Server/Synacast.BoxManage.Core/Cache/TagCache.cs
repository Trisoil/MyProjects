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
    using Synacast.BoxManage.Core.Cache.Language;
    using Synacast.BoxManage.Core.LuceneNet.Tag;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.NhibernateActiveRecord;

    #endregion

    [Serializable]
    public class TagCache : CacheBase<Dictionary<TagNodeKey, List<TagNode>>>
    {
        private static readonly ILog _log = BoxUtils.GetLogger("lucenenet.queue.log");
        private static MutexLock _indexlock = new MutexLock();
        public static readonly TagCache Instance = new TagCache();

        private TagCache()
        { }

        public override void RefreshCache()
        {
            var dic = new Dictionary<TagNodeKey,List<TagNode>>(TagTypeCache.Instance.Dictionary.Count);
            var tagtypes = TagTypeCache.Instance.Dictionary.Values;
            var pcount = PlatformUtils.FormatVodPlatform().Count;
            var tagtypequery = from tagtype in tagtypes                                          
                               join boxtag in BoxTagCache.Instance.Dictionary.Values
                               on tagtype.TagTypeID equals boxtag.TagType
                               into boxtags
                               select new
                               {
                                   TagType = tagtype,
                                   BoxTags = boxtags.OrderBy(v => v.TagRank)
                               };
            foreach (var tag in tagtypequery)
            {
                var key = new TagNodeKey() { Dimension = tag.TagType.TagTypeLimitName, Type = tag.TagType.BoxTypeID };
                //dic[key] = new List<TagNode>(tag.BoxTags.Count());
                LoadTagCount(dic, key, tag.BoxTags.Count());
                var relquery = from boxtag in tag.BoxTags
                               join rel in ChannelTagRelCache.Instance.Items     //加载频道
                               on boxtag.ID equals rel.TagID
                               into rels
                               select new
                               {
                                   Node = new TagNode()
                                   {
                                       Language = SimpleLanguageHandler.ProcessLang(boxtag.TagName, boxtag.TagNameEng),
                                       Channels = new Dictionary<VideoNodeKey, List<int>>(pcount)
                                   },
                                   Rels = rels.OrderByDescending(r => r.Rank)//OrderBy(r => r.Rank)
                               };
                foreach (var rel in relquery)
                {
                    var result = false;
                    var rcount = rel.Rels.Count();
                    foreach (var r in rel.Rels)
                    {
                        VideoNode v;
                        if (ListCache.Instance.Dictionary.TryGetValue(r.ChannelID, out v))
                        {
                            foreach (var plat in v.PlatForms)
                            {
                                var vk = VideoNodeKeyArray.Instance.FindKey(plat.PlatformName, v.Type, plat.Licence); //new VideoNodeKey(plat.PlatformName, v.Type, plat.Licence);
                                if (!rel.Node.Channels.ContainsKey(vk))
                                    rel.Node.Channels.Add(vk, new List<int>(rcount));
                                rel.Node.Channels[vk].Add(v.Main.ChannelID);
                                result = true;
                            }
                        }
                    }
                    if (result)
                        dic[key].Add(rel.Node);
                }
            }
            if (bool.Parse(AppSettingCache.Instance["ActDirectorTag"]))
                LoadActorDirector(dic);
            _cache = dic;
            LuceneInitIndex(dic);
            GC.Collect();
        }

        private void LoadTagCount(Dictionary<TagNodeKey, List<TagNode>> dic, TagNodeKey key, int count)
        {
            var c = count;
            if (bool.Parse(AppSettingCache.Instance["ActDirectorTag"]))
            {
                try
                {
                    if (key.Dimension == CustomArray.ActDimension)
                    {
                        if (key.Type == 1)
                        {
                            var sql = "select count(*) from bktags where tagtype=4 and bktype=0";
                            c = Convert.ToInt32(DbObject<BoxTypeInfo>.ExecuteScalarBySql(sql));
                        }
                        else if (key.Type == 2)
                        {
                            var sql = "select count(*) from bktags where tagtype=4 and bktype=1";
                            c = Convert.ToInt32(DbObject<BoxTypeInfo>.ExecuteScalarBySql(sql));
                        }
                    }
                    else if (key.Dimension == CustomArray.DirectorDimension)
                    {
                        if (key.Type == 1)
                        {
                            var sql = "select count(*) from bktags where tagtype=5 and bktype=0";
                            c = Convert.ToInt32(DbObject<BoxTypeInfo>.ExecuteScalarBySql(sql));
                        }
                        else if (key.Type == 2)
                        {
                            var sql = "select count(*) from bktags where tagtype=5 and bktype=1";
                            c = Convert.ToInt32(DbObject<BoxTypeInfo>.ExecuteScalarBySql(sql));
                        }
                    }
                }
                catch (Exception ex)
                {
                    _log.Error(ex);
                    dic[key] = new List<TagNode>(1000);
                }
            }
            dic[key] = new List<TagNode>(c);
        }

        private void LuceneInitIndex(Dictionary<TagNodeKey, List<TagNode>> tags)
        {
            if (bool.Parse(AppSettingCache.Instance["TagSearchAct"]))
            {
                Thread thread = new Thread(state =>
                {
                    if (!_indexlock.TryEnter())
                        return;
                    try
                    {
                        _log.Info("TagCache开始建立LuceneNet索引");
                        var dic = TagLuceneNet.Instance.Init();
                        foreach (var kv in tags)
                        {
                            foreach (var tag in kv.Value)
                            {
                                TagIndex.AddTag(tag, kv.Key.Dimension, dic);
                            }
                        }
                        TagLuceneNet.Instance.CompleteIndex(dic);
                        _log.Info("TagCache索引建立完成");
                    }
                    catch (Exception ex)
                    {
                        _log.Error(ex);
                    }
                    finally
                    {
                        _indexlock.Exit();
                    }
                });
                thread.Start();
            }
        }

        private void LoadActorDirector(Dictionary<TagNodeKey, List<TagNode>> dic)
        {
            foreach (var video in ListCache.Instance.Dictionary.Values)
            {
                var akey = GetNodeKey(CustomArray.ActDimension, video.Type);
                if (dic.ContainsKey(akey))
                {
                    var acts = video.BKInfo.Actors.FormatStrToArray(SplitArray.DHArray);
                    LoadBKTags(dic, akey, video, acts);
                }
                var dkey = GetNodeKey(CustomArray.DirectorDimension, video.Type);
                if (dic.ContainsKey(dkey))
                {
                    var directors = video.BKInfo.Directors.FormatStrToArray(SplitArray.DHArray);
                    LoadBKTags(dic, dkey, video, directors);
                }
            }
        }

        private void LoadBKTags(Dictionary<TagNodeKey, List<TagNode>> dic, TagNodeKey key, VideoNode video, string[] dimensions)
        {
            foreach (var dimension in dimensions)
            {
                var node = dic[key].FirstOrDefault(v => v.Language[CustomArray.LanguageArray[0]].Title == dimension);
                if (node != null)
                {
                    foreach (var plat in video.PlatForms)
                    {
                        var vk = VideoNodeKeyArray.Instance.FindKey(plat.PlatformName, video.Type, plat.Licence);
                        if (!node.Channels.ContainsKey(vk))
                            node.Channels.Add(vk, new List<int>());
                        node.Channels[vk].Add(video.Main.ChannelID);
                    }
                }
                else
                {
                    node = new TagNode()
                    {
                        Language = SimpleLanguageHandler.ProcessLang(dimension, string.Empty),
                        Channels = new Dictionary<VideoNodeKey, List<int>>()//VideoNodeKeyArray.Instance.InitChannels() 
                    };
                    foreach (var plat in video.PlatForms)
                    {
                        var vk = VideoNodeKeyArray.Instance.FindKey(plat.PlatformName, video.Type, plat.Licence);
                        if (!node.Channels.ContainsKey(vk))
                            node.Channels.Add(vk, new List<int>());
                        node.Channels[vk].Add(video.Main.ChannelID);
                    }
                    dic[key].Add(node);
                }
            }
        }

        private List<TagNodeKey> _list = new List<TagNodeKey>(14);

        private TagNodeKey GetNodeKey(string dimension, int type)
        {
            var key = _list.FirstOrDefault(v => v.Dimension == dimension && v.Type == type);
            if (key == null)
            {
                key = new TagNodeKey() { Dimension = dimension, Type = type };
                _list.Add(key);
            }
            return key;
        }
    }
}
