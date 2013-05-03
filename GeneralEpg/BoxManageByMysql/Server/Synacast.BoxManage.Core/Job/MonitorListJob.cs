using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Job
{
    using Quartz;
    using log4net;
    using Synacast.BoxManage.Core.Sync;
    using Synacast.BoxManage.Core.Utils;

    /// <summary>
    /// 监控任务，当该节点未接受到父节点的数据时，连接数据库更新
    /// </summary>
    public class MonitorListJob : IJob 
    {
        #region Fields

        static readonly ILog _logger = BoxUtils.GetLogger("quartz.job.monitorlist.log");

        #endregion

        #region IJob 成员

        public void Execute(JobExecutionContext context)
        {
            try
            {
                if (!NetNodeService.Instance.IsRefresh)
                {
                    _logger.Info("缓存未更新，连接数据库同步数据");
                    ListCacheJob job = new ListCacheJob();
                    job.Execute(context);
                }
                else
                {
                    _logger.Info("缓存已更新");
                    NetNodeService.Instance.IsRefresh = false;
                }
            }
            catch (Exception ex)
            {
                _logger.Error(ex);
            }
            finally
            {
                _logger.Info("MonitorListJob结束更新");
            }
        }

        #endregion
    }
}
