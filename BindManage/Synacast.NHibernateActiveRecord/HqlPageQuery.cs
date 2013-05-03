using System;
using System.Data;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using NHibernate;

namespace Synacast.NHibernateActiveRecord
{
    /// <summary>
    /// 用于分页的NH查询类
    /// </summary>
    public class HqlPageQuery:IActiveRecordQuery 
    {
        private string _hql;
        private object[] _parameters;
        private int _start;
        private int _limit;

        public HqlPageQuery(string hql, int start, int limit,params object[] parameters)
        {
            _hql = hql;
            _start = start;
            _limit = limit;
            _parameters = parameters;
        }

        #region IActiveRecordQuery 成员

        public object Execute(ISession session)
        {
            IQuery query = session.CreateQuery(_hql);
            if (_parameters != null)
            {
                for (int i = 0; i < _parameters.Length; i++)
                    query.SetParameter(i, _parameters[i]);
            }
            return query.SetFirstResult(_start).SetMaxResults(_limit).List();
        }

        public Type TargetType
        {
            get { return typeof(ActiveRecordBase); }
        }

        #endregion
    }
}
