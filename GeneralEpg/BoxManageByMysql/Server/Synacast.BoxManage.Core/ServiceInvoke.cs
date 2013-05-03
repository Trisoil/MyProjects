using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;

namespace Synacast.BoxManage.Core
{
    using log4net;
    using Synacast.BoxManage.Core.Utils;

    /// <summary>
    /// 客户端调用WCF服务帮助类
    /// </summary>
    public class ServiceInvoke<TContract>
    {
        static ILog _logger = BoxUtils.GetLogger("boxmanage.syncservice.log");

        private ServiceInvoke()
        { }

        /// <summary>
        /// 根据指定Binding创建客户端访问通道
        /// </summary>
        public static TContract CreateContract(Binding binding, string remoteAddress)
        {
            ChannelFactory<TContract> factory = new ChannelFactory<TContract>(binding, remoteAddress);
            ContractDescription cd = factory.Endpoint.Contract;
            OperationDescription myOperationDescription = cd.Operations.Find("Excute");
            DataContractSerializerOperationBehavior serializerBehavior = myOperationDescription.Behaviors.Find<DataContractSerializerOperationBehavior>();
            if (serializerBehavior == null)
            {
                serializerBehavior = new DataContractSerializerOperationBehavior(myOperationDescription);
                myOperationDescription.Behaviors.Add(serializerBehavior);
            }
            serializerBehavior.MaxItemsInObjectGraph = 1000000000;
            return factory.CreateChannel();
        }

        /// <summary>
        /// 客户端执行有返回值的服务
        /// </summary>
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
            catch (InsufficientMemoryException)
            {
                (proxy as ICommunicationObject).Abort();
                throw;
            }
            catch (Exception ex)
            {
                _logger.Error(ex);
                (proxy as ICommunicationObject).Close(TimeSpan.FromSeconds(1));
                throw ex;
            }
            return returnValue;
        }

        /// <summary>
        /// 客户端执行无返回值服务
        /// </summary>
        /// <param name="proxy"></param>
        /// <param name="action"></param>
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
            catch (InsufficientMemoryException)
            {
                (proxy as ICommunicationObject).Abort();
                throw;
            }
            catch (Exception ex)
            {
                _logger.Error(ex);
                (proxy as ICommunicationObject).Close(TimeSpan.FromSeconds(1));
                throw ex;
            }
        }
    }
}
