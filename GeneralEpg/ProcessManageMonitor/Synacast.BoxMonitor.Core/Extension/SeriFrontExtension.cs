using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.ProcessManageMonitor.Core.Extension
{
    using Synacast.ServicesFramework.Extension;
    using Synacast.ProcessManageMonitor.Core.Cache;

    public class SeriFrontExtension : IExtension 
    {
        #region IExtension Members

        public object Execute()
        {
            Console.WriteLine("ProcessManageMonitor Loading...");
            AppSettingCache.Instance.RefreshCache();
            return null;
        }

        #endregion
    }
}
