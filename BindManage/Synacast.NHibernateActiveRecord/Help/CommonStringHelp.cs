using System;
using System.Collections.Generic;
using System.Text;

namespace Synacast.NHibernateActiveRecord.Help
{
    using Synacast.NHibernateActiveRecord.Common;

    public class CommonStringHelp
    {

        public static CommonDictionary GetFilterSql(string query, int start, int limit, string sort, string dir, string tableName, string primaryKey, string[][] filters, string otherWhere, params object[] otherParemeters)
        {
            
            CommonDictionary dic = GetFilgerWhere( start, limit, sort, dir, tableName, primaryKey, filters, otherWhere, otherParemeters);
            string where = dic.Sql;
            if (string.IsNullOrEmpty(where))
                where = string.Format("select top {0} {4} from {1} where {2} not in (select top {3} {2} from {1} )", limit, tableName, primaryKey, start, query);
            else
            {
                if (where.StartsWith(" order"))
                    where = string.Format("select top {0} {5} from {1} where {2} not in (select top {3} {2} from {1} {4}) {4}", limit, tableName, primaryKey, start, where, query);
                else
                    where = string.Format("select top {0} {5} from {1} where {2} not in (select top {3} {2} from {1} where {4}) and {4}", limit, tableName, primaryKey, start, where, query);
            }
            dic.Sql = where;
            return dic;
        }

        #region Private

        private static CommonDictionary GetFilgerWhere(int start, int limit, string sort, string dir, string tableName, string primaryKey, string[][] filters, string otherWhere, params object[] otherParemeters)
        {
            CommonDictionary dic = new CommonDictionary();
            StringBuilder builder = new StringBuilder();
            int i = 1;
            foreach (string[] filter in filters)
            {
                if (!string.IsNullOrEmpty(filter[1]))
                {
                    switch (filter[1])
                    {
                        case "eq":
                            builder.Append(string.Format(" and {0}=:{1}", filter[2], i));
                            dic.AddParameter(i.ToString(), filter[3]);
                            i++;
                            break;
                        case "lt":
                            builder.Append(string.Format(" and {0}<=:{1}", filter[2], i));
                            dic.AddParameter(i.ToString(), filter[3]);
                            i++;
                            break;
                        case "gt":
                            builder.Append(string.Format(" and {0}>=:{1}", filter[2], i));
                            dic.AddParameter(i.ToString(), filter[3]);
                            i++;
                            break;
                    }
                }
                else
                {
                    builder.Append(string.Format(" and {0} like @{1}", filter[2], i));
                    dic.AddParameter(i.ToString(), string.Format("%{0}%", filter[3]));
                    i++;
                }
            }
            if (!string.IsNullOrEmpty(otherWhere))
            {
                string[] ws = otherWhere.Trim().Split(new string[] { "?" }, StringSplitOptions.RemoveEmptyEntries);
                for (int j = 0; j < ws.Length; j++)
                {
                    if (j != ws.Length - 1)
                    {
                        builder.Append(string.Format(" {0}:{1}", ws[j], i));
                        dic.AddParameter(i.ToString(), otherParemeters[j]);
                        i++;
                    }
                    else if (j == 0 && otherWhere.Contains("?"))
                    {
                        builder.Append(string.Format(" {0}:{1}", ws[j], i));
                        dic.AddParameter(i.ToString(), otherParemeters[j]);
                        i++;
                    }
                    else
                        builder.Append(string.Format(" {0}", ws[j]));
                }
            }
            if (!string.IsNullOrEmpty(sort))
                builder.Append(string.Format(" order by {0} {2},{1} {2}", sort, primaryKey, dir));
            string where = builder.ToString();
            if (where.StartsWith(" and"))
                where = where.Substring(4);
            if (where.StartsWith("and"))
                where = where.Substring(3);
            dic.Sql = where;
            return dic;

        }
        #endregion
    }
}
