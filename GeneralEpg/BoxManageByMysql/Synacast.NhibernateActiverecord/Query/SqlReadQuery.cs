using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.NhibernateActiveRecord.Query
{
    using NHibernate;
    using Castle.ActiveRecord;

    public class SqlReadQuery<T> : IActiveRecordQuery where T : new()
    {
        private string _sql;
        private object[] _paras;

        public SqlReadQuery(string sql, params object[] paras)
        {
            _sql = sql;
            _paras = paras;
        }

        #region IActiveRecordQuery 成员

        public System.Collections.IEnumerable Enumerate(ISession session)
        {
            throw new NotImplementedException();
        }

        public object Execute(ISession session)
        {
            session.FlushMode = FlushMode.Never;
            session.CacheMode = CacheMode.Ignore;

            string cql = string.Format("select count(*) {0}", _sql.Substring(_sql.LastIndexOf("from")));
            var cquery = session.CreateSQLQuery(cql);
            var count = Convert.ToInt32(cquery.UniqueResult());

            ISQLQuery query = session.CreateSQLQuery(_sql).AddEntity(typeof(T));
            query.SetReadOnly(true);
            if (_paras != null)
            {
                for (int i = 0; i < _paras.Length; i++)
                {
                    query.SetParameter(i, _paras[i]);
                }
            }
            var results = new List<T>(count);
            query.List(results);
            return results;
        }

        public Type RootType
        {
            get { return typeof(T); }
        }

        #endregion
    }
}
