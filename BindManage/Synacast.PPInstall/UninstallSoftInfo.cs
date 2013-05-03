using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("UninstallSoftware")]
    public class UninstallSoftInfo:DbObject<UninstallSoftInfo>
    {
        private int _softwareID;

        [PrimaryKey]
        public int SoftwareID
        {
            get { return _softwareID; }
            set { _softwareID = value; }
        }

        private string _name;

        [Property]
        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }

        private string _uninstallName;

        [Property]
        public string UninstallName
        {
            get { return _uninstallName; }
            set { _uninstallName = value; }
        }

        private string _killExe;

        [Property]
        public string KillExe
        {
            get { return _killExe; }
            set { _killExe = value; }
        }

        private int _forceUninstall;

        [Property]
        public int ForceUninstall
        {
            get { return _forceUninstall; }
            set { _forceUninstall = value; }
        }

        private int _isMust;

        [Property]
        public int IsMust
        {
            get { return _isMust; }
            set { _isMust = value; }
        }

        private string _appPath;

        [Property]
        public string AppPath
        {
            get { return _appPath;}
            set { _appPath = value; }
        }

        public static string TableName
        {
            get { return "UninstallSoftware"; }
        }

        public static string PrimaryKey
        {
            get { return "SoftwareID"; }
        }
    }
}
