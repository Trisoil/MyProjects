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

    public class ListCacheJob : IJob 
    {
        #region Fields

        static MutexLock _synclock = new MutexLock();
        static MutexLock _lock = new MutexLock();
        static readonly ILog _logger = BoxUtils.GetLogger("quartz.job.listcache.log");

        #endregion

        #region IJob 成员

        public void Execute(JobExecutionContext context)
        {
            if (!_lock.TryEnter())
                return;
            try
            {
                _logger.Info("ListCacheJob开始更新");

                PlatFormInfoCache.Instance.DatabaseInit();
                AuthenticationCache.Instance.DatabaseInit();
                //CustomerCache.Instance.DatabaseInit();
                PlatFormCache.Instance.DatabaseInit();
                VideoNodeKeyArray.Instance.Reset();
                
                _logger.Info("BKInfo开始同步");
                BKInfoCache.Instance.Init();
                BKTypeCache.Instance.Init();
                _logger.Info("BKInfo结束同步");

                ForbiddenAreaCache.Instance.DatabaseInit();

                if (bool.Parse(AppSettingCache.Instance["VodAct"]))   //是否启动点播
                {
                    BoxTagCache.Instance.Init();

                    _logger.Info("Channels开始同步");
                    ChannelCache.Instance.Init();
                    ViceChnanelCache.Instance.Init();
                    _logger.Info("Channels结束同步");

                    ChannelTagRelCache.Instance.Init();

                    ChannelRecomRelCache.Instance.Init();

                    BoxTypeInfoCache.Instance.Init();
                    TagTypeCache.Instance.Init();
                    BoxTypeCache.Instance.DatabaseInit();

                    _logger.Info("ListCache开始计算整合");
                    ListCache.Instance.DatabaseInit();
                    _logger.Info("ListCache结束整合");

                    _logger.Info("TagCache开始计算整合");
                    TagCache.Instance.DatabaseInit();
                    _logger.Info("TagCache结束整合");

                    _logger.Info("RecommandCache开始计算整合");
                    RecommandCache.Instance.DatabaseInit();
                    _logger.Info("RecommandCache结束整合");

                    _logger.Info("TreeNodeCache开始计算整合");
                    TreeNodeInfoCache.Instance.Init();
                    TreeNodeChannelRelCache.Instance.Init();
                    TreeNodeCache.Instance.DatabaseInit();
                    _logger.Info("TreeNodeCache结束整合");

                }

                if (bool.Parse(AppSettingCache.Instance["LiveAct"]))   //是否启动直播
                {
                    _logger.Info("LiveCache开始计算整合");
                    LiveChannelCache.Instance.Init();
                    ChannelLiveItemsCache.Instance.DatabaseInit();
                    ChannelLiveRelCache.Instance.Init();
                    ChannelLiveRecomRelCache.Instance.Init();
                    ChannelLiveCustomerRelCache.Instance.Init();
                    LiveTypeCache.Instance.DatabaseInit();
                    LiveListCache.Instance.DatabaseInit();
                    LiveTypeDicCache.Instance.DatabaseInit();
                    LiveRecommandCache.Instance.DatabaseInit();
                    _logger.Info("LiveCache结束整合");
                }

                CoverImageCache.Instance.DatabaseInit();
                CoverNavCache.Instance.DatabaseInit();
                //ConverImageCache.Instance.DatabaseInit();
                SplashScreenCache.Instance.DatabaseInit();

                //if (NetNodeService.Instance.HasChildren)     //有孩子节点启动分布式
                //{
                //    Thread thread = new Thread(state => {
                //        if (!_synclock.TryEnter())
                //            return;
                //        try
                //        {
                //            NetNodeService.Instance.SyncChildrenData();
                //        }
                //        catch (Exception ex)
                //        {
                //            _logger.Error(ex);
                //        }
                //        finally
                //        {
                //            _synclock.Exit();
                //        }
                //    });
                //    thread.Start();
                //}
            }
            catch (Exception ex)
            {
                _logger.Error(ex);
            }
            finally
            {
                _logger.Info("ListCacheJob结束更新");
                _lock.Exit();
            }
        }

        #endregion
    }
}
