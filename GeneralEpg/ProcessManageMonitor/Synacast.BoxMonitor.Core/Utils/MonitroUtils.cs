using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.ProcessManageMonitor.Core.Utils
{
    using log4net;
    public static class MonitroUtils
    {
        public static ILog GetLogger(string name)
        {
            log4net.Config.XmlConfigurator.Configure();
            return LogManager.GetLogger(name);
        }
    }
}
