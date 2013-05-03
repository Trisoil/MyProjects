using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using System.ServiceModel;

namespace Synacast.BoxManage.Core.Sync
{
    using log4net;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Extensions;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Cache;

    [ServiceContract]
    public interface ISyncService
    {
        [OperationContract]
        void Excute(Stream stream);
    }

    [ServiceBehavior]
    public class SyncService:ISyncService
    {
        #region Fields

        static readonly ILog _logger = BoxUtils.GetLogger("quartz.job.monitorlist.log");
        static int i = 0;

        #endregion

        #region ISyncService 成员

        public void Excute(Stream stream)
        {
            i++;
            try
            {
                var path = string.Format("{0}/Config/Buffer/{1}", AppDomain.CurrentDomain.BaseDirectory, AppSettingCache.Instance[i.ToString()]);
                using (var file = new FileStream(path, FileMode.Create))
                {
                    int readed;
                    var buffer = new byte[100];
                    do
                    {
                        readed = stream.Read(buffer, 0, buffer.Length);
                        file.Write(buffer, 0, readed);
                    }
                    while (readed != 0);
                }
                //最后一个文件执行序列化扩展任务刷新内存
                if (i == int.Parse(AppSettingCache.Instance["SyncFileNum"]))
                {
                    i = 0;
                    NetNodeService.Instance.IsRefresh = true;
                    //CustomerCache.Instance.SerializeInit();
                    PlatFormCache.Instance.SerializeInit();

                    //点播
                    if (bool.Parse(AppSettingCache.Instance["VodAct"]))
                    {
                        BoxTypeCache.Instance.SerializeInit();
                        RecommandCache.Instance.SerializeInit();
                        ListCache.Instance.SerializeInit();
                        TagCache.Instance.SerializeInit();
                    }

                    //直播
                    if (bool.Parse(AppSettingCache.Instance["LiveAct"]))
                    {
                        ChannelLiveItemsCache.Instance.SerializeInit();
                        LiveTypeCache.Instance.SerializeInit();
                        LiveListCache.Instance.SerializeInit();
                        LiveTypeDicCache.Instance.SerializeInit();
                        LiveRecommandCache.Instance.SerializeInit();
                    }
                }
            }
            catch (Exception ex)
            {
                _logger.Error(ex);
            }
        }

        #endregion
    }
}
