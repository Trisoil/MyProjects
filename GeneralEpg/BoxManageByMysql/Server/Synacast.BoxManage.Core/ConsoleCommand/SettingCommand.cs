using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.ServicesFramework.ConsoleCommand;

    public class SettingCommand : ConsoleCmdBase 
    {
        public override string StartLine
        {
            get { return "Setting Start..."; }
        }

        public override string RunningLine
        {
            get { return "Setting is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "Setting Complete"; }
        }

        public override Func<object[], object> Fun
        {
            get
            {
                return args =>
                {
                    AppSettingCache.Instance.RefreshCache();
                    return null;
                };
            }
        }
    }
}
