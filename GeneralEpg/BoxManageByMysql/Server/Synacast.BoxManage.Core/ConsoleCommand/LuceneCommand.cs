using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.ServicesFramework.ConsoleCommand;
    using Synacast.LuceneNetSearcher;

    public class LuceneCommand : ConsoleCmdBase 
    {
        public override string StartLine
        {
            get { return "LuceneNet Start..."; }
        }

        public override string RunningLine
        {
            get { return "LuceneNet is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "LuceneNet Complete"; }
        }

        public override Func<object[], object> Fun
        {
            get
            {
                return args =>
                {
                    LuceneEngine engine = new LuceneEngine();
                    engine.Init();
                    if (bool.Parse(AppSettingCache.Instance["VodSearchAct"]))
                    {
                        ListCache.Instance.RefreshListIndex();
                    }
                    if (bool.Parse(AppSettingCache.Instance["LiveSearchAct"]))
                    {
                        LiveListCache.Instance.RefreshListIndex();
                    }
                    return null;
                };
            }
        }
    }
}
