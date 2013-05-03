using System;
using System.Collections.Generic;
using System.Text;
using System.Data;

using Castle.ActiveRecord;
using NHibernate;

namespace Synacast.NHibernateActiveRecord
{
    /// <summary>
    /// �û�֧�ַ��ض�ά����Ĳ�ѯͳ����
    /// </summary>
    public class SqlStatisticQuery:IActiveRecordQuery  
    {
        private string _sql;
        public SqlStatisticQuery(string sql)
        {
            _sql = sql;
        }
        #region IActiveRecordQuery ��Ա

        public object Execute(ISession session)
        {
            IDbCommand cmd = session.Connection.CreateCommand();
            cmd.CommandText = _sql;
            List<string[]> list = new List<string[]>();
            IDataReader reader = cmd.ExecuteReader();
            while (reader.Read())
            {
                string[] r = new string[2];
                if (string.IsNullOrEmpty(reader[0].ToString()))
                    r[0] = "��";
                else 
                    r[0] = reader[0].ToString();
                r[1] = reader[1].ToString();
                list.Add(r);
            }
            return list.ToArray();
        }

        public Type TargetType
        {
            get { return typeof(ActiveRecordBase); }
        }

        #endregion
    }
}
