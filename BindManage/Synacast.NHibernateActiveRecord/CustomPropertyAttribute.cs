using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;
using Castle.ActiveRecord;

namespace Synacast.NHibernateActiveRecord
{
    [AttributeUsage(AttributeTargets.Property), Serializable]
    public class CustomPropertyAttribute:Castle.ActiveRecord.PropertyAttribute 
    {
        public CustomPropertyAttribute(Type userType) : base() { ColumnType = userType.AssemblyQualifiedName; }

        public CustomPropertyAttribute()
        { 

        }

        public CustomPropertyAttribute(string column)
        {
         
            Column = column;
        }
        public CustomPropertyAttribute(string column, string type):this(column)
        {
            ColumnType = type;

        }

        public override string Column
        {
            get
            {
                return base.Column;
            }
            set
            {
                base.Column = value;
            }
        }
    }


 
}
