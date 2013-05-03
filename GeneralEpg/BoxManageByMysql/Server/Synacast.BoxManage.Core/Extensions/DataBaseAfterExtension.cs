using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Extensions
{
    using Synacast.ServicesFramework.Extension;
    using Synacast.BoxManage.Core.Job;
    using Synacast.BoxManage.Core.Sync;

    /// <summary>
    /// 后续任务扩展，数据库更新加载
    /// </summary>
    public class DataBaseAfterExtension:IExtension 
    {
        #region IExtension 成员

        public object Execute()
        {
            NetNodeService.Instance.Init();
            ListCacheJob job = new ListCacheJob();
            job.Execute(null);
            return null;
        }

        #endregion
    }
}
