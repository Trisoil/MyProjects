using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Collections;

namespace Synacast.NhibernateActiveRecord.Query
{
    using NHibernate;
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord.Help;

    public class SqlQuery<T>:IActiveRecordQuery where T:new()
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

        public System.Collections.IEnumerable Enumerate(ISession session)
        {
            throw new NotImplementedException();
        }

        public object Execute(ISession session)
        {
            CommonDictionary dic = ActiveRecordHelp.GetFilterSql(_query, _start, _limit, _sort, _dir, _tableName, _primaryKey, _filters, _otherWhere, _otherParameter);
            ISQLQuery query = session.CreateSQLQuery(dic.ContentSql).AddEntity(typeof(T));
            if (dic.Parameters != null)
            {
                foreach (DictionaryEntry par in dic.Parameters)
                {
                    query.SetParameter(par.Key.ToString(), par.Value);
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
