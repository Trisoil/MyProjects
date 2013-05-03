using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Factory
{
    /// <summary>
    /// 下载事件传递参数类型
    /// </summary>
    /// <typeparam name="M"></typeparam>
    public class HttpFactoryArgs<M> : EventArgs
        where M : class
    {
        public HttpFactoryArgs()
        {
        }

        public HttpFactoryArgs(string message, M result)
        {
            Message = message;
            Result = result;
        }

        public string Message { get; set; }
        public M Result { get; set; }
    }
}
