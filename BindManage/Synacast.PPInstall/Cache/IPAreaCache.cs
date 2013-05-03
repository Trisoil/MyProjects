using System;
using System.Collections.Generic;
using System.Text;

namespace Synacast.PPInstall.Cache
{
    public class IPAreaCache
    {
        private static List<IPAreaTableInfo> _list = new List<IPAreaTableInfo>();
        public readonly static IPAreaCache Instance = new IPAreaCache();

        static IPAreaCache()
        {
            _list.AddRange(IPAreaTableInfo.FindAll());
        }

        private IPAreaCache()
        { }

        public void Init()
        { }

        public List<IPAreaTableInfo> Items
        {
            get { return _list; }
        }

    }
}
