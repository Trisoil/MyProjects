using System;
using System.Collections.Generic;
using System.Text;

namespace Synacast.PPInstall
{
    using Castle.ActiveRecord;
    using Synacast.NHibernateActiveRecord;

    [ActiveRecord("IPAreaGroup")]
    public class IPAreaGroupInfo:DbObject<IPAreaGroupInfo>
    {
        private int _id;

        [PrimaryKey]
        public int ID
        {
            get { return _id; }
            set { _id = value; }
        }

        private string _areaGroupName;

        [Property]
        public string AreaGroupName
        {
            get { return _areaGroupName; }
            set { _areaGroupName = value; }
        }

        private string _areaIDList;

        [Property]
        public string AreaIDList
        {
            get { return _areaIDList; }
            set { _areaIDList = value; }
        }

        public static string TableName
        {
            get { return "IPAreaGroup"; }
        }

        public static string PrimaryKey
        {
            get { return "ID"; }
        }
    }
}
