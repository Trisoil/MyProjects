using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.BoxManage.Core.Cache;
    using Synacast.ServicesFramework.ConsoleCommand;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Job;
    using Synacast.BoxManage.Core.Cache.Custom;

    public class PlatFormCommand : ConsoleCmdBase 
    {
        public override string StartLine
        {
            get { return "PlatForm Start..."; }
        }

        public override string RunningLine
        {
            get { return "PlatForm is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "PlatForm Complete"; }
        }

        public override Func<object[], object> Fun
        {
            get
            {
                return args =>
                {
                    PlatFormInfoCache.Instance.DatabaseInit();
                    AuthenticationCache.Instance.DatabaseInit();
                    PlatFormCache.Instance.DatabaseInit();
                    VideoNodeKeyArray.Instance.ForbiddenInit();
                    //ListCacheJob job = new ListCacheJob();
                    //job.Execute(null);
                    return null;
                };
            }
        }
    }
}
