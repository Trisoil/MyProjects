using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Reflection;

namespace Synacast.NHibernateActiveRecord
{
    using Castle.ActiveRecord;
    using NHibernate;

    /// <summary>
    /// 不分页的普通查询
    /// </summary>
    public class SqlCommonQuery<T>:IActiveRecordQuery where T:new()
    {
        private string _sql;
        private object[] _pars;

        public SqlCommonQuery(string sql, params object[] parameters)
        {
            _sql = sql;
            _pars = parameters;
        }

        #region IActiveRecordQuery 成员

        public object Execute(ISession session)
        {
            IDbCommand cmd = session.Connection.CreateCommand();
            cmd.CommandText = _sql;
            if (_pars != null)
            {
                for(int j=0;j<_pars.Length;j++)
                {
                    IDbDataParameter p = cmd.CreateParameter();
                    p.ParameterName = "@"+j;
                    p.Value = _pars[j];
                    cmd.Parameters.Add(p);
                }
            }
            List<T> list = new List<T>();
            IDataReader r = cmd.ExecuteReader();
            while (r.Read())
            {
                T o = new T();
                for (int i = 0; i < r.FieldCount; i++)
                {
                    object value = r.GetValue(i);
                    PropertyInfo pro = typeof(T).GetProperty(r.GetName(i));
                    if (pro != null)
                    {
                        if (pro.PropertyType == typeof(string) && value == DBNull.Value) value = "";
                        if (pro.PropertyType == typeof(int) && value == DBNull.Value) value = 0;
                        if (value == DBNull.Value) value = null;
                        pro.SetValue(o, value, null);
                    }
                }
                list.Add(o);
            }
            return list.ToArray();
        }

        public Type TargetType
        {
            get { return typeof(T); }
        }

        #endregion
    }
}
