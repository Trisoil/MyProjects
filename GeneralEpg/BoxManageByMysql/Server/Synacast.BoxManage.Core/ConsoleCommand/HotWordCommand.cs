using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.BoxManage.Core.Job;
    using Synacast.ServicesFramework.ConsoleCommand;

    public class HotWordCommand : ConsoleCmdBase 
    {
        public override string StartLine
        {
            get { return "HotWord Start..."; }
        }

        public override string RunningLine
        {
            get { return "HotWord is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "HotWord Complete"; }
        }

        public override Func<object[], object> Fun
        {
            get
            {
                return args =>
                {
                    HotSearchWordJob job = new HotSearchWordJob();
                    job.Execute(null);
                    return null;
                };
            }
        }
    }
}
