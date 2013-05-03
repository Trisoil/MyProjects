using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.Threading;
using System.IO;

namespace Synacast.ServicesFramework.ConsoleCommand
{
    using Synacast.ServicesFramework.Reflection;

    public class ConsoleCommandFactory : IServiceFactory 
    {
        private static string[] _split = new string[] { " " };
        private Dictionary<string, IConsoleCommand> _cmds = new Dictionary<string, IConsoleCommand>();

        #region IServiceFactory 成员

        public object ServiceInit()
        {
            string configPath = string.Format("{0}/Config/ConsoleCommand.xml", AppDomain.CurrentDomain.BaseDirectory);
            if (!File.Exists(configPath))
                return null;
            var root = XElement.Load(configPath);
            var cmds = from data in root.Elements("Command") select data;
            foreach (var cmd in cmds)
            {
                IConsoleCommand consoleCmd = ReflectionOptimizer.CreateInstanceMethod((string)cmd.Attribute("Type"))() as IConsoleCommand;
                if (consoleCmd != null)
                {
                    string key = (string)cmd.Attribute("Key");
                    consoleCmd.SetKey(key);
                    if (!_cmds.ContainsKey(key))
                        _cmds.Add(key, consoleCmd);
                }
            }
            bool flag = true;
            while (flag)
            {
                var readStr = Console.ReadLine().Split(_split, StringSplitOptions.RemoveEmptyEntries);
                if (readStr.Length > 0)
                {
                    if (_cmds.ContainsKey(readStr[0]))
                    {
                        IConsoleCommand consoleCmd = _cmds[readStr[0]];
                        if (!consoleCmd.IsStarting)
                        {
                            ThreadPool.QueueUserWorkItem(state =>
                            {
                                try
                                {
                                    Console.WriteLine("{0} {1}", DateTime.Now, consoleCmd.StartLine);
                                    consoleCmd.Fun(readStr);
                                    Console.WriteLine("{0} {1}", DateTime.Now, consoleCmd.CompleteLine);
                                }
                                catch (Exception ex)
                                {
                                    Console.WriteLine(ex);
                                }
                                finally
                                {
                                    consoleCmd.IsStarting = false;
                                }
                            });
                            consoleCmd.IsStarting = true;
                        }
                        else if (consoleCmd.IsStarting)
                        {
                            Console.WriteLine(consoleCmd.RunningLine);
                        }
                    }
                    else if (readStr[0] == "exit")
                    {
                        flag = false;
                    }
                    else
                    {
                        Console.WriteLine("Bad Command,Please Input Again!");
                    }
                }
                else
                {
                    Console.WriteLine("Bad Command,Please Input Again!");
                }
            }
            return null;
        }

        #endregion
    }
}
