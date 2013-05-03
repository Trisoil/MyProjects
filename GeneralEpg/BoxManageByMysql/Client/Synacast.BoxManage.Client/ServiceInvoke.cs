using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Synacast.BoxManage.Client
{
    public class ServiceInvoke<TContract>
    {
        private ServiceInvoke()
        { }

        public static TContract CreateContract(string endpointName)
        {
            ChannelFactory<TContract> factory = new ChannelFactory<TContract>(endpointName);
            return factory.CreateChannel();
        }

        public static TReturn Invoke<TReturn>(TContract proxy, Func<TContract, TReturn> func)
        {
            TReturn returnValue = default(TReturn);
            try
            {
                returnValue = func(proxy);
                (proxy as ICommunicationObject).Close(TimeSpan.FromSeconds(1));
            }
            catch (CommunicationException)
            {
                (proxy as ICommunicationObject).Abort();
                throw;
            }
            catch (TimeoutException)
            {
                (proxy as ICommunicationObject).Abort();
                throw;
            }
            catch (Exception)
            {
                (proxy as ICommunicationObject).Close(TimeSpan.FromSeconds(1));
                throw;
            }
            return returnValue;
        }

        public static void Invoke(TContract proxy, Action<TContract> action)
        {
            try
            {
                action(proxy);
                (proxy as ICommunicationObject).Close(TimeSpan.FromSeconds(1));
            }
            catch (CommunicationException)
            {
                (proxy as ICommunicationObject).Abort();
                throw;
            }
            catch (TimeoutException)
            {
                (proxy as ICommunicationObject).Abort();
                throw;
            }
            catch (Exception)
            {
                (proxy as ICommunicationObject).Close(TimeSpan.FromSeconds(1));
                throw;
            }
        }
    }
}
