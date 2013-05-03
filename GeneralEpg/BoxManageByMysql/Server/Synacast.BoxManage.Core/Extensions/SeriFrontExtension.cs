using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Extensions
{
    using Synacast.ServicesFramework.Extension;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.LuceneNetSearcher;
    using Synacast.BoxManage.Core.Cache.Internal;

    /// <summary>
    /// 先行任务扩展，序列化加载
    /// </summary>
    public class SeriFrontExtension:IExtension 
    {
        #region IExtension 成员

        public object Execute()
        {
            Console.WriteLine("Services Loading...");

            AppSettingCache.Instance.Init();
            //CustomerCache.Instance.SerializeInit();
            //PlatFormCache.Instance.SerializeInit();

            LuceneEngine engine = new LuceneEngine();
            engine.Init();

            ////点播
            //if (bool.Parse(AppSettingCache.Instance["VodAct"]))
            //{
            //    BoxTypeCache.Instance.SerializeInit();
            //    RecommandCache.Instance.SerializeInit();
            //    ListCache.Instance.SerializeInit();
            //    TagCache.Instance.SerializeInit();
            //}

            ////直播
            //if (bool.Parse(AppSettingCache.Instance["LiveAct"]))
            //{
            //    ChannelLiveItemsCache.Instance.SerializeInit();
            //    LiveTypeCache.Instance.SerializeInit();
            //    LiveListCache.Instance.SerializeInit();
            //    LiveTypeDicCache.Instance.SerializeInit();
            //    LiveRecommandCache.Instance.SerializeInit();
            //}

            //后台接口
            if (bool.Parse(AppSettingCache.Instance["BackJobAct"]))
            {
                YesterdayListCache.Instance.SerializeInit();
            }

            return null;
        }

        #endregion
    }
}
