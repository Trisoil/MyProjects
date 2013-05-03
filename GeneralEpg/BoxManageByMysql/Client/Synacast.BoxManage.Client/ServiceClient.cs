using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.Client
{
    [Obsolete("Please Use Class ServiceInvoke")]
    public class ServiceClient<T> : IDisposable
    {
        private ChannelFactory<T> _factory;

        public T GetServices(string name)
        {
            _factory = new ChannelFactory<T>(name);
            return _factory.CreateChannel();
        }

        #region IDisposable 成员

        public void Dispose()
        {
            if (_factory != null)
                _factory.Close(TimeSpan.FromSeconds(2));
        }

        #endregion
    }
}
