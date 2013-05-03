using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.ServicesFramework.Utils
{
    using log4net;

    public class ServiceUtils
    {
        public static ILog GetLogger(string name)
        {
            log4net.Config.XmlConfigurator.Configure();
            return LogManager.GetLogger(name);
        }
    }
}
