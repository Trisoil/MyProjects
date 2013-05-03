using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Management.Instrumentation;

namespace Synacast.LuceneNetSearcher.Stat.Instrument
{
    [Serializable]
    [InstrumentationClass(InstrumentationType.Instance)]
    public class InstrumentNode : InstrumentBase 
    {
        public string Key { get; set; }

        public int LastHitCount { get; set; }

        public int SearchCount { get; set; }

        public string PIndex { get; set; }

    }
}
