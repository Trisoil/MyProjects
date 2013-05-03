using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.BoxManage.Core.Sync;
    using Synacast.ServicesFramework.ConsoleCommand;

    public class NetNodeCommand : ConsoleCmdBase 
    {
        public override string StartLine
        {
            get { return "NetNode Start..."; }
        }

        public override string RunningLine
        {
            get { return "NetNode is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "NetNode Complete"; }
        }

        public override Func<object[], object> Fun
        {
            get
            {
                return args =>
                {
                    NetNodeService.Instance.Init();
                    return null;
                };
            }
        }
    }
}
