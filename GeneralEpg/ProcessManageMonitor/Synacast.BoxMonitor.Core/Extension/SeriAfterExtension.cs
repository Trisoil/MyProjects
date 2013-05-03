using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.ProcessManageMonitor.Core.Extension
{
    using Synacast.ServicesFramework.Extension;

    public class SeriAfterExtension : IExtension 
    {
        #region IExtension Members

        public object Execute()
        {
            Console.WriteLine("ProcessManageMonitor Loading Complete.");
            Console.WriteLine("This is ProcessManageMonitor Don't Close this windows.");
            return null;
        }

        #endregion
    }
}
