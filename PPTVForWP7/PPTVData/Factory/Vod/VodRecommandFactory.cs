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

namespace PPTVData.Factory
{
    public class VodRecommandFactory : HttpVodFactoryBase
    {

        protected override string CreateUri(params object[] paras)
        {
            return string.Format(EpgUtils.RecommandListUri, paras[2], '1', paras[1], paras[0], _contentCharNum);
        }
    }
}
