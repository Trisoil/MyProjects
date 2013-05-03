using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.LuceneNet
{
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.LuceneNetSearcher.Index;
    using Synacast.LuceneNetSearcher.Utils;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache;

    public class EpgIndex
    {
        public static void AddVideo(VideoNode video, IndexBase indexBase)
        {
            var builder = new StringBuilder();
            foreach (var authnode in video.PlatForms)
            {
                if (authnode.SearchTypeIndexList.Contains(video.Type))
                {
                    builder.Append(VideoNodeKeyArray.Instance.Items[VideoNodeKeyArray.Instance.FindKey(authnode.PlatformName, 0, authnode.Licence)]);
                    builder.Append(SplitArray.DH);
                }
            }
            if (builder.Length < 2)
                return;
            
            var node = new IndexNode();
            var indexnames = new List<KeyValuePair<string, string>>(10);
            var name = new KeyValuePair<string, string>(EpgIndexNode.Name, video.Language[CustomArray.LanguageArray[0]].ChannelName);
            var cnname = new KeyValuePair<string, string>(EpgIndexNode.CnName, string.Format("{0}{1}", video.Language[CustomArray.LanguageArray[0]].ChannelName, SplitArray.IndexCnName));
            var flag = video.Flags.FormatListToStr(SplitArray.Line, 16);
            var catalog = new KeyValuePair<string, string>(EpgIndexNode.Catalog, string.Format("{0}{1}", video.Language[CustomArray.LanguageArray[0]].CatalogTags, SplitArray.IndexFlag));
            var area = new KeyValuePair<string, string>(EpgIndexNode.Area, string.Format("{0}{1}", video.Language[CustomArray.LanguageArray[0]].AreaTags, SplitArray.IndexFlag));
            var year = new KeyValuePair<string, string>(EpgIndexNode.Year, string.Format("{0}{1}", video.YearTags, SplitArray.IndexFlag));
            var act = new KeyValuePair<string, string>(EpgIndexNode.Act, string.Format("{0}{1}", video.BKInfo.Actors, SplitArray.IndexFlag));
            var director = new KeyValuePair<string, string>(EpgIndexNode.Director, string.Format("{0}{1}", video.BKInfo.Directors, SplitArray.IndexFlag));
            var flags = new KeyValuePair<string, string>(EpgIndexNode.Flags, flag);
            var pindex = new KeyValuePair<string, string>(EpgIndexNode.PlatformIndex, string.Format("{0}{1}", builder.Remove(builder.Length - 1, 1), SplitArray.IndexFlag));
            var fbcode = new KeyValuePair<string, string>(EpgIndexNode.ForbiddenCode, string.Format("{0},{1}", string.Join(",", ForbiddenAreaCache.Instance.ForbiddenNames.Except(video.ForbiddenAreas).ToArray()), SplitArray.IndexFlag));
            indexnames.Add(name);
            indexnames.Add(cnname);
            indexnames.Add(catalog);
            indexnames.Add(area);
            indexnames.Add(year);
            indexnames.Add(act);
            indexnames.Add(director);
            indexnames.Add(flags);
            indexnames.Add(pindex);
            indexnames.Add(fbcode);

            var indexvalues = new List<KeyValuePair<string, string>>(6);
            var vid = new KeyValuePair<string, string>(EpgIndexNode.IndexValue, video.Main.ChannelID.ToString());
            var vipflag = new KeyValuePair<string, string>(EpgIndexNode.VipFlag, video.Main.VipFlag.ToString());
            var tabletype = new KeyValuePair<string, string>(EpgIndexNode.TableType, video.Main.TableType.ToString());
            var screen = new KeyValuePair<string, string>(EpgIndexNode.Screen, video.Main.TableType == 1 ? "1" : "0");
            var group = new KeyValuePair<string, string>(EpgIndexNode.IsGroup, video.IsGroup ? "1" : "0");
            var type = new KeyValuePair<string, string>(EpgIndexNode.Type, video.Type.ToString());
            indexvalues.Add(vid);
            indexvalues.Add(vipflag);
            indexvalues.Add(tabletype);
            indexvalues.Add(screen);
            indexvalues.Add(group);
            indexvalues.Add(type);

            var indexnumerices = new List<KeyValuePair<string, int>>(3);
            var bitrate = new KeyValuePair<string, int>(EpgIndexNode.BitrateRange, video.Main.BitRate);
            var hight = new KeyValuePair<string, int>(EpgIndexNode.HightRange, video.Main.VideoHeight);
            var width = new KeyValuePair<string, int>(EpgIndexNode.WidthRange, video.Main.VideoWidth);
            indexnumerices.Add(bitrate);
            indexnumerices.Add(hight);
            indexnumerices.Add(width);

            node.IndexNames = indexnames;
            node.IndexValues = indexvalues;
            node.IndexNumerices = indexnumerices;

            LuceneNetUtils.AddIndex(node, indexBase);
        }
    }
}
