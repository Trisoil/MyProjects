using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.DataModel
{
    public class ChannelCoverGroup : ObservableCollection<ChannelCoverItem>
    {
        public string GroupName { get; set; }
    }
}
