using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Management.Instrumentation;

[assembly:Instrumented(@"root\Synacast\BoxManage\LuceneNet")]
namespace Synacast.LuceneNetSearcher.Stat.Instrument
{
    [System.ComponentModel.RunInstaller(true)]
    public class InstrumentInstall : DefaultManagementProjectInstaller 
    {
    }
}
