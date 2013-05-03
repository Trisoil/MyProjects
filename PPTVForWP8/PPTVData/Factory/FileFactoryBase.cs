using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;

namespace PPTVData.Factory
{
    using PPTVData.Utils;

    /// <summary>
    /// 本地数据管理基类
    /// </summary>
    /// <typeparam name="T">子类类型</typeparam>
    /// <typeparam name="M">子类实体类类型</typeparam>
    public abstract class FileFactoryBase<T, M>
        where T : new()
        where M : class, new()
    {
        /// <summary>
        /// 为提高效率，目前采用单例
        /// </summary>
        public static readonly T Instance = new T();

        protected FileFactoryBase()
        {
        }

        /// <summary>
        /// 初始化
        /// </summary>
        public virtual async Task Init()
        {
            await CheckData();
        }

        /// <summary>
        /// 检查本地数据是否已经加载完成
        /// </summary>
        private async Task CheckData()
        {
            if (DataInfos == null)
            {
                await Refresh();
            }
        }

        /// <summary>
        /// 数据项集合
        /// </summary>
        public virtual ObservableCollection<M> DataInfos { get; set; }

        /// <summary>
        /// 本地文件名
        /// </summary>
        public abstract string FileName { get; }

        /// <summary>
        /// 重新获取数据
        /// </summary>
        public async virtual Task Refresh()
        {
            DataInfos = await LoadFromLocal();
        }

        /// <summary>
        /// 从本地加载数据
        /// </summary>
        /// <returns></returns>
        protected virtual async Task<ObservableCollection<M>> LoadFromLocal()
        {
            return await Task.Run(() => { return DataCommonUtils.GetContent<ObservableCollection<M>>(FileName); });
        }

        /// <summary>
        /// 插入一条记录
        /// </summary>
        /// <param name="record"></param>
        public virtual async void InsertRecord(M record)
        {
            await CheckData();
            DataInfos.Add(record);   
            SubmitChange();
        }

        /// <summary>
        /// 删除一条记录
        /// </summary>
        /// <param name="record"></param>
        public virtual async void DeleteRecord(M record)
        {
            await CheckData();
            DataInfos.Remove(record);
            SubmitChange();
        }

        /// <summary>
        /// 清空所有记录
        /// </summary>
        public virtual async void RemoveAll()
        {
            await CheckData();
            DataInfos.PerformanceClear();
            SubmitChange();
        }

        /// <summary>
        /// 提交修改
        /// </summary>
        public virtual void SubmitChange()
        {
            Task.Run(async () => { await DataCommonUtils.CreatEntity<ObservableCollection<M>>(DataInfos, FileName); });
        }
    }
}
