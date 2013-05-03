using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Version")]
    public class VersionInfo:DbObject<VersionInfo>
    {
        private int _versionID;

        [PrimaryKey]
        public int VersionID
        {
            get { return _versionID; }
            set { _versionID = value; }
        }

        private string _endVersion;

        [Property]
        public string EndVersion
        {
            get { return _endVersion; }
            set { _endVersion = value; }
        }

        private string _downloadUrl;

        [Property]
        public string DownloadUrl
        {
            get { return _downloadUrl; }
            set { _downloadUrl = value; }
        }

        private int _softwareCount;

        [Property]
        public int SoftwareCount
        {
            get { return _softwareCount; }
            set { _softwareCount = value; }
        }

        private int _customID;

        [Property]
        public int CustomID
        {
            get { return _customID; }
            set { _customID = value; }
        }

        private string _customCnName;

        public string CustomCnName
        {
            get { return _customCnName; }
            set { _customCnName = value; }
        }

        private string _custionEnName;

        public string CustionEnName
        {
            get { return _custionEnName; }
            set { _custionEnName = value; }
        }

        private int _customProductID;

        public int CustomProductID
        {
            get { return _customProductID; }
            set { _customProductID = value; }
        }

        private string _productEnName;

        public string ProductEnName
        {
            get { return _productEnName; }
            set { _productEnName = value; }
        }

        private string _productCnName;

        public string ProductCnName
        {
            get { return _productCnName; }
            set { _productCnName = value; }
        }

        public static string ViewName
        {
            get { return "View_Version"; }
        }

        public static string TableName
        {
            get { return "Version"; }
        }

        public static string PrimaryKey
        {
            get { return "VersionID"; }
        }
    }
}
