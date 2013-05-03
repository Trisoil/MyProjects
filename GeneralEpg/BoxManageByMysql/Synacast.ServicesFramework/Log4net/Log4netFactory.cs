using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Synacast.ServicesFramework.Log4net
{
    public class Log4netFactory:IServiceFactory 
    {

        #region IServiceFactory 成员

        public object ServiceInit()
        {
            string xmlPath = string.Format("{0}/Config/log.config", AppDomain.CurrentDomain.BaseDirectory);
            log4net.Config.XmlConfigurator.ConfigureAndWatch(new FileInfo(xmlPath));
            return null;
        }

        #endregion
    }
}
