using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.BoxManage.Core.Job;
    using Synacast.ServicesFramework.ConsoleCommand;

    public class ListExtensionCommand : ConsoleCmdBase
    {
        public override string StartLine
        {
            get { return "ListExtension Start..."; }
        }

        public override string RunningLine
        {
            get { return "ListExtension is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "ListExtension Complete"; }
        }

        public override Func<object[], object> Fun
        {
            get
            {
                return args =>
                {
                    var job = new ListExtensionJob();
                    job.Execute(null);
                    return null;
                };
            }
        }
    }
}
