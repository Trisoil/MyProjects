using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.Threading;

namespace Synacast.ServicesFramework
{
    using Synacast.ServicesFramework.Reflection;
    using Synacast.ServicesFramework.Extension;

    public class Engine
    {
        /// <summary>
        /// 启动服务
        /// </summary>
        public void Start()
        {
            InitServices();
        }

        /// <summary>
        /// 按顺序启动ServiceItems.xml中配置的服务
        /// </summary>
        private void InitServices()
        {
            string configPath = string.Format("{0}/Config/ServiceItems.xml", AppDomain.CurrentDomain.BaseDirectory);
            var root = XElement.Load(configPath);
            foreach (var element in root.Elements())
            {
                if (element.Name.LocalName == "SyncServices")
                {
                    var services = from data in element.Elements("Service") select data;
                    LoadService(services);
                }
                else if (element.Name.LocalName == "AsyncServices")
                {
                    var services = from data in element.Elements("Service") select data;
                    ThreadPool.QueueUserWorkItem(state =>
                    {
                        LoadService(services);
                    });
                }
            }
        }

        /// <summary>
        /// 加载服务
        /// </summary>
        /// <param name="services"></param>
        private void LoadService(IEnumerable<XElement> services)
        {
            foreach (var service in services)
            {
                if (service.Element("Extensions") != null && service.Element("Extensions").Element("FrontExtensions") != null)
                {
                    var fronts = from extension in service.Element("Extensions").Element("FrontExtensions").Elements("Extension") select (string)extension.Attribute("Type");
                    ExtensionExecute(fronts);
                }
                ServiceExecute((string)service.Attribute("Type"));
                if (service.Element("Extensions") != null && service.Element("Extensions").Element("AfterExtensions") != null)
                {
                    var afters = from extension in service.Element("Extensions").Element("AfterExtensions").Elements("Extension") select (string)extension.Attribute("Type");
                    ExtensionExecute(afters);
                }
            }
        }

        /// <summary>
        /// 启动扩展服务
        /// </summary>
        /// <param name="extensions"></param>
        private void ExtensionExecute(IEnumerable<string> extensions)
        {
            foreach (string extension in extensions)
            {
                IExtension ex = ReflectionOptimizer.CreateInstanceMethod(extension)() as IExtension;
                if (ex != null)
                    ex.Execute();
            }
        }

        /// <summary>
        /// 启动主服务
        /// </summary>
        /// <param name="service"></param>
        private void ServiceExecute(string service)
        {
            IServiceFactory sf = ReflectionOptimizer.CreateInstanceMethod(service)() as IServiceFactory;
            if (sf != null)
                sf.ServiceInit();
        }
    }
}
