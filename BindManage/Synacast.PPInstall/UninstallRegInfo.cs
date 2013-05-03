using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Relation_UninstallSoft_Reg")]
    public class UninstallRegInfo:DbObject<UninstallRegInfo>
    {
        private int _id;

        [PrimaryKey]
        public int ID
        {
            get { return _id; }
            set { _id = value; }
        }

        private int _softwareID;

        [Property]
        public int SoftwareID
        {
            get { return _softwareID; }
            set { _softwareID = value; }
        }

        private string _delRootReg;

        [Property]
        public string DelRootReg
        {
            get { return _delRootReg; }
            set { _delRootReg = value; }
        }

        private string _delSubReg;

        [Property]
        public string DelSubReg
        {
            get { return _delSubReg;}
            set { _delSubReg = value; }
        }

        private string _delValue;

        [Property]
        public string DelValue
        {
            get { return _delValue; }
            set { _delValue = value; }
        }

        public static string TableName
        {
            get { return "Relation_UninstallSoft_Reg"; }
        }

        public static string PrimaryKey
        {
            get { return "ID"; }
        }
    }
}
