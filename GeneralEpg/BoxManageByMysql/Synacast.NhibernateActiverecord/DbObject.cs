using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Collections;

namespace Synacast.NhibernateActiveRecord
{
    using Castle.ActiveRecord;

    [Serializable]
    public class DbObject<T> : ActiveRecordBase where T : ActiveRecordBase, new()
    {
        public static T FindById(object id)
        {
            return FindByPrimaryKey(typeof(T), id) as T;
        }

        public static T FindById(object id, bool thrownotFound)
        {
            return FindByPrimaryKey(typeof(T), id, thrownotFound) as T;
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

        public static void DeleteAll(string columnIdName, int[] ids)
        {
            string where = null;
            foreach (int id in ids)
            {
                where += string.Concat(" ", columnIdName, "=", id, " or");
            }
            if (where.EndsWith(" or"))
                where = where.Substring(0, where.LastIndexOf(" or"));
            DeleteAll(where);
        }

        public static object Execute(IActiveRecordQuery query)
        {
            return ExecuteQuery(query);
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

        public static IList SearchBySQL(string sql, string[] columns)
        {
            IList result = null;
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
