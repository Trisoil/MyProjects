using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace PPTVData.Entity
{
    public class UpgradeInfo
    {
        private bool _allow3g;
        private string _url;
        private string _latestversion;
        private string _newversion;
        private string _latestVersionInfo;
        private string _newVersionInfo;

        public UpgradeInfo()
        { }

        public UpgradeInfo(bool allow3g, string url, string latestversion, string newversion, string latestVersionInfo,string newVersionInfo)
        {
            _allow3g = allow3g;
            _url = url;
            _latestversion = latestversion;
            _newversion = newversion;
            _latestVersionInfo = latestVersionInfo;
            _newVersionInfo = newVersionInfo;
        }

        public bool Allow3G
        {
            get
            {
                return _allow3g;
            }
        }
        public string Url
        {
            get
            {
                return _url;
            }
        }
        public string LatestVersion
        {
            get
            {
                return _latestversion;
            }
        }
        public string NewVersion
        {
            get
            {
                return _newversion;
            }
        }
        public string NewVersionInfo
        {
            get
            {
                return _newVersionInfo;
            }
        }
        public string LatestVersionInfo
        {
            get
            {
                return _latestVersionInfo;
            }
        }
    }
}
