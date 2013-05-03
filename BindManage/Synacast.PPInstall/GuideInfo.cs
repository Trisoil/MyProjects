using System;
using System.Collections.Generic;
using System.Text;

namespace Synacast.PPInstall
{
    using Castle.ActiveRecord;
    using Synacast.NHibernateActiveRecord;

    [ActiveRecord("Guide")]
    public class GuideInfo:DbObject<GuideInfo>
    {
        private int _guideID;

        [PrimaryKey]
        public int GuideID
        {
            get { return _guideID; }
            set { _guideID = value; }
        }

        private int _customID;

        [Property]
        public int CustomID
        {
            get { return _customID; }
            set { _customID = value; }
        }

        private string _description;

        [Property]
        public string Description
        {
            get { return _description; }
            set { _description = value; }
        }

        private int _interval;

        [Property]
        public int Interval
        {
            get { return _interval; }
            set { _interval = value; }
        }

        private string _imageArray;

        [Property]
        public string ImageArray
        {
            get { return _imageArray; }
            set { _imageArray = value; }
        }

        private string _customCnName;

        public string CustomCnName
        {
            get { return _customCnName; }
            set { _customCnName = value; }
        }

        private string _customEnName;

        public string CustomEnName
        {
            get { return _customEnName; }
            set { _customEnName = value; }
        }

        private string _productCnName;

        public string ProductCnName
        {
            get { return _productCnName; }
            set { _productCnName = value; }
        }

        private string _productEnName;

        public string ProductEnName
        {
            get { return _productEnName; }
            set { _productEnName = value; }
        }

        public static string TableName
        {
            get { return "Guide"; }
        }

        public static string ViewName
        {
            get { return "View_Guide"; }
        }

        public static string PrimaryKey
        {
            get { return "GuideID"; }
        }
    }
}
