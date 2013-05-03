using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using NHibernate;

namespace Synacast.ServicesFramework.NHibernate
{
    using Castle.ActiveRecord;

    public class SessionManage
    {
        public static ISession OpenSession()
        {
            return OpenSession(typeof(ActiveRecordBase));
        }

        public static ISession OpenSession(Type type)
        {
            return ActiveRecordMediator.GetSessionFactoryHolder().CreateSession(type);       
        }
    }
}
