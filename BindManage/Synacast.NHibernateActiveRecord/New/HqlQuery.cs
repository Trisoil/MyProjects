using System;
using System.Collections.Generic;
using System.Text;

namespace Synacast.NHibernateActiveRecord.New
{
    using NHibernate;
    using Castle.ActiveRecord;

    public class HqlQuery<T>:IActiveRecordQuery 
    {
        string _hql;
        object[] _parameters;

        public HqlQuery(string hql, params object[] parameters)
        {
            _hql = hql;
            _parameters = parameters;
        }

        #region IActiveRecordQuery ≥…‘±

        public object Execute(ISession session)
        {
            IQuery q = session.CreateQuery(_hql);

            if (_parameters != null)
                for (int i = 0; i < _parameters.Length; i++)
                    q.SetParameter(i, _parameters[i]);

            return q.List();
        }

        public Type TargetType
        {
            get { return typeof(T); }
        }

        #endregion
    }
}
