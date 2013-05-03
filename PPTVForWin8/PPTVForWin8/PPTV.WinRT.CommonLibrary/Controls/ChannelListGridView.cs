using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace PPTV.WinRT.CommonLibrary.Controls
{
    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.Factory;

    public class ChannelListGridView : GridView
    {
        private int _rowVal;
        private int _colVal;

        protected override void PrepareContainerForItemOverride(DependencyObject element, object item)
        {
            base.PrepareContainerForItemOverride(element, item);

            var dataItem = item as ChannelListItem;
            if (dataItem != null)
            {
                if (ChannelTypeFactory.Instance.JudgeLandscapeChannel(dataItem.TypeInfo.TypeName))
                {
                    _colVal = (int)LayoutSizes.LandscapeChannel.Width;
                    _rowVal = (int)LayoutSizes.LandscapeChannel.Height;
                }
                else
                {
                    _colVal = (int)LayoutSizes.PortraitChannel.Width;
                    _rowVal = (int)LayoutSizes.PortraitChannel.Height;
                }
            }

            VariableSizedWrapGrid.SetRowSpan(element as UIElement, _rowVal);
            VariableSizedWrapGrid.SetColumnSpan(element as UIElement, _colVal);
        }
    }
}
