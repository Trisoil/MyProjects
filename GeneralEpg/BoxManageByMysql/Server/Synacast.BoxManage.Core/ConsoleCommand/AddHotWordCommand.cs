using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.ConsoleCommand
{
    using Synacast.LuceneNetSearcher.Stat;
    using Synacast.ServicesFramework.ConsoleCommand;

    public class AddHotWordCommand : ConsoleCmdBase 
    {
        public override string StartLine
        {
            get { return "AddHotWord Start..."; }
        }

        public override string RunningLine
        {
            get { return "AddHotWord is Starting..."; }
        }

        public override string CompleteLine
        {
            get { return "AddHotWord Complete"; ; }
        }

        public override Func<object[], object> Fun
        {
            get {
                return args =>
                {
                    if (args.Length < 4)
                    {
                        Console.WriteLine("Parameters Error");
                    }
                    else
                    {
                        var result = LuceneDictionary.Instance.AddNode(args[1].ToString(), 500, int.Parse(args[2].ToString()), args[3].ToString());
                        if (!result)
                        {
                            Console.WriteLine("Excute Error,Please Try Again.");
                        }
                    }
                    return null;
                };
            }
        }
    }
}
