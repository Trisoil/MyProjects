using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Synacast.BoxManage.Core.Job
{
    #region Namespaces

    using Quartz;
    using log4net;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Sync;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;

    #endregion

    public class ListExtensionJob : IJob 
    {
        #region Fields

        static MutexLock _synclock = new MutexLock();
        static MutexLock _lock = new MutexLock();
        static readonly ILog _logger = BoxUtils.GetLogger("quartz.job.listcacheextension.log");

        #endregion

        #region IJob 成员

        public void Execute(JobExecutionContext context)
        {
            if (!_lock.TryEnter() && !NetNodeService.Instance.HasChildren)
                return;
            try
            {
                _logger.Info("ListExtensionJob开始更新");

                //CustomerCache.Instance.RefreshExtension();
                PlatFormInfoCache.Instance.RefreshExtension();
                AuthenticationCache.Instance.RefreshExtension();
                PlatFormCache.Instance.RefreshExtension();

                if (bool.Parse(AppSettingCache.Instance["VodAct"]))   //是否启动点播
                {
                    BoxTypeCache.Instance.RefreshExtension();
                    ListCache.Instance.RefreshExtension();
                    TagCache.Instance.RefreshExtension();
                    RecommandCache.Instance.RefreshExtension();
                }
                if (bool.Parse(AppSettingCache.Instance["LiveAct"]))   //是否启动直播
                {
                    ChannelLiveItemsCache.Instance.RefreshExtension();
                    LiveTypeCache.Instance.RefreshExtension();
                    LiveListCache.Instance.RefreshExtension();
                    LiveTypeDicCache.Instance.RefreshExtension();
                    LiveRecommandCache.Instance.RefreshExtension();
                }

                if (NetNodeService.Instance.HasChildren)     //有孩子节点启动分布式
                {
                    ThreadPool.QueueUserWorkItem(state =>
                    {
                        if (!_synclock.TryEnter())
                            return;
                        try
                        {
                            NetNodeService.Instance.SyncChildrenData();
                        }
                        catch (Exception ex)
                        {
                            _logger.Error(ex);
                        }
                        finally
                        {
                            _synclock.Exit();
                        }
                    });
                }
            }
            catch (Exception ex)
            {
                _logger.Error(ex);
            }
            finally
            {
                _lock.Exit();
                _logger.Info("ListExtensionJob结束更新");
            }
        }

        #endregion
    }
}
