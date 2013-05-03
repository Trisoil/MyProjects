using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Diagnostics;

namespace Synacast.ProcessManageMonitor.Core.Job
{
    using Quartz;
    using log4net;
    using Synacast.ProcessManageMonitor.Core.Cache;
    using Synacast.ProcessManageMonitor.Core.Utils;

    public class MonitorJob : IJob 
    {
        static object _lock = new object();
        static readonly ILog _log = MonitroUtils.GetLogger("quartz.job.monitorjob.log");

        #region IJob Members

        public void Execute(JobExecutionContext context)
        {
            if (!Monitor.TryEnter(_lock))
                return;
            try
            {
                var dic = AppSettingCache.Instance.Dictionary;
                if (dic != null)
                {
                    foreach (var process in dic)
                    {
                        var count = Process.GetProcessesByName(process.Key).Length;
                        if (count <= 0)
                        {
                            if (System.IO.File.Exists(process.Value))
                            {
                                using (System.Diagnostics.Process.Start(process.Value))
                                { }
                                _log.InfoFormat("进程{0}不存在，重新启动", process.Key);
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                _log.Error(ex);
            }
            finally
            {
                Monitor.Exit(_lock);
            }
        }

        #endregion
    }
}
