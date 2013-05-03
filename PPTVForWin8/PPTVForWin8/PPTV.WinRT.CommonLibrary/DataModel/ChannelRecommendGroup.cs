using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.DataModel
{
    public class ChannelRecommendGroup : GroupBase<ChannelRecommendListItem>
    {
        public int RecommendIndex { get; set; }
    }
}
