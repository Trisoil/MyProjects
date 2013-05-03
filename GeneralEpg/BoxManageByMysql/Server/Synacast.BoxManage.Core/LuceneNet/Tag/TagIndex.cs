using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.LuceneNet.Tag
{
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.LuceneNetSearcher.Index;
    using Synacast.LuceneNetSearcher.Utils;

    public class TagIndex
    {
        public static void AddTag(TagNode tag, string dimension, IndexBase index)
        {
            var builder = new StringBuilder();
            //Dictionary<int, string[]> fbcodes = new Dictionary<int, string[]>(tag.Channels.Keys.Count);
            foreach (var plat in tag.Channels.Keys)
            {
                var indexx = VideoNodeKeyArray.Instance.Items[VideoNodeKeyArray.Instance.FindKey(plat.PlatForm, 0, plat.Auth)];
                builder.Append(indexx);
                builder.Append(SplitArray.DH);
                //IEnumerable<string> fbcode = null;
                //var query = tag.Channels[plat].Select(v => ListCache.Instance.Dictionary[v].ForbiddenAreas);
                //foreach (var q in query)
                //{
                //    if (fbcode == null)
                //        fbcode = q;
                //    else
                //        fbcode = fbcode.Intersect(q);
                //}
                //fbcodes[indexx] = fbcode.ToArray();
            }
            if (builder.Length < 2)
                return;
            var node = new IndexNode();
            var indexnames = new List<KeyValuePair<string, string>>(3);
            var name = new KeyValuePair<string, string>(EpgIndexNode.Name, tag.Language[CustomArray.LanguageArray[0]].Title);
            var cnname = new KeyValuePair<string, string>(EpgIndexNode.CnName, string.Format("{0}{1}", tag.Language[CustomArray.LanguageArray[0]].Title, SplitArray.IndexCnName));
            var pindex = new KeyValuePair<string, string>(EpgIndexNode.PlatformIndex, string.Format("{0}{1}", builder.Remove(builder.Length - 1, 1), SplitArray.IndexFlag));
            //foreach (var fbcode in fbcodes)
            //{
            //    var fb = new KeyValuePair<string, string>(string.Format("{0}{1}", EpgIndexNode.ForbiddenCode, fbcode.Key), string.Format("{0},{1}", string.Join(",", ForbiddenAreaCache.Instance.ForbiddenNames.Except(fbcode.Value).ToArray()), SplitArray.IndexFlag));
            //    indexnames.Add(fb);
            //}
            indexnames.Add(name);
            indexnames.Add(cnname);
            indexnames.Add(pindex);

            var indexvalues = new List<KeyValuePair<string, string>>(2);
            var vid = new KeyValuePair<string, string>(EpgIndexNode.IndexValue, tag.Language[CustomArray.LanguageArray[0]].Title);
            var dimen = new KeyValuePair<string, string>(EpgIndexNode.Flags, dimension);
            indexvalues.Add(vid);
            indexvalues.Add(dimen);

            node.IndexNames = indexnames;
            node.IndexValues = indexvalues;

            LuceneNetUtils.AddIndex(node, index);
        }
    }
}
