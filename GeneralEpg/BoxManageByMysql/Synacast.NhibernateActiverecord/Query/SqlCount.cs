using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace Synacast.NhibernateActiveRecord.Query
{
    using NHibernate;
    using Castle.ActiveRecord;
    using Synacast.NhibernateActiveRecord.Help;

    public class SqlCount<T>:IActiveRecordQuery 
    {
        private string _tableName;
        private string _otherWhere;
        private string[][] _filters;
        private object[] _otherParameter;

        public SqlCount(string tableName, string otherWhere, string[][] filters, params object[] otherParameter)
        {
            _tableName = tableName;
            _otherWhere = otherWhere;
            _filters = filters;
            _otherParameter = otherParameter;
        }

        #region IActiveRecordQuery 成员

        public System.Collections.IEnumerable Enumerate(ISession session)
        {
            throw new NotImplementedException();
        }

        public object Execute(ISession session)
        {
            CommonDictionary dic = ActiveRecordHelp.GetFilgerCount(_tableName, _filters, _otherWhere, _otherParameter);
            ISQLQuery query = session.CreateSQLQuery(dic.ContentSql);
            if (dic.Parameters != null)
            {
                foreach (DictionaryEntry par in dic.Parameters)
                {
                    query.SetParameter(par.Key.ToString(), par.Value);
                }
            }

            return query.List()[0];
        }

        public Type RootType
        {
            get { return typeof(T); }
        }

        #endregion
    }
}
