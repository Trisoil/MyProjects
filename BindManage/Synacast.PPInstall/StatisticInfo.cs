using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;
using Synacast.NHibernateActiveRecord.Types;

namespace Synacast.PPInstall
{
    [ActiveRecord("Stat")]
    public class StatisticInfo:DbObject<StatisticInfo>
    {
        private int _id;

        [PrimaryKey]
        public int ID
        {
            get { return _id; }
            set { _id = value; }
        }

        private int _productID;

        [Property]
        public int ProductID
        {
            get { return _productID; }
            set { _productID = value; }
        }

        private int _customID;

        [Property]
        public int CustomID
        {
            get { return _customID; }
            set { _customID = value; }
        }

        private int _channelID;

        [Property]
        public int ChannelID
        {
            get { return _channelID; }
            set { _channelID = value; }
        }

        private int _softID;

        [Property]
        public int SoftID
        {
            get { return _softID; }
            set { _softID = value; }
        }

        private DateTime? _statDate;

        [CustomProperty(typeof(DateTime))]
        public DateTime? StatDate
        {
            get { return _statDate; }
            set { _statDate = value; }
        }

        private int _statCount;

        [Property]
        public int StatCount
        {
            get { return _statCount; }
            set { _statCount = value; }
        }

        private int _loadCount;

        [Property]
        public int LoadCount
        {
            get { return _loadCount; }
            set { _loadCount = value; }
        }

        private int _downCount;

        [Property]
        public int DownCount
        {
            get { return _downCount; }
            set { _downCount = value; }
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

        private string _customEnName;

        public string CustomEnName
        {
            get { return _customEnName; }
            set { _customEnName = value; }
        }

        private string _customCnName;

        public string CustomCnName
        {
            get { return _customCnName; }
            set { _customCnName = value; }
        }

        private string _channelEnName;

        public string ChannelEnName
        {
            get { return _channelEnName; }
            set { _channelEnName = value; }
        }

        private string _channelCnName;

        public string ChannelCnName
        {
            get { return _channelCnName; }
            set { _channelCnName = value; }
        }

        private string _softCnName;

        public string SoftCnName
        {
            get { return _softCnName; }
            set { _softCnName = value; }
        }

        private string _sofgEnName;

        public string SofgEnName
        {
            get { return _sofgEnName; }
            set { _sofgEnName = value; }
        }

        public static string ViewName
        {
            get { return "View_Stat"; }
        }

        public static string TableName
        {
            get { return "Stat"; }
        }

        public static string PrimaryKey
        {
            get { return "ID"; }
        }
    }
}
