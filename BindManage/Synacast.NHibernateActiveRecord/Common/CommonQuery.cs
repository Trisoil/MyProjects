using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace Synacast.NHibernateActiveRecord.Common
{
    using NHibernate;
    using Castle.ActiveRecord;
    using Synacast.NHibernateActiveRecord.Help;

    public class CommonQuery<T>:IActiveRecordQuery where T:ActiveRecordBase 
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

        public CommonQuery(string query, int start, int limit, string sort, string dir, string tableName, string primaryKey, string[][] filters, string otherWhere, params object[] otherParameters)
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

        #region IActiveRecordQuery ≥…‘±

        public object Execute(NHibernate.ISession session)
        {
            CommonDictionary dic = CommonStringHelp.GetFilterSql(_query, _start, _limit, _sort, _dir, _tableName, _primaryKey, _filters, _otherWhere, _otherParameter);
            IQuery query = session.CreateSQLQuery(dic.Sql, typeof(T).FullName.Replace(".",""), typeof(T));
            if (dic.Parameters != null)
            {
                foreach (DictionaryEntry par in dic.Parameters)
                {
                    query.SetParameter(par.Key.ToString(), par.Value);
                }
            }

            return query.List();
        }

        public Type TargetType
        {
            get { return typeof(T); }
        }

        #endregion
    }
}
