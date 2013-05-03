using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Relation_UninstallSoft_Dir")]
    public class UninstallDirInfo:DbObject<UninstallDirInfo>
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

        private string _delDir;

        [Property]
        public string DelDir
        {
            get { return _delDir; }
            set { _delDir = value; }
        }

        private string _delFolder;

        [Property]
        public string DelFolder
        {
            get { return _delFolder; }
            set { _delFolder = value; }
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
            get { return "Relation_UninstallSoft_Dir"; }
        }

        public static string PrimaryKey
        {
            get { return "ID"; }
        }
    }
}
