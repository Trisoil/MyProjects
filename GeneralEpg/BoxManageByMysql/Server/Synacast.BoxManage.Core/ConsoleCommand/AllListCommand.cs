using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.BoxManage.Core.Job;
    using Synacast.ServicesFramework.ConsoleCommand;

    public class AllListCommand : ConsoleCmdBase 
    {
        public override string StartLine
        {
            get { return "AllList Start..."; }
        }

        public override string RunningLine
        {
            get { return "AllList is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "AllList Complete"; }
        }

        public override Func<object[], object> Fun
        {
            get
            {
                return args =>
                {
                    AllListJob job = new AllListJob();
                    job.Execute(null);
                    return null;
                };
            }
        }
    }
}
