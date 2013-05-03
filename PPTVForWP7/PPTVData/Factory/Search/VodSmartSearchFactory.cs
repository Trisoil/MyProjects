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
using System.Collections.Generic;

namespace PPTVData.Factory
{
    using PPTVData.Entity;

    public class VodSmartSearchFactory : HttpVodFactoryBase
    {

        protected override string CreateUri(params object[] paras)
        {
            return string.Format(EpgUtils.SearchSmartUri, paras[2], paras[1], paras[0], _contentCharNum);
        }

    }
}
