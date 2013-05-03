using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Utils
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;

    public class PlatformUtils 
    {
        /// <summary>
        /// 对每一个视频判断属于哪个平台
        /// </summary>
        public static Func<T, bool> CreateFilter<T>(List<AuthNode> plats, params Func<AuthNode, T, object>[] funs) where T : VideoBase
        {
            return v =>
            {
                bool hasFlat = false;
                v.PlatForms = new List<AuthNode>();
                var platformids = v.Main.PlatformIDs.Split(SplitArray.DHArray, StringSplitOptions.RemoveEmptyEntries);
                foreach (AuthNode plat in plats)
                {
                    if (!plat.IsVisable)                              //判断该平台是否显示隐藏频道
                    {
                        if (!v.Main.IsVisible)
                            continue;
                    }
                    if (!string.IsNullOrEmpty(plat.NotContainsNames))  //判断该平台应过滤的频道名称
                    {
                        string[] names = plat.NotContainsNames.Split(SplitArray.DHArray, StringSplitOptions.RemoveEmptyEntries);
                        bool result = false;
                        foreach (string name in names)
                        {
                            result = result || v.Main.Name.ToLower().Contains(name.ToLower());
                        }
                        if (result)
                            continue;
                    }
                    if(platformids.Contains(plat.PlatformId))                              
                    {
                        hasFlat = true;
                        funs[0](plat, v);
                    }
                }
                if (!hasFlat)                                        //不属于任何平台的频道
                {
                    if (funs.Length > 1)
                        funs[1](null, v);
                }
                return false;
            };
        }

        /// <summary>
        /// 得到所有的点播平台
        /// </summary>
        public static List<AuthNode> FormatVodPlatform()
        {
            return PlatFormCache.Instance.Items;
        }

        /// <summary>
        /// 得到所有的直播平台
        /// </summary>
        public static List<AuthNode> FormatLivePlatform()
        {
            return PlatFormCache.Instance.Items;
            //List<AuthNode> list = new List<AuthNode>();
            //list.AddRange(PlatFormCache.Instance.Items);
            //list.AddRange(PlatFormCache.Instance.Customers);
            //return list;
        }

        public static List<VideoNodeKey> ConvertPlatForms(int vid, int channelType)
        {
            List<AuthNode> auths;
            if (channelType == 3)
            {
                auths = ListCache.Instance.Dictionary[vid].PlatForms;
            }
            else
            {
                auths = LiveListCache.Instance.Dictionary[vid].PlatForms;
            }
            var list = new List<VideoNodeKey>(auths.Count);
            foreach (var auth in auths)
            {
                list.Add(VideoNodeKeyArray.Instance.FindKey(auth.PlatformName, 0, auth.Licence));
            }
            return list;
        }
    }
}
