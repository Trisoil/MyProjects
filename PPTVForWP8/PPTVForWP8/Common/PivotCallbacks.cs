using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVForWP8.Common
{
    public class PivotCallbacks
    {
        public Action Init { get; set; }
        public Action OnActivated { get; set; }
        public Action<CancelEventArgs> OnBackKeyPress { get; set; }
    }
}
