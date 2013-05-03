using System;
using System.Collections.Generic;
using System.Text;
using NHibernate;
using NHibernate.SqlTypes;

namespace Synacast.NHibernateActiveRecord.Types
{
    public class DateTimeType : IUserType
    {
        public object DeepCopy(object value)
        {
            return value;
        }

        public bool IsMutable
        {
            get { return false; }
        }

        public object NullSafeGet(System.Data.IDataReader rs, string[] names, object owner)
        {
            DateTime ret = new DateTime();
            string value = NHibernateUtil.String.NullSafeGet(rs, names[0]) as string;

            if (!string.IsNullOrEmpty(value))
            {
                ret = DateTime.Parse(value);
            }

            return ret;
        }

        public void NullSafeSet(System.Data.IDbCommand cmd, object value, int index)
        {
            if(value!=null)
                NHibernateUtil.String.NullSafeSet(cmd, ((DateTime)value).ToString("yyyy-MM-dd HH:mm:ss"), index);
            else
                NHibernateUtil.String.NullSafeSet(cmd,null, index);
        }

        public Type ReturnedType
        {
            get { return typeof(DateTime); }
        }

        public global::NHibernate.SqlTypes.SqlType[] SqlTypes
        {
            get { return new SqlType[] { NHibernateUtil.String.SqlType }; }
        }

      
        bool IUserType.Equals(object x, object y)
        {
            return object.Equals(x, y);
        }
    }
}
