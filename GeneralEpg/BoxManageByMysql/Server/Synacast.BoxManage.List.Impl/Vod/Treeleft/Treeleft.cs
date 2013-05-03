using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Vod.Treeleft
{
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.BoxManage.List.Vod.Treeleft;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Utils;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class Treeleft : ITreeleft
    {

        #region ITreeleft Members

        public string Treelefts(TreeleftFilter filter)
        {
            return LoadTree(filter, false);
        }

        public string TreeleftsEx(TreeleftFilter filter)
        {
            return LoadTree(filter, true);
        }

        #endregion

        private string LoadTree(TreeleftFilter filter, bool isEx)
        {
            try
            {
                var treeid = filter.treeleftid;
                if (treeid == 0)
                    treeid = TreeNodeCache.Instance.Roots[filter.type];
                TreeNode root;
                if (TreeNodeCache.Instance.Items.TryGetValue(treeid, out root))
                {
                    var key = new VideoNodeKey(filter.platform, filter.type, filter.auth);
                    var xml = new XElement("tree");
                    RangTree(root, xml, filter, key, isEx);
                    return xml.ToString(SaveOptions.DisableFormatting);
                }
                return NoPlatForm();
            }
            catch (KeyNotFoundException)
            {
                return NoPlatForm();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        /// <summary>
        /// 广度优先遍历树
        /// </summary>
        private void RangTree(TreeNode root, XElement xml, TreeleftFilter filter, VideoNodeKey key,bool isEx)
        {
            var queue = new Queue<TreeNode>(root.Childrens.Count * filter.rank);
            var depth = root.Depth + filter.rank;
            var isex = isEx;
            queue.Enqueue(root);
            while (queue.Count > 0)
            {
                var node = queue.Dequeue();
                foreach (var child in node.Childrens)
                {
                    var childnode = TreeNodeCache.Instance.Items[child];
                    if (childnode.Depth < depth)
                        queue.Enqueue(childnode);
                    ResponseXml(childnode, xml, filter, key, isex);
                }
            }
        }

        /// <summary>
        /// XML输出
        /// </summary>
        private void ResponseXml(TreeNode node, XElement xml, TreeleftFilter filter, VideoNodeKey key, bool isEx)
        {
            int count, allcount;
            if (!isEx)
            {
                var filters = VideoNodesUtil.CommonCustomFilter(filter);
                if (filters.Count <= 0)
                {
                    count = GetChannelCount(node, key);
                    allcount = GetAllChannelCount(node, key) + count;
                }
                else
                {
                    count = GetChannelExCount(node, key, filters, filter);
                    allcount = GetAllChannelExCount(node, key, filters, filter) + count;
                }
            }
            else
            {
                var filters = VideoNodesUtil.FormateTagFilter(filter);
                count = GetChannelExCount(node, key, filters, filter);
                allcount = GetAllChannelExCount(node, key, filters, filter) + count;
            }
            xml.Add(new XElement("treeleft",
                new XElement("tlid", node.Node.NodeID),
                new XElement("father_tlid", node.Node.ParentNodeID),
                new XElement("name", node.Language[filter.lang].Title),
                new XElement("remark", node.Language[filter.lang].SubTitle),
                new XElement("subtreeleft_count", node.Childrens.Count),
                new XElement("subtreeleft_totalcount", node.ALLChildrens),
                new XElement("subv_count", count),
                new XElement("subv_totalcount", allcount)
                ));
        }

        private int GetChannelCount(TreeNode node, VideoNodeKey key)
        {
            if (node.Channels.ContainsKey(key))
                return node.Channels[key].Count;
            return 0;
        }

        private int GetAllChannelCount(TreeNode node, VideoNodeKey key)
        {
            if (node.AllChannels.ContainsKey(key))
                return node.AllChannels[key].Count;
            return 0;
        }

        private int GetChannelExCount(TreeNode node, VideoNodeKey key, List<Func<VideoBase, ExFilterBase, bool>> filters, TreeleftFilter filter)
        {
            int count = 0;
            if (node.Channels.ContainsKey(key))
            {
                var cs = node.Channels[key];
                foreach (var c in cs)
                {
                    var result = true;
                    foreach (var f in filters)
                    {
                        result = result && f(ListCache.Instance.Dictionary[c], filter);
                    }
                    if (result)
                        count++;
                }
            }
            return count;
        }

        private int GetAllChannelExCount(TreeNode node, VideoNodeKey key, List<Func<VideoBase, ExFilterBase, bool>> filters, TreeleftFilter filter)
        {
            int count = 0;
            if (node.AllChannels.ContainsKey(key))
            {
                var cs = node.AllChannels[key];
                foreach (var c in cs)
                {
                    var result = true;
                    foreach (var f in filters)
                    {
                        result = result && f(ListCache.Instance.Dictionary[c], filter);
                    }
                    if (result)
                        count++;
                }
            }
            return count;
        }


        private string NoPlatForm()
        {
            return BoxUtils.FormatErrorMsg("不存在该平台下的树形分类");
        }
    }
}
