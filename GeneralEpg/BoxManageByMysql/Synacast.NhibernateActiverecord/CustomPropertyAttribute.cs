using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;

namespace Synacast.NhibernateActiveRecord
{
    using Castle.ActiveRecord;

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
        public CustomPropertyAttribute(string column, string type)
            : this(column)
        {
            ColumnType = type;

        }
    }
}
