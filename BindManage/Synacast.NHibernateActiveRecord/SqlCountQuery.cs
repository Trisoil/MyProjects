using System;
using System.Collections.Generic;
using System.Text;
using System.Data;

namespace Synacast.NHibernateActiveRecord
{
    using NHibernate;
    using Castle.ActiveRecord;
    
    /// <summary>
    /// 用户获取记录总数的查询类
    /// </summary>
    public class SqlCountQuery:IActiveRecordQuery 
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

        public SqlCountQuery(int start, int limit, string sort, string dir, string tableName, string primaryKey, string[][] filters, string otherWhere, params object[] otherParameters)
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
        }
        #region IActiveRecordQuery 成员

        public object Execute(ISession session)
        {
            //try
            //{
                IDbCommand cmd = session.Connection.CreateCommand();
                cmd.CommandText = ActiveRecordHelp.GetFilgerCount(cmd, _start, _limit, _sort, _dir, _tableName, _primaryKey, _filters,_otherWhere,_otherParameter );
                return cmd.ExecuteScalar();
            //}
            //catch (Exception ex)
            //{ return 0; }
        }

        public Type TargetType
        {
            get { return typeof(ActiveRecordBase); }
        }

        #endregion
    }
}
