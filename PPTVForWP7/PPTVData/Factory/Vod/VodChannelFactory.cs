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

    public class VodChannelFactory : HttpVodFactoryBase
    {

        protected override string CreateUri(params object[] paras)
        {
            var uri = string.Format(EpgUtils.ListUri, paras[3], paras[2], paras[0], _contentCharNum);
            var tagInfos = paras[1] as List<TagInfo>;
            if (tagInfos != null)
            {
                uri = string.Format("{0}&tag={1}", uri, GenerateTagString(tagInfos));
            }            
            if (paras.Length >= 5 && paras[4] != null)
            {
                uri += "&order=" + paras[4];
            }
            return uri;
        }
    }
}
