using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Synacast.BoxManage.Core.Utils;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    public abstract class SubCacheBase<T>
    {
        private T _cache;
        protected MutexLock _lock = new MutexLock();

        public void Init()
        {
            if (!_lock.TryEnter())
                return;
            try
            {
                _cache = RefreshCache();
            }
            finally
            {
                _lock.Exit();
            }
        }

        public virtual T Items
        {
            get
            {
                return _cache;
            }
        }

        public abstract T RefreshCache();
    }
}
