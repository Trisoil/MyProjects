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

namespace PPTVData.Entity.Notify
{
    using PPTVData.Entity;
    using PPTVData.Factory;

    public class PursuitViewModel : ViewModelBase<PursuitViewModel, PursuitInfo>
    {
        protected override void LoadFromDatabase()
        {
            DataInfos = new ObservableCollection<PursuitInfo>(DataContext.PursuitTable);
        }

        /// <summary>
        /// 根据频道ID删除一个追剧
        /// </summary>
        /// <param name="chanelId"></param>
        public void RemoveByChannelId(int channelId)
        {
            var pursuitInfo = DataInfos.FirstOrDefault(p => p.ChannelId == channelId);
            if (pursuitInfo != null)
            {
                RemoveEntity(pursuitInfo);
            }
        }

        /// <summary>
        /// 根据频道ID删除一个追剧，但不保存至CE数据库
        /// </summary>
        /// <param name="channelId"></param>
        public void RemoveNotSave(int channelId)
        {
            var pursuitInfo = DataInfos.FirstOrDefault(p => p.ChannelId == channelId);
            if (pursuitInfo != null)
            {
                DataInfos.Remove(pursuitInfo);
                DataContext.PursuitTable.DeleteOnSubmit(pursuitInfo);
            }
        }

        /// <summary>
        /// 返回指定频道ID的追剧信息
        /// </summary>
        /// <param name="channelId"></param>
        /// <returns></returns>
        public PursuitInfo GetByChannelId(int channelId)
        {
            return DataInfos.FirstOrDefault(p => p.ChannelId == channelId);
        }

        /// <summary>
        /// 确定是否追了该剧
        /// </summary>
        /// <param name="channelId"></param>
        /// <returns></returns>
        public bool HasByChannelId(int channelId)
        {
            var pursuit = DataInfos.FirstOrDefault(p => p.ChannelId == channelId);
            return pursuit == null ? false : true;
        }

        /// <summary>
        /// 更新指定频道的集数
        /// </summary>
        /// <param name="channelId"></param>
        /// <param name="count"></param>
        public void UpdateCountByChannelId(int channelId, int count)
        {
            var pursuitInfo = DataInfos.FirstOrDefault(p => p.ChannelId == channelId);
            if (pursuitInfo != null)
            {
                pursuitInfo.ChannelCount = count;
                SubmitDatabase();
            }
        }
    }
}
