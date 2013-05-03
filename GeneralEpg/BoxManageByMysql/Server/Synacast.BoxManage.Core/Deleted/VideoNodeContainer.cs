//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Xml.Linq;

//namespace Synacast.BoxManage.Core.Deleted
//{
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Cache.Entity;
//    using Synacast.BoxManage.Core.Cache.Custom;
    
//    /// <summary>
//    /// 频道集合容器
//    /// </summary>
//    internal class VideoNodeContainer
//    {
//        private Dictionary<VideoNodeKey, List<VideoNode>> _dic;
//        private Func<VideoNode, bool> _fun;
//        private static string[] _spacesplit = new string[] { "," };

//        public VideoNodeContainer()
//        {
//            Dictionary<VideoNodeKey, List<VideoNode>> dic = new Dictionary<VideoNodeKey, List<VideoNode>>();
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

//        private Func<VideoNode,bool> LoadVideos(Dictionary<VideoNodeKey, List<VideoNode>> dic)
//        {
//            var platforms = PlatFormCache.Instance.Items;
//            foreach (var f in platforms)
//            {
//                dic.Add(new VideoNodeKey(f.Name, 0, f.Licence), new List<VideoNode>());
//                foreach (BoxTypeInfo type in BoxTypeCache.Instance.Items)
//                {
//                    VideoNodeKey typekey = new VideoNodeKey(f.Name, type.TypeID, f.Licence);
//                    dic.Add(typekey, new List<VideoNode>());
//                }
//            }
//            dic.Add(new VideoNodeKey(AuthNode.NoPlatFormName, 0, AuthNode.NoPlatFormLicence), new List<VideoNode>());
//            return CreateFilter(platforms, dic);
//        }

//        /// <summary>
//        /// 对每一个视频判断属于哪个平台
//        /// </summary>
//        /// <param name="plats"></param>
//        /// <param name="dic"></param>
//        /// <returns></returns>
//        private Func<VideoNode, bool> CreateFilter(List<AuthNode> plats, Dictionary<VideoNodeKey, List<VideoNode>> dic)
//        {
//            return v =>
//            {
//                bool hasFlat = false;
//                foreach (AuthNode plat in plats)
//                {
//                    if (!plat.IsVisable)                               //判断该平台是否显示隐藏频道
//                    {
//                        if (!v.Main.IsVisible)
//                            continue;
//                    }
//                    if (!string.IsNullOrEmpty(plat.NotContainsNames))   //判断该平台应过滤的频道名称
//                    {
//                        string[] names = plat.NotContainsNames.Split(_spacesplit, StringSplitOptions.RemoveEmptyEntries);
//                        bool result = false;
//                        foreach (string name in names)
//                        {
//                            result = result || v.Main.Name.ToLower().Contains(name.ToLower());
//                        }
//                        if (!result)
//                        {
//                            hasFlat = true;
//                            dic[new VideoNodeKey(plat.Name, 0, plat.Licence)].Add(v);
//                            dic[new VideoNodeKey(plat.Name, v.Type, plat.Licence)].Add(v);
//                            FormateImage(plat, v);
//                            FormateChannelName(plat, v);
//                        }
//                    }
//                    else if (!string.IsNullOrEmpty(plat.ContainsNames))  //判断该平台只包含的频道名称
//                    {
//                        string[] names = plat.ContainsNames.Split(_spacesplit, StringSplitOptions.RemoveEmptyEntries);
//                        bool result = false;
//                        foreach (string name in names)
//                        {
//                            result = result || v.Main.Name.ToLower().EndsWith(name.ToLower());
//                        }
//                        if (result)
//                        {
//                            hasFlat = true;
//                            dic[new VideoNodeKey(plat.Name, 0, plat.Licence)].Add(v);
//                            dic[new VideoNodeKey(plat.Name, v.Type, plat.Licence)].Add(v);
//                            FormateImage(plat, v);
//                            FormateChannelName(plat, v);
//                        }
//                    }
//                    else                                               //以上两项为空将该视频加入该平台中
//                    {
//                        hasFlat = true;
//                        dic[new VideoNodeKey(plat.Name, 0, plat.Licence)].Add(v);
//                        dic[new VideoNodeKey(plat.Name, v.Type, plat.Licence)].Add(v);
//                        FormateImage(plat, v);
//                        FormateChannelName(plat, v);
//                    }
//                }
//                if (!hasFlat)                                        //不属于任何平台的频道
//                {
//                    dic[new VideoNodeKey(AuthNode.NoPlatFormName, 0, AuthNode.NoPlatFormLicence)].Add(v);
//                    FormateImage(v);
//                    FormateChannelName(v);
//                }
//                return false;
//            };
//        }

//        #region 格式化视频属性

//        private void FormateImage(AuthNode plat, VideoNode video)
//        {
//            if (!string.IsNullOrEmpty(plat.FormateImage))
//            {
//                video.ResImage = video.BKInfo.CoverPic.Replace("75X100", plat.FormateImage);
//            }
//            else
//            {
//                video.ResImage = video.BKInfo.CoverPic;
//            }
//        }

//        private void FormateImage(VideoNode video)
//        {
//            video.ResImage = video.BKInfo.CoverPic;
//        }

//        private void FormateChannelName(AuthNode plat, VideoNode video)
//        {
//            if (!string.IsNullOrEmpty(plat.FormateName))
//            {
//                int index = video.Main.Name.LastIndexOf(plat.FormateName);
//                if (index >= 0)
//                    video.ChannelName = video.Main.Name.Substring(0, index);
//                else
//                    video.ChannelName = video.Main.Name;
//            }
//            else
//            {
//                video.ChannelName = video.Main.Name;
//            }
//        }

//        private void FormateChannelName(VideoNode video)
//        {
//            video.ChannelName = video.Main.Name;
//        }

//        #endregion
//    }
//}
