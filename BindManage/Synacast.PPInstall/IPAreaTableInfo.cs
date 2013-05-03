using System;
using System.Collections.Generic;
using System.Text;

namespace Synacast.PPInstall
{
    using Castle.ActiveRecord;
    using Synacast.NHibernateActiveRecord;

    [ActiveRecord("IPAreaTable")]
    public class IPAreaTableInfo:DbObject<IPAreaTableInfo>
    {
        private int _id;

        [PrimaryKey]
        public int ID
        {
            get { return _id; }
            set { _id = value; }
        }

        private string _country;

        [Property]
        public string Country
        {
            get { return _country; }
            set { _country = value; }
        }

        private string _province;

        [Property]
        public string Province
        {
            get { return _province; }
            set { _province = value; }
        }

        private string _city;

        [Property]
        public string City
        {
            get { return _city; }
            set { _city = value; }
        }


        public static string TableName
        {
            get { return "IPAreaTable"; }
        }

        public static string PrimaryKey
        {
            get { return "ID"; }
        }

    }
}
