using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.BoxManage.Core.Cache;
    using Synacast.ServicesFramework.ConsoleCommand;

    public class ForbiddenAreaCommand : ConsoleCmdBase 
    {
        public override string StartLine
        {
            get { return "ForbiddenArea Start..."; }
        }

        public override string RunningLine
        {
            get { return "ForbiddenArea is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "ForbiddenArea Complete"; }
        }

        public override Func<object[], object> Fun
        {
            get
            {
                return args =>
                {
                    ForbiddenAreaCache.Instance.DatabaseInit();
                    return null;
                };
            }
        }
    }
}
