using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.LuceneNet.Live
{
    using Lucene.Net.Index;
    using Synacast.LuceneNetSearcher.Index;
    using Synacast.LuceneNetSearcher.Utils;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache;

    public class LiveEpgIndex 
    {
        public static void AddVideo(LiveVideoNode video, IndexBase index)
        {
            var builder = new StringBuilder();
            foreach (var authnode in video.PlatForms)
            {
                builder.Append(VideoNodeKeyArray.Instance.Items[VideoNodeKeyArray.Instance.FindKey(authnode.PlatformName, 0, authnode.Licence)]);
                builder.Append(SplitArray.DH);
            }
            if (builder.Length < 2)
                return;
            var nodes = CreateNewNode(video,builder.ToString());
            LuceneNetUtils.AddIndex(nodes, index);
        }

        public static void DeleteVideo(LiveVideoNode video, IndexBase index)
        {
            var terms = new List<Term>(video.Items.Count + 1);
            terms.Add(new Term(LiveEpgNode.ChannelId, video.Main.ChannelID.ToString()));
            foreach (var parade in video.Items)
            {
                terms.Add(new Term(LiveEpgNode.ParadeId, parade.ID.ToString()));
            }
            index.DeleteDocument(terms.ToArray());
        }

        public static void InsertVideo(LiveVideoNode video, IndexBase index)
        {
            var builder = new StringBuilder();
            foreach (var authnode in video.PlatForms)
            {
                builder.Append(VideoNodeKeyArray.Instance.Items[VideoNodeKeyArray.Instance.FindKey(authnode.PlatformName, 0, authnode.Licence)]);
                builder.Append(SplitArray.DH);
            }
            if (builder.Length < 2)
                return; 
            var nodes = CreateNewNode(video, builder.ToString());
            index.InsertIndex(nodes);
        }

        /// <summary>
        /// 建立以节目预告名称为索引的字典
        /// </summary>
        private static List<IndexNode> CreateNewNode(LiveVideoNode video, string builder)
        {
            var flag = video.Flags.FormatListToStr(SplitArray.Line, 16);
            var list = new List<IndexNode>(video.Items.Count + 1);
            list.Add(AddNameVideo(video, builder, flag));
            foreach (var parade in video.Items)
            {
                IndexNode node = new IndexNode();
                var indexnames = new List<KeyValuePair<string, string>>(5);
                var paradename = new KeyValuePair<string, string>(LiveEpgNode.ParadeName, parade.Language[CustomArray.LanguageArray[0]].Title);
                var paradecnname = new KeyValuePair<string, string>(LiveEpgNode.ParadeCnName, string.Format("{0}{1}", parade.Language[CustomArray.LanguageArray[0]].Title, SplitArray.IndexCnName));
                var flags = new KeyValuePair<string, string>(LiveEpgNode.Flags, flag);
                var pindex = new KeyValuePair<string, string>(LiveEpgNode.PlatformIndex, string.Format("{0}{1}", builder, SplitArray.IndexFlag));
                var fbcode = new KeyValuePair<string, string>(LiveEpgNode.ForbiddenCode, string.Format("{0},{1}", string.Join(",", ForbiddenAreaCache.Instance.ForbiddenNames.Except(video.ForbiddenAreas).ToArray()), SplitArray.IndexFlag));
                indexnames.Add(paradename);
                indexnames.Add(paradecnname);
                indexnames.Add(flags);
                indexnames.Add(pindex);
                indexnames.Add(fbcode);

                List<KeyValuePair<string, string>> indexvalues = new List<KeyValuePair<string, string>>(5);
                KeyValuePair<string, string> vid = new KeyValuePair<string, string>(LiveEpgNode.IndexValue, parade.ID.ToString());
                KeyValuePair<string, string> paradeid = new KeyValuePair<string, string>(LiveEpgNode.ParadeId, parade.ID.ToString());
                KeyValuePair<string, string> vipflag = new KeyValuePair<string, string>(LiveEpgNode.VipFlag, video.Main.VipFlag.ToString());
                var tabletype = new KeyValuePair<string, string>(LiveEpgNode.TableType, video.Main.TableType.ToString());
                var group = new KeyValuePair<string, string>(LiveEpgNode.IsGroup, video.IsGroup ? "1" : "0");
                indexvalues.Add(vid);
                indexvalues.Add(paradeid);
                indexvalues.Add(vipflag);
                indexvalues.Add(tabletype);
                indexvalues.Add(group);

                List<KeyValuePair<string, int>> indexnumerices = new List<KeyValuePair<string, int>>(3);
                KeyValuePair<string, int> bitrate = new KeyValuePair<string, int>(LiveEpgNode.BitrateRange, video.Main.BitRate);
                KeyValuePair<string, int> hight = new KeyValuePair<string, int>(LiveEpgNode.HightRange, video.Main.VideoHeight);
                KeyValuePair<string, int> width = new KeyValuePair<string, int>(LiveEpgNode.WidthRange, video.Main.VideoWidth);
                KeyValuePair<string, int> paradebegintime = new KeyValuePair<string, int>(LiveEpgNode.ParadeBeginTime, video.Main.VideoWidth);
                indexnumerices.Add(bitrate);
                indexnumerices.Add(hight);
                indexnumerices.Add(width);

                var indexlongs = new List<KeyValuePair<string, long>>(1);
                KeyValuePair<string, long> begintime = new KeyValuePair<string, long>(LiveEpgNode.ParadeBeginTime, parade.StartTime.Ticks);
                indexlongs.Add(begintime);

                node.IndexNames = indexnames;
                node.IndexValues = indexvalues;
                node.IndexNumerices = indexnumerices;
                node.IndexLongs = indexlongs;
                list.Add(node);
            }
            return list;
        }

        /// <summary>
        /// 建立以频道名称为索引的字典
        /// </summary>
        private static IndexNode AddNameVideo(LiveVideoNode video, string builder, string flag)
        {
            IndexNode node = new IndexNode();
            var indexnames = new List<KeyValuePair<string, string>>(5);
            var name = new KeyValuePair<string, string>(LiveEpgNode.Name, video.ChannelName);
            var cnname = new KeyValuePair<string, string>(LiveEpgNode.CnName, string.Format("{0}{1}", video.ChannelName, SplitArray.IndexCnName));
            var flags = new KeyValuePair<string, string>(LiveEpgNode.Flags, flag);
            var pindex = new KeyValuePair<string, string>(LiveEpgNode.PlatformIndex, string.Format("{0}{1}", builder, SplitArray.IndexFlag));
            var fbcode = new KeyValuePair<string, string>(LiveEpgNode.ForbiddenCode, string.Format("{0},{1}", string.Join(",", ForbiddenAreaCache.Instance.ForbiddenNames.Except(video.ForbiddenAreas).ToArray()), SplitArray.IndexFlag));
            indexnames.Add(name);
            indexnames.Add(cnname);
            indexnames.Add(flags);
            indexnames.Add(pindex);
            indexnames.Add(fbcode);

            List<KeyValuePair<string, string>> indexvalues = new List<KeyValuePair<string, string>>(5);
            KeyValuePair<string, string> vid = new KeyValuePair<string, string>(LiveEpgNode.IndexValue, video.Main.ChannelID.ToString());
            KeyValuePair<string, string> channelid = new KeyValuePair<string, string>(LiveEpgNode.ChannelId, video.Main.ChannelID.ToString());
            KeyValuePair<string, string> vipflag = new KeyValuePair<string, string>(LiveEpgNode.VipFlag, video.Main.VipFlag.ToString());
            var tabletype = new KeyValuePair<string, string>(LiveEpgNode.TableType, video.Main.TableType.ToString());
            var group = new KeyValuePair<string, string>(LiveEpgNode.IsGroup, video.IsGroup ? "1" : "0");
            indexvalues.Add(vid);
            indexvalues.Add(channelid);
            indexvalues.Add(vipflag);
            indexvalues.Add(tabletype);
            indexvalues.Add(group);

            List<KeyValuePair<string, int>> indexnumerices = new List<KeyValuePair<string, int>>(3);
            KeyValuePair<string, int> bitrate = new KeyValuePair<string, int>(LiveEpgNode.BitrateRange, video.Main.BitRate);
            KeyValuePair<string, int> hight = new KeyValuePair<string, int>(LiveEpgNode.HightRange, video.Main.VideoHeight);
            KeyValuePair<string, int> width = new KeyValuePair<string, int>(LiveEpgNode.WidthRange, video.Main.VideoWidth);
            indexnumerices.Add(bitrate);
            indexnumerices.Add(hight);
            indexnumerices.Add(width);

            node.IndexNames = indexnames;
            node.IndexValues = indexvalues;
            node.IndexNumerices = indexnumerices;
            return node;
        }
    }
}
