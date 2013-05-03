using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache.Internal;

    public abstract class CacheBase<T> where T : class, new()
    {
        #region Fields

        protected MutexLock _lock = new MutexLock();
        protected static T _cache = new T();

        #endregion

        #region Properties

        public T Items
        {
            get
            {
                return _cache;
            }
        }

        protected virtual string BufferName { get; private set; }

        #endregion

        #region Override

        public abstract void RefreshCache();

        #endregion

        /// <summary>
        /// 序列化加载初始化
        /// </summary>
        public virtual void SerializeInit()
        {
            _cache = SerizlizerUtils<T>.DeserializeLastCache(BufferName);
        }

        /// <summary>
        /// 数据库更新初始化
        /// </summary>
        public virtual void DatabaseInit()
        {
            BaseRefreshCache(null);
        }

        /// <summary>
        /// 刷新扩展，保存内存数据到本地
        /// </summary>
        public virtual void RefreshExtension()
        {
            SerizlizerUtils<T>.SerializeLastCache(_cache, BufferName);
        }

        public void BaseRefreshCache(T state)
        {
            if (!_lock.TryEnter())
                return;
            try
            {
                if (state == null)
                {
                    var lastcache = _cache;
                    RefreshCache();
                    if (bool.Parse(AppSettingCache.Instance["AbandonZero"]))
                    {
                        var c = Synacast.ServicesFramework.Reflection.ReflectionOptimizer.GetProperty<T, int>(_cache, "Count");
                        if (c == 0)
                        {
                            _cache = lastcache;
                        }
                        else
                        {
                            //if (lastcache != null)
                            //    GC.ReRegisterForFinalize(lastcache);
                            //GC.Collect();
                        }
                    }
                    else
                    {
                        //if (lastcache != null)
                        //    GC.ReRegisterForFinalize(lastcache);
                        //GC.Collect();
                    }
                }
                else
                {
                    _cache = state;
                }
            }
            finally
            {
                _lock.Exit();
            }
        }
    }
}
