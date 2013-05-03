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

    public class CoverJob : IJob 
    {
        static readonly ILog _log = BoxUtils.GetLogger("boxmanage.coverjob.log");
        static MutexLock _lock = new MutexLock();

        #region IJob Members

        public void Execute(JobExecutionContext context)
        {
            if (!_lock.TryEnter())
                return;
            try
            {
                _log.Info("CoverJob开始更新");
                CoverImageCache.Instance.DatabaseInit();
                CoverNavCache.Instance.DatabaseInit();
                //ConverImageCache.Instance.DatabaseInit();
                SplashScreenCache.Instance.DatabaseInit();
            }
            catch (Exception ex)
            {
                _log.Error(ex);
            }
            finally
            {
                _log.Info("CoverJob结束更新");
                _lock.Exit();
            }
        }

        #endregion
    }
}
