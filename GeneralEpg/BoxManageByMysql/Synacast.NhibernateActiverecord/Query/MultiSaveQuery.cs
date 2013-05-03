using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.NhibernateActiveRecord.Query
{
    using NHibernate;
    using Castle.ActiveRecord;

    public class MultiSaveQuery<T>:IActiveRecordQuery 
    {
        IEnumerable<T> _objs;

        public MultiSaveQuery(IEnumerable<T> objs)
        {
            _objs = objs;    
        }

        #region IActiveRecordQuery 成员

        public System.Collections.IEnumerable Enumerate(ISession session)
        {
            throw new NotImplementedException();
        }

        public object Execute(ISession session)
        {
            ITransaction tran = session.BeginTransaction();
            foreach (T obj in _objs)
            {
                session.SaveOrUpdate(obj);
            }
            tran.Commit();
            return null;
        }

        public Type RootType
        {
            get { return typeof(T); }
        }

        #endregion
    }
}
