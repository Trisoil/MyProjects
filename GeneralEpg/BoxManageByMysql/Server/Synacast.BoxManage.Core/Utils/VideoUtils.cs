using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Utils
{
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;

    public class VideoUtils
    {
        public static List<YesterdayVideoNode> ConvertVodList(IEnumerable<VideoNode> videos)
        {
            return videos.Select<VideoNode, YesterdayVideoNode>(v => new YesterdayVideoNode() { ChannelId = v.Main.ChannelID, ModifyTime = v.Main.ModifyTime }).ToPerfectList();
        }

        public static List<YesterdayVideoNode> ConvertVodList(IEnumerable<int> videos)
        {
            return videos.Select<int, YesterdayVideoNode>(v => new YesterdayVideoNode() { ChannelId = v, ModifyTime = ListCache.Instance.Dictionary[v].Main.ModifyTime }).ToPerfectList();
        }

        public static IEnumerable<YesterdayVideoNode> ConvertVodIenum(IEnumerable<VideoNode> videos)
        {
            return videos.Select<VideoNode, YesterdayVideoNode>(v => new YesterdayVideoNode() { ChannelId = v.Main.ChannelID, ModifyTime = v.Main.ModifyTime });
        }

        public static IEnumerable<YesterdayVideoNode> ConvertVodIenum(IEnumerable<int> videos)
        {
            return videos.Select<int, YesterdayVideoNode>(v => new YesterdayVideoNode() { ChannelId = v, ModifyTime = ListCache.Instance.Dictionary[v].Main.ModifyTime });
        }

        public static string FormatBoxTag(Dictionary<string, int> dic, string dimension)
        {
            var builder = new StringBuilder(50);
            foreach (var d in dic)
            { 
                if(d.Key.Contains(dimension))
                {
                    builder.Append(d.Key.Substring(0, d.Key.IndexOf(":")));
                    builder.Append(",");
                }
            }
            if (builder.Length < 2)
                return string.Empty;
            builder.Remove(builder.Length - 1, 1);
            return builder.ToString();
        }

        public static string FormatBoxTag(Dictionary<string, int> dic, string dimension, int capacity)
        {
            var builder = new StringBuilder(capacity);
            foreach (var d in dic)
            {
                if (d.Key.Contains(dimension))
                {
                    builder.Append(d.Key.Substring(0, d.Key.IndexOf(":")));
                    builder.Append(",");
                }
            }
            if (builder.Length < 2)
                return string.Empty;
            builder.Remove(builder.Length - 1, 1);
            return builder.ToString();
        }

        public static string FormatStrArray(int capacity, params object[] sources)
        {
            var builder = new StringBuilder(capacity);
            foreach (var source in sources)
            {
                builder.Append(source);
            }
            return builder.ToString();
        }
    }
}
