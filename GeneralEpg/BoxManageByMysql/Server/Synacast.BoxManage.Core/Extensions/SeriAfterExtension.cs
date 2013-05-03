using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Extensions
{
    using Synacast.ServicesFramework.Extension;

    /// <summary>
    /// 扩展后续任务扩展
    /// </summary>
    public class SeriAfterExtension : IExtension 
    {
        #region IExtension 成员

        public object Execute()
        {
            Console.WriteLine("Loading Complete.");
            return null;
        }

        #endregion
    }
}
