using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal.Container
{
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;

    public class VodContainer : ContainerBase 
    {
        private Dictionary<VideoNodeKey, VodContent> _dic;
        private List<AuthNode> _plats;

        public Dictionary<VideoNodeKey, VodContent> Dictionary
        {
            get {
                return _dic;
            }
        }

        public VodContainer()
        {
            _plats = PlatformUtils.FormatVodPlatform();
            VideoNodeKeyArray.Instance.Init();
            var dic = new Dictionary<VideoNodeKey, VodContent>(VideoNodeKeyArray.Instance.Items.Count);
            foreach (var v in VideoNodeKeyArray.Instance.Items.Keys)
            {
                dic[v] = new VodContent();
            }
            _dic = dic;
        }

        public void Process(VideoNode video)
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
                    _dic[VideoNodeKeyArray.Instance.FindKey(plat.PlatformName, 0, plat.Licence)].AddVideo(video);
                    _dic[VideoNodeKeyArray.Instance.FindKey(plat.PlatformName, video.Type, plat.Licence)].AddVideo(video);
                    FormateImage(plat, video);
                    FormateChannelName(plat, video);
                }
            }
            if (!hasFlat)                                        //不属于任何平台的频道
            {
                _dic[VideoNodeKeyArray.Instance.FindKey(AuthNode.NoPlatFormName, 0, AuthNode.NoPlatFormLicence)].AddVideo(video);
                FormateImage(video);
                FormateChannelName(video);
            }
        }

        public void Complete(Dictionary<int, VideoNode> videos)
        {
            foreach (var d in _dic)
            {
                d.Value.Complete(videos);
            }
        }
    }
}
