using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Custom")]
    public class CustomInfo:DbObject<CustomInfo>
    {
        private int _customID;

        [PrimaryKey]
        public int CustomID
        {
            get { return _customID; }
            set { _customID = value; }
        }

        private string _cnName;

        [Property]
        public string CnName
        {
            get { return _cnName; }
            set { _cnName = value; }
        }

        private string _enName;

        [Property]
        public string EnName
        {
            get { return _enName; }
            set { _enName = value; }
        }

        private string _description;

        [Property(Length=512)]
        public string Description
        {
            get { return _description; }
            set { _description = value; }
        }

        private int _productID;

        [Property]
        public int ProductID
        {
            get { return _productID; }
            set { _productID = value; }
        }

        private string _clientPath;

        [Property]
        public string ClientPath
        {
            get { return _clientPath; }
            set { _clientPath = value; }
        }

        private string _curVersion;

        [Property]
        public string CurVersion
        {
            get { return _curVersion; }
            set { _curVersion = value; }
        }

        private string _guideClientPath;

        [Property]
        public string GuideClientPath
        {
            get { return _guideClientPath; }
            set { _guideClientPath = value; }
        }

        private string _guideCurVersion;

        [Property]
        public string GuideCurVersion
        {
            get { return _guideCurVersion; }
            set { _guideCurVersion = value; }
        }

        private string _silentClientPath;

        [Property]
        public string SilentClientPath
        {
            get { return _silentClientPath; }
            set { _silentClientPath = value; }
        }

        private string _silentCurVersion;

        [Property]
        public string SilentCurVersion
        {
            get { return _silentCurVersion; }
            set { _silentCurVersion = value; }
        }

        private string _gSClientPath;

        [Property]
        public string GSClientPath
        {
            get { return _gSClientPath; }
            set { _gSClientPath = value; }
        }

        private string _gSCurVersion;

        [Property]
        public string GSCurVersion
        {
            get { return _gSCurVersion; }
            set { _gSCurVersion = value; }
        }

        private string _vaRidInfo;

        [Property]
        public string VaRidInfo
        {
            get { return _vaRidInfo; }
            set { _vaRidInfo = value; }
        }

        private string _productcnName;

        //[Property]
        public string ProductCnName
        {
            get { return _productcnName; }
            set { _productcnName = value; }
        }

        private string _productenName;

        //[Property]
        public string ProductEnName
        {
            get { return _productenName; }
            set { _productenName = value; }
        }

        //private ProductInfo _product;

        //[BelongsTo(Column="ProductID")]
        //public ProductInfo Product
        //{
        //    get { return _product; }
        //    set { _product = value; }
        //}

        public static string TableName
        {
            get { return "Custom"; }
        }

        public static string PrimaryKey
        {
            get { return "CustomID"; }
        }

        public static string ViewName
        {
            get { return "View_Custom"; }
        }
    }
}
