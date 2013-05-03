using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.ServicesFramework.ConsoleCommand;
    using Synacast.LuceneNetSearcher.Cache;

    public class LuceneSettingCommand : ConsoleCmdBase 
    {
        public override string StartLine
        {
            get { return "LuceneSetting Start..."; }
        }

        public override string RunningLine
        {
            get { return "LuceneSetting is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "LuceneSetting Complete"; }
        }

        public override Func<object[], object> Fun
        {
            get
            {
                return args =>
                {
                    SettingCache.Instance.Init();
                    return null;
                };
            }
        }
    }
}
