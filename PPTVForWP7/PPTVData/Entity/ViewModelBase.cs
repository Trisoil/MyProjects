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
using System.Collections.ObjectModel;
using System.Data.Linq;

namespace PPTVData.Entity
{
    using PPTVData.Factory;

    /// <summary>
    /// VideoModel基类
    /// </summary>
    /// <typeparam name="T">子类类型</typeparam>
    /// <typeparam name="M">View实体类</typeparam>
    public abstract class ViewModelBase<T, M> where M : class where T : ViewModelBase<T, M>, new()
    {
        protected Table<M> _table;

        public static readonly T Instance = new T();

        protected ViewModelBase()
        {
            try
            {
                DataContext = new PPTVDataContext();
                _table = DataContext.GetTable<M>();
                LoadFromDatabase();
            }
            catch
            {
                DataBaseUtils.Delete(DataContext);
            }
        }

        /// <summary>
        /// 数据库上下文
        /// </summary>
        public virtual PPTVDataContext DataContext { get; set; }

        /// <summary>
        /// 数据项集合
        /// </summary>
        public virtual ObservableCollection<M> DataInfos { get; set; }

        /// <summary>
        /// 从本地CE数据库加载
        /// </summary>
        protected virtual void LoadFromDatabase()
        {
            DataInfos = new ObservableCollection<M>(_table);
        }

        /// <summary>
        /// 增加一条记录
        /// </summary>
        /// <param name="entity"></param>
        public virtual void AddEntity(M entity)
        {
            DataInfos.Add(entity);
            _table.InsertOnSubmit(entity);
            SubmitDatabase();
        }

        /// <summary>
        /// 删除一条记录
        /// </summary>
        /// <param name="entity"></param>
        public virtual void RemoveEntity(M entity)
        {
            DataInfos.Remove(entity);
            _table.DeleteOnSubmit(entity);
            SubmitDatabase();
        }

        /// <summary>
        /// 清空全部记录
        /// </summary>
        public virtual void RemoveAll()
        {
            _table.DeleteAllOnSubmit(DataInfos);
            DataInfos.Clear();
            SubmitDatabase();
        }

        /// <summary>
        /// 保存至数据库
        /// </summary>
        public virtual void SubmitDatabase()
        {
            DataContext.SubmitChanges();
        }
    }
}
