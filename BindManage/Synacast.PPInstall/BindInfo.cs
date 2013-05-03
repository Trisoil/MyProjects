using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Bind")]
    public class BindInfo:DbObject<BindInfo>
    {
        private int _bindID;

        [PrimaryKey]
        public int BindID
        {
            get { return _bindID; }
            set { _bindID = value; }
        }

        private int _pageCount;

        [Property]
        public int PageCount
        {
            get { return _pageCount; }
            set { _pageCount = value; }
        }

        private int _forceCount;

        [Property]
        public int ForceCount
        {
            get { return _forceCount; }
            set { _forceCount = value; }
        }

        private string _popupPage;

        [Property]
        public string PopupPage
        {
            get { return _popupPage; }
            set { _popupPage = value; }
        }

        private int _channelID;

        [Property]
        public int ChannelID
        {
            get { return _channelID; }
            set { _channelID = value; }
        }

        private int _customID;

        [Property]
        public int CustomID
        {
            get { return _customID; }
            set { _customID = value; }
        }

        private string _clientFullPath;

        [Property]
        public string ClientFullPath
        {
            get { return _clientFullPath; }
            set { _clientFullPath = value; }
        }

        private string _guideClientFullPath;

        [Property]
        public string GuideClientFullPath
        {
            get { return _guideClientFullPath; }
            set { _guideClientFullPath = value; }
        }

        private string _silentClientFullPath;

        [Property]
        public string SilentClientFullPath
        {
            get { return _silentClientFullPath; }
            set { _silentClientFullPath = value; }
        }

        private int _isStart;

        [Property]
        public int IsStart
        {
            get { return _isStart; }
            set { _isStart = value; }
        }

        private int _isStartWithWindows;

        [Property]
        public int IsStartWithWindows
        {
            get { return _isStartWithWindows; }
            set { _isStartWithWindows = value; }
        }

        private int _bindMode;

        [Property]
        public int BindMode
        {
            get { return _bindMode; }
            set { _bindMode = value; }
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

        private int _customProductID;

        public int CustomProductID
        {
            get { return _customProductID; }
            set { _customProductID = value; }
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

        private string _channelType;

        public string ChannelType
        {
            get { return _channelType; }
            set { _channelType = value; }
        }

        public static string TableName
        {
            get { return "Bind"; }
        }

        public static string ViewName
        {
            get { return "View_Bind"; }
        }

        public static string PrimaryKey
        {
            get { return "BindID"; }
        }
    }
}
