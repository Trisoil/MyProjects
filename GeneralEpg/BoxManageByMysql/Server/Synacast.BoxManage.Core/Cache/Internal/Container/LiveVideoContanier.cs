using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal.Container
{
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;

    /// <summary>
    /// 直播频道容器
    /// </summary>
    internal class LiveVideoContanier : ContainerBase 
    {
        #region Fields

        private Dictionary<VideoNodeKey, List<LiveVideoNode>> _dic;
        private List<AuthNode> _plats;

        #endregion

        #region Properties

        public LiveVideoContanier()
        {
            _plats = PlatformUtils.FormatLivePlatform();
            VideoNodeKeyArray.Instance.Init();
            var dic = new Dictionary<VideoNodeKey, List<LiveVideoNode>>();
            foreach (var f in _plats)
            {
                dic.Add(VideoNodeKeyArray.Instance.FindKey(f.PlatformName, 0, f.Licence), new List<LiveVideoNode>());
                foreach (var type in LiveTypeCache.Instance.Items)
                {
                    dic.Add(new VideoNodeKey(f.PlatformName, type.ID, f.Licence), new List<LiveVideoNode>());
                }
            }
            dic.Add(VideoNodeKeyArray.Instance.FindKey(AuthNode.NoPlatFormName, 0, AuthNode.NoPlatFormLicence), new List<LiveVideoNode>());
            _dic = dic;
        }

        public Dictionary<VideoNodeKey, List<LiveVideoNode>> Dictionary
        {
            get
            {
                return _dic;
            }
        }

        #endregion

        public void Process(LiveVideoNode video)
        {
            bool hasFlat = false;
            video.PlatForms = new List<AuthNode>();
            var platformids = video.Main.PlatformIDs.Split(SplitArray.DHArray, StringSplitOptions.RemoveEmptyEntries);
            foreach (AuthNode plat in _plats)
            {
                if (!plat.IsVisable)                              //判断该平台是否显示隐藏频道
                {
                    if (!video.Main.IsVisible)
                        continue;
                }
                if (!string.IsNullOrEmpty(plat.NotContainsNames))  //判断该平台应过滤的频道名称
                {
                    string[] names = plat.NotContainsNames.Split(SplitArray.DHArray, StringSplitOptions.RemoveEmptyEntries);
                    bool result = false;
                    foreach (string name in names)
                    {
                        result = result || video.Main.Name.ToLower().Contains(name.ToLower());
                    }
                    if (result)
                        continue;
                }
                if (platformids.Contains(plat.PlatformId))
                {
                    hasFlat = true;
                    if (!video.PlatForms.Contains(plat))
                        video.PlatForms.Add(plat);
                    _dic[VideoNodeKeyArray.Instance.FindKey(plat.PlatformName, 0, plat.Licence)].Add(video);
                    foreach (var type in video.Types)
                    {
                        _dic[VideoNodeKeyArray.Instance.FindKey(plat.PlatformName, type, plat.Licence)].Add(video);
                    }
                    FormateImage(plat, video);
                    FormateChannelName(plat, video);
                }
            }
            if (!hasFlat)                                        //不属于任何平台的频道
            {
                _dic[VideoNodeKeyArray.Instance.FindKey(AuthNode.NoPlatFormName, 0, AuthNode.NoPlatFormLicence)].Add(video);
                FormateImage(video);
                FormateChannelName(video);
            }
        }
    }
}
