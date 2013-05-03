using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Relation_Page_Soft")]
    public class PageSoftInfo : DbObject<PageSoftInfo>
    {
        private int _iD;

        [PrimaryKey]
        public int ID
        {
            get { return _iD; }
            set { _iD = value; }
        }

        private int _pageID;

        [Property]
        public int PageID
        {
            get { return _pageID; }
            set { _pageID = value; }
        }

        private int _softID;

        [Property]
        public int SoftID
        {
            get { return _softID; }
            set { _softID = value; }
        }

        private int _cPDOfChannel;

        [Property]
        public int CPDOfChannel
        {
            get { return _cPDOfChannel; }
            set { _cPDOfChannel = value; }
        }

        private string _forceName;

        [Property]
        public string ForceName
        {
            get { return _forceName; }
            set { _forceName = value; }
        }

        private string _forceDescription;

        [Property]
        public string ForceDescription
        {
            get { return _forceDescription; }
            set { _forceDescription = value; }
        }

        private int _rank;

        [Property]
        public int Rank
        {
            get { return _rank; }
            set { _rank = value; }
        }

        private string _softCnName;

        public string SoftCnName
        {
            get { return _softCnName; }
            set { _softCnName = value; }
        }

        private string _softEnName;

        public string SoftEnName
        {
            get { return _softEnName; }
            set { _softEnName = value; }
        }

        private int _softCountPerDay;

        public int SoftCountPerDay
        {
            get { return _softCountPerDay; }
            set { _softCountPerDay = value; }
        }

        private int _softAllCPDOfChannel;

        public int SoftAllCPDOfChannel
        {
            get { return _softAllCPDOfChannel; }
            set { _softAllCPDOfChannel = value; }
        }

        private bool _softBindEnable;

        public bool SoftBindEnable
        {
            get { return _softBindEnable; }
            set { _softBindEnable = value; }
        }

        public static string ViewName
        {
            get { return "View_Relation_Page_Soft"; }
        }

        public static string TableName
        {
            get { return "Relation_Page_Soft"; }
        }

        public static string PrimaryKey
        {
            get { return "ID"; }
        }
    }
}
