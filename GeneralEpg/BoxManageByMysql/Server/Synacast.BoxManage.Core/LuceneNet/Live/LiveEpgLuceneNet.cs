using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.LuceneNet.Live
{
    /// <summary>
    /// 点播LuceneNet索引依赖类
    /// </summary>
    public class LiveEpgLuceneNet : NewLuceneNetBase 
    {
        #region Fields

        public static readonly LiveEpgLuceneNet Instance = new LiveEpgLuceneNet();

        #endregion

        #region Properties

        private LiveEpgLuceneNet()
        { }

        #endregion
    }
}
