using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.ServicesFramework.ConsoleCommand
{
    public abstract class ConsoleCmdBase : IConsoleCommand 
    {
        #region IConsoleCommand 成员

        public abstract string StartLine { get; }

        public abstract string RunningLine { get; }

        public abstract string CompleteLine { get; }

        public abstract Func<object[], object> Fun { get; }

        public string Key { get; set; }

        public bool IsStarting { get; set; }

        public virtual void SetKey(string Key)
        {
            this.Key = Key;
        }

        #endregion
    }
}
