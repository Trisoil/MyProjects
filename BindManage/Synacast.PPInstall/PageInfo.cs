using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Page")]
    public class PageInfo:DbObject<PageInfo>
    {
        private int _pageID;

        [PrimaryKey]
        public int PageID
        {
            get { return _pageID; }
            set { _pageID = value; }
        }

        private int _softCount;

        [Property]
        public int SoftCount
        {
            get { return _softCount; }
            set { _softCount = value; }
        }

        private string _description;

        [Property]
        public string Description
        {
            get { return _description; }
            set { _description = value; }
        }

        private int _templateID;

        [Property]
        public int TemplateID
        {
            get { return _templateID; }
            set { _templateID = value; }
        }

        private int _bindID;

        [Property]
        public int BindID
        {
            get { return _bindID; }
            set { _bindID = value; }
        }

        private string _templateName;

        public string TemplateName
        {
            get { return _templateName; }
            set { _templateName = value; }
        }

        private int _customID;

        public int CustomID
        {
            get { return _customID; }
            set { _customID = value; }
        }

        private int _channelID;

        public int ChannelID
        {
            get { return _channelID; }
            set { _channelID = value; }
        }

        private string _channelCnName;

        public string ChannelCnName
        {
            get { return _channelCnName; }
            set { _channelCnName = value; }
        }

        private string _channelEnName;

        public string ChannelEnName
        {
            get { return _channelEnName; }
            set { _channelEnName = value; }
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

        private int _productID;

        public int ProductID
        {
            get { return _productID; }
            set { _productID = value; }
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

        private int _templateType;

        public int TemplateType
        {
            get { return _templateType; }
            set { _templateType = value; }
        }

        private int _templateSoftCount;

        public int TemplateSoftCount
        {
            get { return _templateSoftCount; }
            set { _templateSoftCount = value; }
        }

        public static string TableName
        {
            get { return "Page"; }
        }

        public static string ViewName
        {
            get { return "View_Page"; }
        }

        public static string PrimaryKey
        {
            get { return "PageID"; }
        }
    }
}
