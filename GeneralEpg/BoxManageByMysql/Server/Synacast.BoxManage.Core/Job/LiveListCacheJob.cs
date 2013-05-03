using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Job
{
    using Quartz;
    using log4net;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.LuceneNet.Live;

    /// <summary>
    /// 直播频道定时任务，更新当前列表的上线与下线，不做增加。
    /// </summary>
    public class LiveListCacheJob : IJob 
    {
        #region Fields

        static MutexLock _lock = new MutexLock();
        static readonly ILog _logger = BoxUtils.GetLogger("boxmanage.livelistcache.log");

        #endregion

        #region IJob 成员

        public void Execute(JobExecutionContext context)
        {
            if (!_lock.TryEnter())
                return;
            try
            {
                LiveChannelCache.Instance.Init();
                var source = LiveListCache.Instance.Dictionary.Values.Where(l => l.HiddenFlag == 0).Select(l => l.Main.ChannelID).ToPerfectList();
                var compare = LiveChannelCache.Instance.Items.Select(l => l.ChannelID).ToPerfectList();
                var hiddens = source.Except(compare).ToPerfectList();
                var shows = compare.Except(source).ToPerfectList();
                if (hiddens.Count > 0)
                {
                    if (hiddens.Count < AppSettingCache.LiveHiddenMaxCount)
                    {
                        foreach (var hidden in hiddens)
                        {
                            if (LiveListCache.Instance.Dictionary.ContainsKey(hidden))
                            {
                                var video = LiveListCache.Instance.Dictionary[hidden];
                                video.HiddenFlag = 1;
                                LiveEpgIndex.DeleteVideo(video, LiveEpgLuceneNet.Instance.IndexDic);
                                _logger.Info(string.Format("频道{0}已下线", hidden));
                            }
                        }
                    }
                    else
                    {
                        _logger.Info(string.Format("下线个数超过最大阀值，当前下线个数为{0}", hiddens.Count));
                    }
                }
                if (shows.Count > 0)
                {
                    foreach (var show in shows)
                    {
                        if (LiveListCache.Instance.Dictionary.ContainsKey(show))
                        {
                            var video = LiveListCache.Instance.Dictionary[show];
                            video.HiddenFlag = 0;
                            LiveEpgIndex.InsertVideo(video, LiveEpgLuceneNet.Instance.IndexDic);
                            _logger.Info(string.Format("频道{0}已上线", show));
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                _logger.Error(ex);
            }
            finally
            {
                _lock.Exit();
            }
        }

        #endregion
    }
}
