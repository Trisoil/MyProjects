using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Utils
{
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;

    public class ListUtils<T> where T : VideoBase
    {
        #region Fields

        static DateTime _lastpvtime;
        static Random _random = new Random();

        #endregion

        /// <summary>
        /// 设置频道标记
        /// </summary>
        public static void SetFlags(T video, Func<T, bool> fun)
        {
            var flags = new List<string>(4);   //按字母排好序
            if (video.Main.BitRate >= 900)
                flags.Add("b");  //蓝光
            if (video.Main.Name.ToLower().Contains("3d版") && fun(video))
                flags.Add("d");  //3D
            if (video.Main.BitRate >= 600)
                flags.Add("h");  //高清
            if (video.Main.RecommendType == 1)
                flags.Add("n");  //最新
            if (video.Main.RecommendType == 2)
                flags.Add("r");  //推荐
            video.Flags = flags;
            //video.ResFlags = flags.FormatListToStr(SplitArray.Line);
            //video.ResResolution = VideoUtils.FormatStrArray(10, video.Main.VideoWidth, "|", video.Main.VideoHeight);
        }

        public static void SetSlotImage(T video)
        {
            if (!string.IsNullOrEmpty(video.Main.Md5) && video.Main.Md5.Length > 3)
            {
                var slots = new string[4];
                slots[0] = "http://m.img.pplive.cn/";
                slots[1] = VideoUtils.FormatStrArray(8, video.Main.Md5[0], video.Main.Md5[1], "/", video.Main.Md5[2], video.Main.Md5[3], "/");
                slots[2] = video.Main.Md5;
                slots[3] = "/1.jpg";
                video.SlotImage = slots;
            }
        }

        public static void SetLiveSlotImage(T video)
        {
            var slots = new string[4];
            slots[0] = "http://pic.pplive.com/capture_";
            slots[1] = video.Main.ChannelID.ToString();
            slots[2] = "5169_200_150.";
            slots[3] = "html";
            video.SlotImage = slots;
        }

        /// <summary>
        /// 设置频道码流率标识
        /// </summary>
        public static string SetMark(VideoBase video)
        {
            int bit = video.Main.BitRate;
            if (bit < 400)
                return "极速";
            else if (400 <= bit && bit < 600)
                return "标清";
            else if (600 <= bit && bit < 900)
                return "高清";
            return "蓝光";
        }

        private static string[] _noforbidden = new string[1] { "superyp" };

        public static void SetForbidden(VideoBase video)
        {
            if (!string.IsNullOrEmpty(video.Main.ForbiddenArea))
            {
                video.ForbiddenAreas = video.Main.ForbiddenArea.Split(SplitArray.BlankArray, StringSplitOptions.RemoveEmptyEntries);
            }
            else
            {
                video.ForbiddenAreas = _noforbidden;
            }
        }

        /// <summary>
        /// 设置分集名字最大字节数与最大字符数
        /// </summary>
        public static void SetViceMaxNum(VideoNode video, string langType)
        {
            int maxbytes = 0;
            int maxchars = 0;
            if (video.ViceChannels != null)
            {
                foreach (var vice in video.ViceChannels)
                {
                    int bytes = System.Text.Encoding.GetEncoding("gb2312").GetByteCount(vice.Language[langType].Title);
                    if (maxbytes < bytes)
                        maxbytes = bytes;
                    if (maxchars < vice.Language[langType].Title.Length)
                        maxchars = vice.Language[langType].Title.Length;
                }
                if (video.ViceChannels.Count == 0 && !string.IsNullOrEmpty(video.Language[langType].ChannelName))
                {
                    maxbytes = System.Text.Encoding.GetEncoding("gb2312").GetByteCount(video.Language[langType].ChannelName);
                    maxchars = video.Language[langType].ChannelName.Length;
                }
            }
            video.Language[langType].MaxBytes = maxbytes;
            video.Language[langType].MaxChars = maxchars;
        }

        /// <summary>
        /// 模拟频道的PV人数
        /// </summary>
        public static void SetPV(VideoBase video)
        {
            if (video.PV == 0)
            {
                int hours = DateTime.Now.Subtract(video.Main.CreateTime).Hours;
                video.PV = hours * PVBaseNum(video) + PVBaseNum(video);
            }
            else
            {
                if (_lastpvtime != null && _lastpvtime != DateTime.MinValue)
                {
                    int hours = DateTime.Now.Subtract(_lastpvtime).Hours;
                    video.PV = video.PV + hours * PVBaseNum(video);
                }
                else
                {
                    video.PV += PVBaseNum(video);
                }
            }
            _lastpvtime = DateTime.Now;
        }

        private static int PVBaseNum(VideoBase video)
        {
            if (video.Hot > 0)
            {
                int max = video.Hot;
                if (max < int.Parse(AppSettingCache.Instance["PVHourNum"]))
                    max += int.Parse(AppSettingCache.Instance["PVHourNum"]);
                return _random.Next(int.Parse(AppSettingCache.Instance["PVHourNum"]), max) + 1;
            }
            else
            {
                return _random.Next(int.Parse(AppSettingCache.Instance["PVHourNum"])) + 1;
            }
        }
    }
}
