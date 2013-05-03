using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace Synacast.NhibernateActiveRecord
{
    public class CommonDictionary
    {
        private string _contentsql;
        private Hashtable _table;

        public CommonDictionary()
        {
        }

        public string ContentSql
        {
            get { return _contentsql; }
            set { _contentsql = value; }
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
