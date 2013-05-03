using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.BoxManage.Core.Job;
    using Synacast.ServicesFramework.ConsoleCommand;

    public class RefreshCommand : ConsoleCmdBase  
    {
        public override string StartLine
        {
            get { return "Refresh Start..."; }
        }

        public override string RunningLine
        {
            get { return "Refresh is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "Refresh Complete"; }
        }

        public override Func<object[], object> Fun
        {
            get
            {
                return args =>
                {
                    ListCacheJob job = new ListCacheJob();
                    job.Execute(null);
                    return null;
                };
            }
        }
    }
}
