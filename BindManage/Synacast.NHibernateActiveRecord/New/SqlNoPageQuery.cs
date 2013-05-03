using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Reflection;

namespace Synacast.NHibernateActiveRecord.New
{
    using NHibernate;
    using Castle.ActiveRecord;

    public class SqlNoPageQuery<T>:IActiveRecordQuery where T:ActiveRecordBase ,new()
    {
        //private string _sort;
        //private string _dir;
        private string _tableName;
        private string _primaryKey;
        private string[][] _filters;
        private string _otherWhere;
        private object[] _otherParameter;
        private string _query;

        public SqlNoPageQuery(string query, string tableName, string primaryKey, string[][] filters, string otherWhere, params object[] otherParameters)
        {
            _tableName = tableName;
            _primaryKey = primaryKey;
            _filters = filters;
            _otherWhere = otherWhere;
            _otherParameter = otherParameters;
            _query = query;
        }

        #region IActiveRecordQuery ��Ա

        public object Execute(ISession session)
        {
            IDbCommand cmd = session.Connection.CreateCommand();
            cmd.CommandText = ActiveRecordHelp.GetFilterSql(_query, cmd,  _tableName, _primaryKey, _filters, _otherWhere, _otherParameter);
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
                        //if (pro.PropertyType == typeof(int?) && value == DBNull.Value) value = null;
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
