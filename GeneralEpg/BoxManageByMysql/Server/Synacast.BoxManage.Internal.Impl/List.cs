using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.ServiceModel;

namespace Synacast.BoxManage.Internal.Impl
{
    using log4net;
    using Synacast.BoxManage.Internal;
    using Synacast.BoxManage.Core;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Job;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Internal;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class List : IList 
    {
        static MutexLock _lock = new MutexLock();
        static MutexLock _cmdlock = new MutexLock();
        static DateTime _lasttime = DateTime.MinValue;
        static ILog _logger = BoxUtils.GetLogger("boxmanage.internalimpl.log");
        //Timer timer;

        #region IList 成员

        public List<int> FindChannelIds(int boxTypeId)
        {
            throw new NotImplementedException();//return ListCache.Instance.Items[new VideoNodeKey(null, boxTypeId, null)].Ver1.ToPerfectList();
        }

        public List<int> FindChannelIds(int boxTypeId, BoxTagTypeEnum dimension, string boxTagName)
        {
            throw new NotImplementedException();
        }

        public void Refresh()
        {
            if (DateTime.Now.Subtract(_lasttime).TotalMinutes < int.Parse(AppSettingCache.Instance["RefreshInterval"]) || !_lock.TryEnter())
            {
                _logger.Info(string.Format("由于时间未到或刷新正在执行，请求刷新未执行，上次请求时间为{0}", _lasttime.ToString("yyyy-MM-dd HH:mm:ss")));
                return;
            }
            try
            {
                ThreadPool.QueueUserWorkItem(state => {
                    _logger.Info("刷新开始！");
                    var job = new ListCacheJob();
                    job.Execute(null);
                    _lasttime = DateTime.Now;
                    _logger.Info("刷新执行成功！");
                });

                //timer = new Timer(TimerCallback, null, int.Parse(AppSettingCache.Instance["RefreshExtensionInterval"]) * 60 * 1000, 0);
                
            }
            catch (Exception ex)
            {
                _logger.Error(ex);
            }
            finally
            {
                _lock.Exit();
            }
        }

        public void Command(string command)
        {
            if (!_cmdlock.TryEnter())
            {
                _logger.Info("当前命令正在执行！");
                return;
            }
            try
            {
                _logger.InfoFormat("开始执行{0}命令", command);
                switch (command)
                {
                    case "platform":
                        PlatFormInfoCache.Instance.DatabaseInit();
                        AuthenticationCache.Instance.DatabaseInit();
                        PlatFormCache.Instance.DatabaseInit();
                        VideoNodeKeyArray.Instance.ForbiddenInit();
                        break;
                }
                _logger.InfoFormat("执行{0}命令结束", command);
            }
            catch (Exception ex)
            {
                _logger.Error(ex);
            }
            finally
            {
                _cmdlock.Exit();
            }
        }

        #endregion

        private void TimerCallback(object state)
        {
            _logger.Info("ListExtensionJob开始！");
            var job = new ListExtensionJob();
            job.Execute(null);
            _logger.Info("ListExtensionJob执行成功！");
        }
    }
}
