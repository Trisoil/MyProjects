using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Synacast.BoxManage.Core.Cache
{
    public class MutexLock
    {
        const int CLOSE = 1;
        const int OPEN = 0;

        int _flag = OPEN;

        public bool TryEnter()
        {
            return Interlocked.Exchange(ref _flag, CLOSE) == OPEN;
        }

        public void Exit()
        {
            Interlocked.Exchange(ref _flag, OPEN);
        }
    }
}
