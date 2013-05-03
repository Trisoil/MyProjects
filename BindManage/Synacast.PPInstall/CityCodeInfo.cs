using System;
using System.Collections.Generic;
using System.Text;

namespace Synacast.PPInstall
{
    using Castle.ActiveRecord;
    using Synacast.NHibernateActiveRecord;

    [ActiveRecord("CityCode")]
    public class CityCodeInfo:DbObject<CityCodeInfo>
    {
        private int _id;

        [PrimaryKey]
        public int ID
        {
            get { return _id; }
            set { _id = value; }
        }

        private string _cityName;

        [Property]
        public string CityName
        {
            get { return _cityName; }
            set { _cityName = value; }
        }

        public static string TableName
        {
            get { return "CityCode"; }
        }

        public static string PrimaryKey
        {
            get { return "ID"; }
        }
    }
}
