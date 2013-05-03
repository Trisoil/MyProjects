using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;

namespace BoxManageHost
{
    using Synacast.ServicesFramework;

    class Program
    {
        [MTAThread]
        static void Main(string[] args)
        {
            try
            {
                Engine engine = new Engine();
                engine.Start();
                Environment.Exit(0);
            }
            catch (Exception ex)
            {
                Console.Write(ex);
            }
        }
    }
}
