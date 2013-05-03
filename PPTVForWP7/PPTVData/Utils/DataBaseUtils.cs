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

using Microsoft.Phone.Data.Linq;

namespace PPTVData
{
    using PPTVData.Entity;
    using PPTVData.Entity.Social;
    using PPTVData.Entity.Notify;

    public class DataBaseUtils
    {
        /// <summary>
        /// 当前数据库最新版本
        /// </summary>
        public static readonly int Version = 2;

        /// <summary>
        /// 创建数据库并更新版本信息
        /// </summary>
        /// <param name="context"></param>
        public static void CreateUpdate(DataContext context)
        {
            if (!context.DatabaseExists())
            {
                Create(context);
            }
            else
            {
                Update(context);
            }
        }

        /// <summary>
        /// 删除数据库重新创建
        /// </summary>
        /// <param name="context"></param>
        public static void Delete(DataContext context)
        {
            context.DeleteDatabase();
            Create(context);
        }

        /// <summary>
        /// 创建数据库并更新版本
        /// </summary>
        /// <param name="context"></param>
        private static void Create(DataContext context)
        {
            context.CreateDatabase();
            var update = context.CreateDatabaseSchemaUpdater();
            update.DatabaseSchemaVersion = Version;
            update.Execute();
        }

        /// <summary>
        /// 更新数据库架构
        /// </summary>
        /// <param name="context"></param>
        private static void Update(DataContext context)
        {
            ZeroToOneUpdate(context);
            OneToTwoUpdate(context);
        }

        /// <summary>
        /// 0版本到1版本更新
        /// </summary>
        private static void ZeroToOneUpdate(DataContext context)
        {
            var update = context.CreateDatabaseSchemaUpdater();
            var version = update.DatabaseSchemaVersion;
            if (version == 0)
            {
                try
                {
                    update.AddTable<DBMyDownloadInfo>();
                    update.AddTable<SocialDataInfo>();
                    update.AddTable<LocalPlay>();
                    update.AddTable<PursuitInfo>();
                    update.DatabaseSchemaVersion = 1;
                    update.Execute();
                }
                catch
                {
                    var newupdate = context.CreateDatabaseSchemaUpdater();
                    newupdate.DatabaseSchemaVersion = 1;
                    newupdate.Execute();
                }
            }
        }

        /// <summary>
        /// 1版本到2版本更新
        /// </summary>
        /// <param name="context"></param>
        private static void OneToTwoUpdate(DataContext context)
        {
            var update = context.CreateDatabaseSchemaUpdater();
            var version = update.DatabaseSchemaVersion;
            if (version == 1)
            {
                try
                {
                    update.AddColumn<DBWatchHistoryChanneInfo>("ProgrameTitle");
                    update.DatabaseSchemaVersion = 2;
                    update.Execute();
                }
                catch
                {
                    var newupdate = context.CreateDatabaseSchemaUpdater();
                    newupdate.DatabaseSchemaVersion = 2;
                    newupdate.Execute();
                }
            }
        }
    }
}
