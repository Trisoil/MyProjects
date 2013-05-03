using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml;

namespace PPTV.WinRT.CommonLibrary.Utils
{
    public static class ScreenUtils
    {
        public static int CreateColNum(int itemHeight, int delta = 200)
        {
            var height = Window.Current.CoreWindow.Bounds.Height - delta;
            var colNum = (int)(height / (itemHeight + 10));
            return colNum > 0 ? colNum : 3;
        }

        public static int CreateRequestNum(int itemWidth, int itemHeight, int delta = 200)
        {
            var colNum = CreateColNum(itemHeight, delta);
            var width = Window.Current.CoreWindow.Bounds.Width;
            if (width == 320)
                width = Window.Current.CoreWindow.Bounds.Height / 9 * 16;
            var requestNmu = ((int)(width / (itemWidth + 10)) * colNum) + colNum * 2;
            requestNmu = requestNmu > 0 ? requestNmu : 45;
            return requestNmu;
        }

        public static int CreateGroupRequestNum(int itemWidth, int itemHeight, int groupCol, int delta = 200)
        {
            var width = Window.Current.CoreWindow.Bounds.Width;
            if (width == 320)
                width = Window.Current.CoreWindow.Bounds.Height / 9 * 16;

            var groupNum = CreateColNum(itemHeight, delta) * groupCol;
            var requestNmu = ((int)(width / (itemWidth * groupCol + 40)) * groupNum) + groupNum;
            requestNmu = requestNmu > 0 ? requestNmu : 45;
            return requestNmu;
        }
    }
}
