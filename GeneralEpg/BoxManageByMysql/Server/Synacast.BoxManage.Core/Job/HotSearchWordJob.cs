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
    using Synacast.LuceneNetSearcher.Stat;

    public class HotSearchWordJob : IJob 
    {
        static readonly ILog _log = BoxUtils.GetLogger("boxmanage.hotwordjob.log");
        static MutexLock _lock = new MutexLock();

        #region IJob Members

        public void Execute(JobExecutionContext context)
        {
            if (!_lock.TryEnter())
                return;
            try
            {
                _log.Info("HotSearchWordJob开始更新");
                LuceneDictionary.Instance.Refresh();
            }
            catch (Exception ex)
            {
                _log.Error(ex);
            }
            finally
            {
                _log.Info("HotSearchWordJob结束更新");
                _lock.Exit();
            }
        }

        #endregion
    }
}
