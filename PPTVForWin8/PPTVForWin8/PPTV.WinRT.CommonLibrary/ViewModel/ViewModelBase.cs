using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.ViewModel
{
    using PPTV.WinRT.CommonLibrary.DataModel;

    public abstract class ViewModelBase<T>
    {
        public ObservableCollection<T> Groups { get; set; }
    }
}
