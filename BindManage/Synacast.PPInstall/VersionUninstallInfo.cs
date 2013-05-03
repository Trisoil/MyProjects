using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Relation_Version_UninstallSoft")]
    public class VersionUninstallInfo:DbObject<VersionUninstallInfo>
    {
        private int _id;

        [PrimaryKey]
        public int ID
        {
            get { return _id; }
            set { _id = value; }
        }

        private int _versionID;

        [Property]
        public int VersionID
        {
            get { return _versionID; }
            set { _versionID = value; }
        }

        private int _softwareID;

        [Property]
        public int SoftwareID
        {
            get { return _softwareID; }
            set { _softwareID = value; }
        }

        private string _name;

        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }

        private string _uninstallName;

        public string UninstallName
        {
            get { return _uninstallName; }
            set { _uninstallName = value; }
        }

        private string _killExe;

        public string KillExe
        {
            get { return _killExe; }
            set { _killExe = value; }
        }

        private int _forceUninstall;

        public int ForceUninstall
        {
            get { return _forceUninstall; }
            set { _forceUninstall = value; }
        }

        private int _isMust;

        public int IsMust
        {
            get { return _isMust; }
            set { _isMust = value; }
        }

        private string _appPath;

        public string AppPath
        {
            get { return _appPath; }
            set { _appPath = value; }
        }

        public static string ViewName
        {
            get { return "View_Relation_Version_UninstallSoft"; }
        }

        public static string TableName
        {
            get { return "Relation_Version_UninstallSoft"; }
        }

        public static string PrimaryKey
        {
            get { return "ID"; }
        }
    }
}
