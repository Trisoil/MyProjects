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
using System.Linq;

namespace PPTVData.Factory
{
    using PPTVData.Entity;

    public class PlayHistoryFactory : ViewModelBase<PlayHistoryFactory,DBWatchHistoryChanneInfo>
    {
        /// <summary>
        /// 从本地数据库加载数据
        /// </summary>
        protected override void LoadFromDatabase()
        {
            var query = from watch in _table
                        orderby watch.Time descending
                        select watch;
            DataInfos = new System.Collections.ObjectModel.ObservableCollection<DBWatchHistoryChanneInfo>(query);
        }

        /// <summary>
        /// 根据频道ID返回观看历史记录
        /// </summary>
        /// <param name="channelId"></param>
        /// <returns></returns>
        public DBWatchHistoryChanneInfo GetEntity(int channelId)
        {
            return DataInfos.FirstOrDefault(watch => watch.VID == channelId);
        }

        public void RemoveEntity(int channelId)
        {
            var item = DataInfos.FirstOrDefault(watch => watch.VID == channelId);
            if (item != null)
            {
                RemoveEntity(item);
            }
        }

        /// <summary>
        /// 新增一条记录
        /// </summary>
        /// <param name="entity"></param>
        public override void AddEntity(DBWatchHistoryChanneInfo entity)
        {
            var item = DataInfos.FirstOrDefault(watch => watch.VID == entity.VID);
            if (item != null)
            {
                DataInfos.Remove(item);
                _table.DeleteOnSubmit(item);
            }
            DataInfos.Insert(0, entity);
            _table.InsertOnSubmit(entity);
            SubmitDatabase();
        }
    }
}
