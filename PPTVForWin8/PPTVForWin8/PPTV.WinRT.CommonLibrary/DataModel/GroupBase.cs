using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.DataModel
{
    public abstract class GroupBase<T> : BindableBase
    {
        private ObservableCollection<T> _itemsGroup;
        public ObservableCollection<T> ItemsGroup
        {
            get { return _itemsGroup; }
            set { this.SetProperty(ref _itemsGroup, value); }
        }

        private string _groupName;
        public string GroupName
        {
            get { return _groupName; }
            set { this.SetProperty(ref _groupName, value); }
        }
    }
}
