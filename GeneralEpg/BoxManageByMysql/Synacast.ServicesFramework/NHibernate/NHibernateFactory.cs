using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

using NHibernate;
using NHibernate.Cfg;

namespace Synacast.ServicesFramework.NHibernate
{
    using Castle.ActiveRecord;

    public class NHibernateFactory : IServiceFactory 
    {
        #region IServiceFactory 成员

        public object ServiceInit()
        {
            string configPath = string.Format("{0}/Config/NHibernateAssemblies.xml", AppDomain.CurrentDomain.BaseDirectory);
            var root = XElement.Load(configPath);
            var groups = from g in root.Elements("GroupAssemblies") select new NHibernateAssemblyInfo() { Type = (string)g.Attribute("Type"), Assemblies = g.Elements("assembly").Select(n => (string)n.Attribute("name")).ToList() };
            foreach (NHibernateAssemblyInfo g in groups)
            {
                Type type = typeof(ActiveRecordBase);
                if (!string.IsNullOrEmpty(g.Type))
                    type = Type.GetType(g.Type);
                 Configuration cfg = ActiveRecordMediator.GetSessionFactoryHolder().GetConfiguration(type);
                 foreach (string assembly in g.Assemblies)
                 {
                     cfg.AddAssembly(assembly);
                 }
            }
            return null;
        }

        #endregion
    }
}
