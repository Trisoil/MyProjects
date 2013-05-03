using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using NHibernate;

namespace Synacast.NHibernateActiveRecord
{
    /// <summary>
    /// NH查询类，返回记录总数
    /// </summary>
    public class HqlCountQuery:IActiveRecordQuery 
    {
        private string _hql;
        private object[] _parameters;

        public HqlCountQuery(string hql, params object[] parameters)
        {
            _hql = hql;
            _parameters = parameters;
        }

        #region IActiveRecordQuery 成员

        public object Execute(NHibernate.ISession session)
        {
            IQuery q = session.CreateQuery(string.Format("select count(*) {0}",this._hql));
            if (this._parameters != null)
            {
                for (int i = 0; i < this._parameters.Length; i++)
                {
                    q.SetParameter(i, this._parameters[i]);
                    
                }
            }
            return q.UniqueResult();
        }

        public Type TargetType
        {
            get { return typeof(ActiveRecordBase); }
        }

        #endregion
    }

}
