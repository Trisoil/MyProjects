using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.NhibernateActiveRecord.Query
{
    using NHibernate;
    using Castle.ActiveRecord;

    public class HqlQuery<T> : IActiveRecordQuery where T : new()
    {
        private string _hql;
        private object[] _paras;

        public HqlQuery(string hql, params object[] paras)
        {
            _hql = hql;
            _paras = paras;
        }
        #region IActiveRecordQuery 成员

        public System.Collections.IEnumerable Enumerate(NHibernate.ISession session)
        {
            throw new NotImplementedException();
        }

        public object Execute(NHibernate.ISession session)
        {
            IQuery query = session.CreateQuery(_hql);
            if (_paras != null)
            {
                for (int i = 0; i < _paras.Length; i++)
                {
                    query.SetParameter(i, _paras[i]);
                }
            }
            return query.List();
        }

        public Type RootType
        {
            get { return typeof(T); }
        }

        #endregion
    }
}
