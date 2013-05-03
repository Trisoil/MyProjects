//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Xml.Linq;

//namespace Synacast.BoxManage.Core.Cache.Internal.Container
//{
//    using Synacast.BoxManage.Core.Utils;
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Cache.Entity;
//    using Synacast.BoxManage.Core.Cache.Custom;
    
//    /// <summary>
//    /// 点播频道集合容器
//    /// </summary>
//    [Obsolete]
//    internal class VideoNodeContainer : ContainerBase
//    {
//        #region Fields

//        private Dictionary<VideoNodeKey, List<VideoNode>> _dic;
//        private Func<VideoNode, bool> _fun;
//        private static string[] _spacesplit = new string[] { "," };

//        #endregion

//        #region Properties

//        public VideoNodeContainer()
//        {
//            var dic = new Dictionary<VideoNodeKey, List<VideoNode>>();
//            _fun = LoadVideos(dic);
//            _dic = dic;
//        }

//        public Dictionary<VideoNodeKey, List<VideoNode>> Dictionary
//        {
//            get {
//                return _dic;
//            }
//        }

//        public Func<VideoNode, bool> Fun
//        {
//            get {
//                return _fun;
//            }
//        }

//        #endregion

//        /// <summary>
//        /// 初始化平台索引
//        /// </summary>
//        private Func<VideoNode,bool> LoadVideos(Dictionary<VideoNodeKey, List<VideoNode>> dic)
//        {
//            var platforms = PlatformUtils.FormatVodPlatform();
//            foreach (var f in platforms)
//            {
//                dic.Add(new VideoNodeKey(f.PlatformName, 0, f.Licence), new List<VideoNode>());
//                foreach (var type in BoxTypeCache.Instance.Items.Values )
//                {
//                    var typekey = new VideoNodeKey(f.PlatformName, type.TypeID, f.Licence);
//                    dic.Add(typekey, new List<VideoNode>());
//                }
//            }
//            dic.Add(new VideoNodeKey(AuthNode.NoPlatFormName, 0, AuthNode.NoPlatFormLicence), new List<VideoNode>());
//            return CreateFilter(platforms, dic);
//        }

//        /// <summary>
//        /// 对每一个视频判断属于哪个平台
//        /// </summary>
//        private Func<VideoNode, bool> CreateFilter(List<AuthNode> plats, Dictionary<VideoNodeKey, List<VideoNode>> dic)
//        {
//            Func<AuthNode, VideoNode, object> f1 = (plat, video) =>
//            {
//                if (!video.PlatForms.Contains(plat))
//                    video.PlatForms.Add(plat);
//                dic[new VideoNodeKey(plat.PlatformName, 0, plat.Licence)].Add(video);
//                dic[new VideoNodeKey(plat.PlatformName, video.Type, plat.Licence)].Add(video);
//                FormateImage(plat, video);
//                FormateChannelName(plat, video);
//                return null;
//            };
//            Func<AuthNode, VideoNode, object> f2 = (plat, video) =>
//            {
//                dic[new VideoNodeKey(AuthNode.NoPlatFormName, 0, AuthNode.NoPlatFormLicence)].Add(video);
//                FormateImage(video);
//                FormateChannelName(video);
//                return null;
//            };
//            return PlatformUtils.CreateFilter<VideoNode>(plats, f1, f2);
//        }
//    }
//}
