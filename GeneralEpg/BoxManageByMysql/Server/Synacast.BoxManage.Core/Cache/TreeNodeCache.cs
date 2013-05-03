using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Cache.Language;
    using Synacast.BoxManage.Core.Utils;

    /// <summary>
    /// 树模型主缓存
    /// </summary>
    public class TreeNodeCache : CacheBase<Dictionary<int, TreeNode>>
    {
        private Dictionary<int, int> _roots;
        public static readonly TreeNodeCache Instance = new TreeNodeCache();

        private TreeNodeCache()
        { }

        public Dictionary<int, int> Roots
        {
            get
            {
                return _roots;
            }
        }

        public override void RefreshCache()
        {
            var roots = new Dictionary<int, int>(BoxTypeCache.Instance.Items.Count);
            var dic = new Dictionary<int, TreeNode>(TreeNodeInfoCache.Instance.Items.Count);
            var group = from tree in TreeNodeInfoCache.Instance.Items group tree by tree.BoxTypeID;
            foreach (var key in group)
            {
                BoxTypeNode boxtype;
                if (BoxTypeCache.Instance.Items.TryGetValue(key.Key, out boxtype))
                {
                    if (boxtype.TreeSupport == 1)
                    {
                        var rs = key.Where(v => v.ParentNodeID == 0);
                        foreach (var root in rs)
                        {
                            roots[key.Key] = root.NodeID;
                            var node = new TreeNode() { Node = root, Language = SimpleLanguageHandler.DoubleProcessLang(root.NodeName, root.NodeNote, root.NodeNameEng, null) };
                            LoadChildren(dic, node, key);
                        }
                    }
                }
            }
            _cache = dic;
            _roots = roots;
        }

        private void LoadChildren(Dictionary<int, TreeNode> dic, TreeNode node, IGrouping<int, TreeNodeInfo> key)
        {
            var children = key.Where(v => v.ParentNodeID == node.Node.NodeID).OrderBy(v => v.Rank);
            var rels = TreeNodeChannelRelCache.Instance.Items.Where(rel => rel.NodeID == node.Node.NodeID).OrderBy(v => v.Rank);
            var count = children.Count();
            node.Childrens = new List<int>(count);
            //node.ALLChildrens += count;
            LoadALLChildrens(dic, node, count);
            dic[node.Node.NodeID] = node;
            LoadChannel(dic, node, rels);
            foreach (var child in children)
            {
                node.Childrens.Add(child.NodeID);
                var childnode = new TreeNode() { Depth = node.Depth + 1, Node = child, Language = SimpleLanguageHandler.DoubleProcessLang(child.NodeName, child.NodeNote, child.NodeNameEng, null) };
                LoadChildren(dic, childnode, key);
            }
        }

        private void LoadChannel(Dictionary<int, TreeNode> dic, TreeNode node, IEnumerable<TreeNodeChannelRelInfo> rels)
        {
            var cc = PlatformUtils.FormatVodPlatform().Count;
            node.Channels = new Dictionary<VideoNodeKey, List<int>>(cc);
            node.AllChannels = new Dictionary<VideoNodeKey, List<int>>(cc);
            var count = rels.Count();
            foreach (var rel in rels)
            {
                VideoNode v;
                if (ListCache.Instance.Dictionary.TryGetValue(rel.ChannelID, out v))
                {
                    foreach (var plat in v.PlatForms)
                    {
                        var key = VideoNodeKeyArray.Instance.FindKey(plat.PlatformName, v.Type, plat.Licence); //new VideoNodeKey(plat.PlatformName, v.Type, plat.Licence);
                        if (!node.Channels.ContainsKey(key))
                            node.Channels[key] = new List<int>(count);
                        node.Channels[key].Add(v.Main.ChannelID);
                        ParentLoadChannel(dic, node, key, count, v.Main.ChannelID);
                    }
                }
            }
        }

        private void ParentLoadChannel(Dictionary<int, TreeNode> dic, TreeNode node, VideoNodeKey key, int count, int channelId)
        {
            //LoadAllChannel(node, key, count, channelId);
            var cnode = node;
            while (cnode.Node.ParentNodeID != 0)
            {
                var pnode = dic[cnode.Node.ParentNodeID];
                cnode = pnode;
                LoadAllChannel(cnode, key, count, channelId);
            }
        }

        private void LoadAllChannel(TreeNode node, VideoNodeKey key, int count, int channelId)
        {
            if (!node.AllChannels.ContainsKey(key))
                node.AllChannels[key] = new List<int>(100);
            node.AllChannels[key].Add(channelId);
        }

        private void LoadALLChildrens(Dictionary<int, TreeNode> dic, TreeNode node, int count)
        {
            var cnode = node;
            cnode.ALLChildrens += count;
            while (cnode.Node.ParentNodeID != 0)
            {
                var pnode = dic[cnode.Node.ParentNodeID];
                cnode = pnode;
                cnode.ALLChildrens += count;
            }
        }
    }
}
