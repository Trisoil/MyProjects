using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml.Linq;
using System.ServiceModel;
using System.ServiceModel.Description;

namespace Synacast.BoxManage.Core.Sync
{
    using log4net;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Job;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Internal;

    public class NetNodeService
    {
        #region Fields

        static ILog _logger = BoxUtils.GetLogger("boxmanage.syncservice.log");
        static NetNode _node;

        /// <summary>
        /// 该主机节点是否已更新
        /// </summary>
        public bool IsRefresh;

        /// <summary>
        /// 单例对象实例
        /// </summary>
        public static readonly NetNodeService Instance = new NetNodeService();

        private NetNodeService()
        { }

        #endregion

        /// <summary>
        /// 是否有需更新的子节点
        /// </summary>
        public bool HasChildren
        {
            get {
                return _node.Children != null && _node.Children.Count > 0 && !string.IsNullOrEmpty(_node.Children[0].Url);
            }
        }

        /// <summary>
        /// 网络主机初始化，加载自身与所有孩子节点
        /// </summary>
        public void Init()
        {
            try
            {
                _logger.Info("分布式应用初始化开始");
                LoadNetNode();
                ProcessNode(_node);
            }
            catch (Exception ex)
            {
                _logger.Error(ex);
            }
        }

        /// <summary>
        /// 同步所有孩子节点的数据
        /// </summary>
        public void SyncChildrenData()
        {
            if (HasChildren)
            {
                foreach (var n in _node.Children)
                {
                    if (!string.IsNullOrEmpty(n.Url))
                    {
                        int max = int.Parse(AppSettingCache.Instance["SyncFileNum"]);
                        for (int i = 1; i <= max; i++)
                        {
                            var path = string.Format("{0}/Config/Buffer/{1}", AppDomain.CurrentDomain.BaseDirectory, AppSettingCache.Instance[i.ToString()]);
                            using (var file = new FileStream(path, FileMode.Open))
                            {
                                var binding = new NetTcpBinding("SyncBinding");
                                var service = ServiceInvoke<ISyncService>.CreateContract(binding, n.Url);
                                ServiceInvoke<ISyncService>.Invoke(service, proxy => proxy.Excute(file));
                            }
                        }
                        _logger.Info(string.Format("成功同步孩子节点{0}的数据", n.Url));
                    }
                }
            }
        }

        /// <summary>
        /// 刷新主机节点
        /// </summary>
        public void LoadNetNode()
        {
            _node = ParseNode();
        }

        /// <summary>
        /// 根据配置文件序列化当前主机节点对象
        /// </summary>
        /// <returns>当前主机节点</returns>
        private NetNode ParseNode()
        {
            string configPath = string.Format("{0}/Config/NetNode.xml", AppDomain.CurrentDomain.BaseDirectory);
            if (File.Exists(configPath))
            {
                var xml = XElement.Load(configPath);
                var query = from node in xml.Elements("Node") select new NetNode() { Info = new NetInfo() { Url = (string)node.Attribute("url") }, Parent = new NetInfo() { Url = (string)node.Attribute("parenturl") }, Children = node.Element("Children").Elements("Child").Select(n => new NetInfo() { Url = (string)n.Attribute("url") }).ToList() };
                return query.First();
            }
            else
            {
                _logger.Error("没有找到主机节点配置文件@/Config/NetNode.xml");
            }
            return new NetNode();
        }

        /// <summary>
        /// 处理当前主机节点，将当前节点做为服务HOST启动
        /// </summary>
        /// <param name="node">当前主机节点</param>
        private void ProcessNode(NetNode node)
        {
            if (node.Info != null && !string.IsNullOrEmpty(node.Info.Url))
            {
                ServiceHost host = new ServiceHost(typeof(SyncService));
                //if (host.State == CommunicationState.Created)
                //{
                //    _logger.Info(string.Format("主机节点{0}已启动", node.Info.Url));
                //    return;
                //}
                NetTcpBinding binding = new NetTcpBinding("SyncBinding");
                host.AddServiceEndpoint(typeof(ISyncService), binding, node.Info.Url);
                ContractDescription cd = host.Description.Endpoints[0].Contract;
                OperationDescription myOperationDescription = cd.Operations.Find("Excute");
                DataContractSerializerOperationBehavior serializerBehavior = myOperationDescription.Behaviors.Find<DataContractSerializerOperationBehavior>();
                if (serializerBehavior == null)
                {
                    serializerBehavior = new DataContractSerializerOperationBehavior(myOperationDescription);
                    myOperationDescription.Behaviors.Add(serializerBehavior);
                }
                serializerBehavior.MaxItemsInObjectGraph = 1000000000;
                ServiceDebugBehavior behavior = host.Description.Behaviors.Find<ServiceDebugBehavior>();
                if (behavior == null)
                {
                    behavior = new ServiceDebugBehavior();
                    host.Description.Behaviors.Add(behavior);
                }
                behavior.IncludeExceptionDetailInFaults = true;
                host.Opened += (obj, e) => { _logger.Info(string.Format("主机节点{0}已启动", node.Info.Url)); };
                host.Open();
            }
            else
            {
                _logger.Error("没有找到可用于启动的主机节点");
            }
        }
    }
}
