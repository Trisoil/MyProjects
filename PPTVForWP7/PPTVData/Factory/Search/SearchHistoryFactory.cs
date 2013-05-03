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
using System.Collections.ObjectModel;

namespace PPTVData.Factory
{
    using PPTVData.Entity;

    public class SearchHistoryFactory : ViewModelBase<SearchHistoryFactory, DBHistorySearchInfo>
    {
        protected override void LoadFromDatabase()
        {
            DataInfos = new ObservableCollection<DBHistorySearchInfo>(
                _table.OrderByDescending(o => o.Date));
        }

        /// <summary>
        /// 新增一条搜索历史记录，如果存在则更新
        /// </summary>
        /// <param name="entity"></param>
        public override void AddEntity(DBHistorySearchInfo entity)
        {
            var item = DataInfos.FirstOrDefault(o => o.Key == entity.Key);
            if (item != null)
            {
                DataInfos.Remove(item);
                _table.DeleteOnSubmit(item);
            }
            _table.InsertOnSubmit(entity);
            DataInfos.Insert(0, entity);
            SubmitDatabase();
        }
    }
}
