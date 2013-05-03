using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.BoxManage.Core.Job;
    using Synacast.ServicesFramework.ConsoleCommand;

    public class UpdateListCommand : ConsoleCmdBase  
    {
        public override string StartLine
        {
            get { return "UpdateList Start..."; }
        }

        public override string RunningLine
        {
            get { return "UpdateList is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "UpdateList Complete"; }
        }

        public override Func<object[], object> Fun
        {
            get
            {
                return args =>
                {
                    UpdateListJob job = new UpdateListJob();
                    job.Execute(null);
                    return null;
                };
            }
        }
    }
}
