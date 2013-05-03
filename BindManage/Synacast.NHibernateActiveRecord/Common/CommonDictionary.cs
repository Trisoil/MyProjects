using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace Synacast.NHibernateActiveRecord.Common
{
    public class CommonDictionary
    {
        private string _sql;
        private Hashtable _table;

        public CommonDictionary()
        {
        }

        public string Sql
        {
            get { return _sql; }
            set { _sql = value; }
        }

        public void AddParameter(string key, object value)
        {
            if (_table == null)
                _table = new Hashtable();
            _table.Add(key, value);
        }
        public Hashtable Parameters
        {
            get { return _table; }
        }
    }
}
