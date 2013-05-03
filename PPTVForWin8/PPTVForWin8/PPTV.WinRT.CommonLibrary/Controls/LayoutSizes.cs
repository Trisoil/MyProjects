using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.Foundation;

namespace PPTV.WinRT.CommonLibrary.Controls
{
    public static class LayoutSizes
    {
        //首页 这个数字必须尽量小，效率才高
        public static Size RecommandPrimaryItem = new Size(5, 2);  
        public static Size RecommandSmallItem = new Size(1, 1); 
        public static Size ChannelTileItem = new Size(0.8, 0.8);

        //频道列表页
        public static Size PortraitChannel = new Size(3, 4);
        public static Size LandscapeChannel = new Size(4, 3);
    }
}
