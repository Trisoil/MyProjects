using System;
using System.Data;
using System.Collections.Generic;
using System.Text;

using NHibernate;
using Castle.ActiveRecord;

namespace Synacast.NHibernateActiveRecord
{
    /// <summary>
    /// 查询帮助类
    /// </summary>
    public class ActiveRecordHelp
    {
        /// <summary>
        /// 根据EXT filter传入的参数生产HQL语句
        /// </summary>
        /// <param name="sort"></param>
        /// <param name="dir"></param>
        /// <param name="filters"></param>
        /// <param name="parameters"></param>
        /// <param name="startAnd">是否需要在hql语句开始处增加and</param>
        /// <returns></returns>
        public static string GetFilterHql(string sort, string dir, string[][] filters,List<object> parameters,bool startAnd)
        {
            string hql = string.Empty;
            
            foreach (string[] filter in filters)
            {
                if (!hql.ToLower().Contains("where")) hql += " where";
                if (!string.IsNullOrEmpty(filter[1]))
                {
                    switch (filter[1])
                    {
                        case "eq":
                            hql += string.Format(" and {0}=?", filter[2]);
                            parameters.Add(filter[3]);
                            break;
                        case "lt":
                            hql += string.Format(" and {0}<?", filter[2]);
                            parameters.Add(filter[3]);
                            break;
                        case "gt":
                            hql += string.Format(" and {0}>?", filter[2]);
                            parameters.Add(filter[3]);
                            break;
                    }
                }
                else
                {
                    hql += string.Format(" and {0} like ?",filter[2]);
                    parameters.Add(string.Format("%{0}%",filter[3]));
                }
            }

            hql += string.Format(" order by {0} {1}",sort,dir);
            if (!startAnd)
            {
                if (hql.StartsWith(" and"))
                    return hql.Substring(4);
                else if (hql.StartsWith(" where and"))
                    return hql.Replace(" where and", " where");
            }
            return hql;
        }

        /// <summary>
        /// 根据EXT filter等生产sql语句
        /// </summary>
        /// <param name="cmd"></param>
        /// <param name="start"></param>
        /// <param name="limit"></param>
        /// <param name="sort"></param>
        /// <param name="dir"></param>
        /// <param name="tableName"></param>
        /// <param name="primaryKey"></param>
        /// <param name="filters"></param>
        /// <param name="otherWhere"></param>
        /// <param name="otherParemeters"></param>
        /// <returns></returns>
        public static string GetFilterSql(IDbCommand cmd, int start, int limit, string sort, string dir, string tableName, string primaryKey, string[][] filters,string otherWhere,params object[] otherParemeters)
        {
            string where = GetFilgerWhere(cmd, start, limit, sort, dir, tableName, primaryKey, filters,otherWhere,otherParemeters);
            if (string.IsNullOrEmpty(where))
                return string.Format("select top {0} * from {1} where {2} not in (select top {3} {2} from {1} )", limit, tableName, primaryKey, start);
            else
            {
                if (where.StartsWith(" order"))
                    return string.Format("select top {0} * from {1} where {2} not in (select top {3} {2} from {1} {4}) {4}", limit, tableName, primaryKey, start, where);
                else
                    return string.Format("select top {0} * from {1} where {2} not in (select top {3} {2} from {1} where {4}) and {4}", limit, tableName, primaryKey, start, where);
            }
        }

        public static string GetFilterSql(string query,IDbCommand cmd, int start, int limit, string sort, string dir, string tableName, string primaryKey, string[][] filters, string otherWhere, params object[] otherParemeters)
        {
            string where = GetFilgerWhere(cmd, start, limit, sort, dir, tableName, primaryKey, filters, otherWhere, otherParemeters);
            if (string.IsNullOrEmpty(where))
                return string.Format("select top {0} {4} from {1} where {2} not in (select top {3} {2} from {1} )", limit, tableName, primaryKey, start,query);
            else
            {
                if (where.StartsWith(" order"))
                    return string.Format("select top {0} {5} from {1} where {2} not in (select top {3} {2} from {1} {4}) {4}", limit, tableName, primaryKey, start, where,query);
                else
                    return string.Format("select top {0} {5} from {1} where {2} not in (select top {3} {2} from {1} where {4}) and {4}", limit, tableName, primaryKey, start, where,query);
            }
        }

        public static string GetFilterSql(string query,IDbCommand cmd,  string tableName, string primaryKey, string[][] filters, string otherWhere, params object[] otherParemeters)
        {
            string where = GetFilgerWhere(cmd, 0, 0, "", "", tableName, primaryKey, filters, otherWhere, otherParemeters);
            if (string.IsNullOrEmpty(where))
                return string.Format("select {0} from {1} ", query, tableName);
            else
                return string.Format("select {0} from {1} where {2}", query, tableName, where);
        }

        public static string GetFilgerCount(IDbCommand cmd, int start, int limit, string sort, string dir, string tableName, string primaryKey, string[][] filters, string otherWhere, params object[] otherParemeters)
        {
            string where = GetFilgerWhere(cmd, start, limit, sort, dir, tableName, primaryKey, filters, otherWhere, otherParemeters);
            if (where.Contains(" order"))
                where = where.Substring(0, where.LastIndexOf(" order"));
            if (string.IsNullOrEmpty(where))
                return string.Format("select count(*) from {0}", tableName);
            else
                return string.Format("select count(*) from {0} where {1}", tableName, where);
        }

        /// <summary>
        /// 执行SQL语句
        /// </summary>
        /// <param name="type"></param>
        /// <param name="sql"></param>
        /// <param name="parameters"></param>
        /// <returns></returns>
        public static object QueryForSql(Type type, string sql,params object[] parameters)
        {
            IDbCommand cmd = ActiveRecordMediator.GetSessionFactoryHolder().CreateSession(type).Connection.CreateCommand();
            cmd.CommandText = sql;
            if (parameters != null)
            {
                foreach (object p in parameters)
                {
                    cmd.Parameters.Add(p);
                }
            }
            return cmd.ExecuteScalar();
        }

        public static string GetFilgerWhere(IDbCommand cmd, int start, int limit, string sort, string dir, string tableName, string primaryKey, string[][] filters,string otherWhere,params object[] otherParemeters)
        {
            StringBuilder builder = new StringBuilder();
            int i = 1;
            foreach (string[] filter in filters)
            {
                if (!string.IsNullOrEmpty(filter[1]))
                {
                    switch (filter[1])
                    {
                        case "eq":
                            builder.Append(string.Format(" and {0}=@{1}", filter[2], i));
                            cmd.Parameters.Add((CreateDataParameter(cmd, "@" + i, filter[3])));
                            i++;
                            break;
                        case "lt":
                            builder.Append(string.Format(" and {0}<=@{1}", filter[2], i));
                            cmd.Parameters.Add((CreateDataParameter(cmd, "@" + i, filter[3])));
                            i++;
                            break;
                        case "gt":
                            builder.Append(string.Format(" and {0}>=@{1}", filter[2], i));
                            cmd.Parameters.Add((CreateDataParameter(cmd, "@" + i, filter[3])));
                            i++;
                            break;
                    }
                }
                else
                {
                    builder.Append(string.Format(" and {0} like @{1}", filter[2], i));
                    cmd.Parameters.Add((CreateDataParameter(cmd, "@" + i, string.Format("%{0}%", filter[3]))));
                    i++;
                }
            }
            if (!string.IsNullOrEmpty(otherWhere))
            {
                string[] ws = otherWhere.Trim().Split(new string[] { "?" }, StringSplitOptions.RemoveEmptyEntries);
                for(int j=0;j<ws.Length;j++)
                {
                    if (j != ws.Length - 1)
                    {
                        builder.Append(string.Format(" {0}@{1}", ws[j], i));
                        cmd.Parameters.Add((CreateDataParameter(cmd, "@" + i, otherParemeters[j])));
                        i++;
                    }
                    else if (j == 0 && otherWhere.Contains("?"))
                    {
                        builder.Append(string.Format(" {0}@{1}", ws[j], i));
                        cmd.Parameters.Add((CreateDataParameter(cmd, "@" + i, otherParemeters[j])));
                        i++;
                    }
                    else
                        builder.Append(string.Format(" {0}",ws[j]));
                }
            }
            if (!string.IsNullOrEmpty(sort))
                builder.Append(string.Format(" order by {0} {2},{1} {2}", sort, primaryKey, dir));
            string where = builder.ToString();
            if (where.StartsWith(" and"))
                where = where.Substring(4);
            if (where.StartsWith("and"))
                where = where.Substring(3);
            return where;
            
        }

        /// <summary>
        /// 返回Sql传参
        /// </summary>
        /// <param name="cmd"></param>
        /// <param name="parameterName"></param>
        /// <param name="parameterValue"></param>
        /// <returns></returns>
        private static IDbDataParameter CreateDataParameter(IDbCommand cmd, string parameterName, object parameterValue)
        {
            IDbDataParameter par = cmd.CreateParameter();
            par.ParameterName = parameterName;
            par.Value = parameterValue;
            return par;
        }
    }
}
