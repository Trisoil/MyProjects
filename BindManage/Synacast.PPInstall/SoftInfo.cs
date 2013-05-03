using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Soft")]
    public class SoftInfo:DbObject<SoftInfo>
    {
        private int _softID;

        [PrimaryKey]
        public int SoftID
        {
            get { return _softID; }
            set { _softID = value; }
        }

        private int _type;

        [Property]
        public int Type
        {
            get { return _type; }
            set { _type = value; }
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

        private string _cnDescription;

        [Property]
        public string CnDescription
        {
            get { return _cnDescription; }
            set { _cnDescription = value; }
        }

        private string _enDescription;

        [Property]
        public string EnDescription
        {
            get { return _enDescription; }
            set { _enDescription = value; }
        }

        private string _url;

        [Property]
        public string Url
        {
            get { return _url; }
            set { _url = value; }
        }

        private string _imageUrl;

        [Property]
        public string ImageUrl
        {
            get { return _imageUrl; }
            set { _imageUrl = value; }
        }

        private string _parameter;

        [Property]
        public string Parameter
        {
            get { return _parameter; }
            set { _parameter = value; }
        }

        private string _mainPage;

        [Property]
        public string MainPage
        {
            get { return _mainPage; }
            set { _mainPage = value; }
        }

        private string _forcePage;

        [Property]
        public string ForcePage
        {
            get { return _forcePage; }
            set { _forcePage = value; }
        }

        private string _excludePage;

        [Property]
        public string ExcludePage
        {
            get { return _excludePage; }
            set { _excludePage = value; }
        }

        private string _iconDownloadUrl;

        [Property]
        public string IconDownloadUrl
        {
            get { return _iconDownloadUrl; }
            set { _iconDownloadUrl = value; }
        }

        private string _iconLinkUrl;

        [Property]
        public string IconLinkUrl
        {
            get { return _iconLinkUrl; }
            set { _iconLinkUrl = value; }
        }

        private string _iconName;

        [Property]
        public string IconName
        {
            get { return _iconName; }
            set { _iconName = value; }
        }

        private string _desktopName;

        [Property]
        public string DesktopName
        {
            get { return _desktopName; }
            set { _desktopName = value; }
        }

        private string _desktopEnName;

        [Property]
        public string DesktopEnName
        {
            get { return _desktopEnName; }
            set { _desktopEnName = value; }
        }

        private bool _isChecked;

        [Property]
        public bool IsChecked
        {
            get { return _isChecked; }
            set { _isChecked = value; }
        }

        private int _countPerDay;

        [Property]
        public int CountPerDay
        {
            get { return _countPerDay; }
            set { _countPerDay = value; }
        }

        private int _allCPDOfChannel;

        [Property]
        public int AllCPDOfChannel
        {
            get { return _allCPDOfChannel; }
            set { _allCPDOfChannel = value; }
        }

        private bool _bindEnable;

        [Property]
        public bool BindEnable
        {
            get { return _bindEnable; }
            set { _bindEnable = value; }
        }

        private string _blackCityCodes;

        [Property]
        public string BlackCityCodes
        {
            get { return _blackCityCodes; }
            set { _blackCityCodes = value; }
        }

        private string _whiteCityCodes;

        [Property]
        public string WhiteCityCodes
        {
            get { return _whiteCityCodes; }
            set { _whiteCityCodes = value; }
        }

        private int _areaControlType;

        [Property]
        public int AreaControlType
        {
            get { return _areaControlType; }
            set { _areaControlType = value; }
        }

        public static string TableName
        {
            get { return "Soft"; }
        }

        public static string PrimaryKey
        {
            get { return "SoftID"; }
        }
    }
}
