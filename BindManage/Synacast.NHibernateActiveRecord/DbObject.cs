using System;
using System.Collections;
using System.Data;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;

namespace Synacast.NHibernateActiveRecord
{
    /// <summary>
    /// 实体类基类
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class DbObject<T>:ActiveRecordBase where T :ActiveRecordBase,new()
    {
        public static T FindById(object id)
        {
            return FindByPrimaryKey(typeof(T), id) as T;    
        }

        public static T[] FindAll()
        { 
            return FindAll(typeof(T)) as T[];
        }

        public static void DeleteAll()
        {
            DeleteAll(typeof(T));
        }

        public static void DeleteAll(string where)
        {
            DeleteAll(typeof(T), where);
        }

        public static void DeleteAll(string columnIdName,int[] ids)
        {
            string where = null;
            foreach (int id in ids)
            {
                where += string.Concat(" ", columnIdName,"=", id, " or");
            }
            if (where.EndsWith(" or"))
                where = where.Substring(0, where.LastIndexOf(" or"));
            DeleteAll(where);
        }



        [Obsolete("请使用其另一同名方法")]
        public static T[] FindAllForGrid(ref string hql,List<object> pars, int start, int limit, string sort, string dir, string[][] filters,bool startAnd)
        {
            hql += ActiveRecordHelp.GetFilterHql(sort, dir, filters, pars, startAnd);
            IList list = null;
            list = ExecuteQuery(new HqlPageQuery(hql, start, limit, pars.ToArray())) as IList;
            T[] result = new T[list.Count];
            list.CopyTo(result,0);
            return result;
        }

        /// <summary>
        /// 返回相应条件分页后的对象数组
        /// </summary>
        /// <param name="start">起始记录索引</param>
        /// <param name="limit">每页记录数</param>
        /// <param name="sort">排序列名</param>
        /// <param name="dir">排序方式</param>
        /// <param name="tableName">表名</param>
        /// <param name="primaryKey">主键名</param>
        /// <param name="filters">filter数组，用于过滤数据</param>
        /// <param name="otherWhere">附加条件语句，没有传null，参数用?</param>
        /// <param name="otherParameters">附加条件语句参数值</param>
        /// <returns></returns>
        public static T[] FindAllForGrid(int start, int limit, string sort, string dir, string tableName, string primaryKey, string[][] filters,string otherWhere, params object[] otherParameters)
        {
            return ExecuteQuery(new SqlPageQuery<T>(start, limit, sort, dir, tableName, primaryKey, filters,otherWhere,otherParameters )) as T[];
        }

        //[Obsolete("请使用其另一同名方法")]
        public static int FindGridCount(string hql,params object[] pars)
        {
            if (hql.Contains("order"))
                hql = hql.Substring(0, hql.LastIndexOf("order"));
            return (int)ExecuteQuery(new HqlCountQuery(hql, pars));
        }

        /// <summary>
        /// 返回相应条件的记录总数
        /// </summary>
        /// <param name="start">起始记录索引</param>
        /// <param name="limit">每页记录数</param>
        /// <param name="sort">排序列名</param>
        /// <param name="dir">排序方式</param>
        /// <param name="tableName">表名</param>
        /// <param name="primaryKey">主键名</param>
        /// <param name="filters">filter数组，用于过滤数据</param>
        /// <param name="otherWhere">附加条件语句，没有传null，参数用?</param>
        /// <param name="otherParameters">附加条件语句参数值</param>
        /// <returns></returns>
        public static int FindGridCount(int start, int limit, string sort, string dir, string tableName, string primaryKey, string[][] filters, string otherWhere, params object[] otherParameters)
        {
            return (int)ExecuteQuery(new SqlCountQuery(start, limit, sort, dir, tableName, primaryKey, filters,otherWhere,otherParameters ));
        }

        public static object Execute(IActiveRecordQuery query)
        {
            return ExecuteQuery(query);
        }

        public static IList SearchBySQL(string sql,string[] columns)
        {
            IList result=null;
            IDbCommand cmd = ActiveRecordMediator.GetSessionFactoryHolder().CreateSession(typeof(T)).Connection.CreateCommand();
            cmd.CommandText = sql;
            IDataReader reader = cmd.ExecuteReader();
            try
            {
                ArrayList rows = new ArrayList();
                while (reader.Read())
                {
                    IList row = CreatDataRow(reader, columns);
                    rows.Add(row);
                }
                result = rows;
            }
            finally
            {
                if (reader != null)
                {
                    reader.Close();
                }
            }
            return result;

        }

        /// <summary>
        /// 返回第一行第一列，忽略额外的行与列
        /// </summary>
        /// <param name="sql"></param>
        /// <returns></returns>
        public static object ExecuteScalarBySql(string sql)
        {
            IDbCommand cmd = ActiveRecordMediator.GetSessionFactoryHolder().CreateSession(typeof(T)).Connection.CreateCommand();
            cmd.CommandText = sql;
            return cmd.ExecuteScalar();
        }

        public static object ExecuteBySql(string sql)
        {
            IDbCommand cmd = ActiveRecordMediator.GetSessionFactoryHolder().CreateSession(typeof(T)).Connection.CreateCommand();
            cmd.CommandText = sql;
            return cmd.ExecuteNonQuery();
        }

        private static IList CreatDataRow(IDataReader reader, string[] columns)
        {
            ArrayList row = new ArrayList();
            foreach (string col in columns)
            {
                row.Add(reader[col]);
            }
            return row;
        }
    }
}
