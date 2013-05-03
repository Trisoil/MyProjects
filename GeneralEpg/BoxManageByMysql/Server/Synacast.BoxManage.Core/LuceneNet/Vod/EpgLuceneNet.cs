using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.LuceneNet
{
    /// <summary>
    /// 点播LuceneNet索引依赖类
    /// </summary>
    public class EpgLuceneNet : NewLuceneNetBase  
    {
        #region Fields

        public static readonly EpgLuceneNet Instance = new EpgLuceneNet();

        #endregion

        #region Properties

        private EpgLuceneNet()
        { }

        #endregion
    }
}
