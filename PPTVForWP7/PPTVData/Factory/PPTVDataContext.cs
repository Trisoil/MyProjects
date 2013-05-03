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
using System.Data.Linq;
using PPTVData.Entity;

namespace PPTVData.Factory
{
    using PPTVData.Entity.Social;
    using PPTVData.Entity.Notify;

    public class PPTVDataContext : DataContext
    {
        private static string DBConnectionString = "Data Source=isostore:/PPTVData.sdf";
        private static bool _isLoaded = false;
        private static object _lock = new object();

        public Table<DBHistorySearchInfo> Table_DBHistorySearchInfo;
        public Table<DBFavourInfo> Table_DBFavourInfo;
        public Table<DBWatchHistoryChanneInfo> Table_DBWatchHistoryChanneInfo;
        //public Table<DBWatchHistoryProgramInfo> Table_DBWatchHistoryProgramInfo;
        public Table<DBMyDownloadInfo> Table_DBMyDownloadInfo;
        public Table<SocialDataInfo> Table_Social;
        public Table<LocalPlay> Table_LocalPlay;
        public Table<PursuitInfo> PursuitTable;

        public PPTVDataContext()
            : base(DBConnectionString)
        {
            if (!_isLoaded)
            {
                lock (_lock)
                {
                    if (!_isLoaded)
                    {
                        DataBaseUtils.CreateUpdate(this);
                        _isLoaded = true;
                    }
                }
            }
        }
    }
}
