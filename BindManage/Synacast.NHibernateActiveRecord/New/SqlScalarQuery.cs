using System;
using System.Collections.Generic;
using System.Text;
using System.Data;

namespace Synacast.NHibernateActiveRecord.New
{
    using Castle.ActiveRecord;
    using NHibernate;

    public class SqlScalarQuery<T> : IActiveRecordQuery
    {

        private string _sql;
        private object[] _pars;

        public SqlScalarQuery(string sql, params object[] parameters)
        {
            _sql = sql;
            _pars = parameters;
        }

        #region IActiveRecordQuery ≥…‘±

        public object Execute(ISession session)
        {
            IDbCommand cmd = session.Connection.CreateCommand();
            cmd.CommandText = _sql;
            if (_pars != null)
            {
                for (int j = 0; j < _pars.Length; j++)
                {
                    IDbDataParameter p = cmd.CreateParameter();
                    p.ParameterName = "@" + j;
                    p.Value = _pars[j];
                    cmd.Parameters.Add(p);
                }
            }
            return cmd.ExecuteScalar();
        }

        public Type TargetType
        {
            get { return typeof(T); }
        }

        #endregion
    }
}
