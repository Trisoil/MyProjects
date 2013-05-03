using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Reflection;

namespace Synacast.NHibernateActiveRecord.New
{
    using Castle.ActiveRecord;
    using NHibernate;
    using Synacast.NHibernateActiveRecord;

    /// <summary>
    /// 支持分页的普通查询
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class SqlQuery<T>:IActiveRecordQuery where T : new()
    {
        private int _start;
        private int _limit;
        private string _sort;
        private string _dir;
        private string _tableName;
        private string _primaryKey;
        private string[][] _filters;
        private string _otherWhere;
        private object[] _otherParameter;
        private string _query;

        public SqlQuery(string query, int start, int limit, string sort, string dir, string tableName, string primaryKey, string[][] filters, string otherWhere, params object[] otherParameters)
        {
            _start = start;
            _limit = limit;
            _sort = sort;
            _dir = dir;
            _tableName = tableName;
            _primaryKey = primaryKey;
            _filters = filters;
            _otherWhere = otherWhere;
            _otherParameter = otherParameters;
            _query = query;
        }

        #region IActiveRecordQuery 成员

        public object Execute(ISession session)
        {
            IDbCommand cmd = session.Connection.CreateCommand();
            cmd.CommandText = ActiveRecordHelp.GetFilterSql(_query,cmd, _start, _limit, _sort, _dir, _tableName, _primaryKey, _filters, _otherWhere, _otherParameter);
            List<T> list = new List<T>();
            try
            {
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
                            if (pro.PropertyType == typeof(string)&&value == DBNull.Value) value = "";
                            //if (pro.PropertyType == typeof(int?) && value == DBNull.Value) value = null;
                            if (pro.PropertyType == typeof(int) && value == DBNull.Value) value = 0;
                            if (value == DBNull.Value) value = null;
                            pro.SetValue(o, value, null);
                        }
                    }
                    list.Add(o);
                }
            }
            catch (Exception ex) { throw ex; }
            return list.ToArray();
        }

        public Type TargetType
        {
            get { return typeof(T); }
        }

        #endregion
    }
}
