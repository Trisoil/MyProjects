using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.ServicesFramework.ConsoleCommand
{
    /// <summary>
    /// 基础控制台命令接口
    /// </summary>
    public interface IConsoleCommand
    {
        /// <summary>
        /// 输入命令Key
        /// </summary>
        string Key { get; set; }

        /// <summary>
        /// 命令任务执行提示
        /// </summary>
        string StartLine { get; }

        /// <summary>
        /// 命令任务正在执行提示
        /// </summary>
        string RunningLine { get; }

        /// <summary>
        /// 命令任务执行完成提示
        /// </summary>
        string CompleteLine { get; }

        /// <summary>
        /// 命令任务是否已启动
        /// </summary>
        bool IsStarting { get; set; }

        /// <summary>
        /// 命令任务
        /// </summary>
        Func<object[], object> Fun { get; }

        /// <summary>
        /// 配置Key输入
        /// </summary>
        /// <param name="Key"></param>
        void SetKey(string Key);
    }
}
