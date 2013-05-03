using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;

using Windows.UI.Xaml.Controls;

namespace PPTV.WinRT.CommonLibrary.ViewModel
{
    public class ChannelSearchViewModel<T,U>
    {
        public ObservableCollection<T> Items { get; set; }

        public ObservableCollection<TextBlock> Navs { get; set; }

        public List<U> HotNames { get; set; }
    }
}
