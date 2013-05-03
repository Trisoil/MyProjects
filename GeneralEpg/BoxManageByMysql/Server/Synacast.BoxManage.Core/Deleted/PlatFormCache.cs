//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Xml.Linq;

//namespace Synacast.BoxManage.Core.Cache.Deleted
//{
//    using Synacast.BoxManage.Core.Cache.Custom;
//    using Synacast.BoxManage.Core.Cache.Entity;
//    using Synacast.ServicesFramework.Reflection;

//    public class PlatFormCache : CacheBase<List<AuthNode>>
//    {
//        #region Fields

//        private Dictionary<AuthKey,string> _auth;
//        private List<AuthNode> _customers;
//        private static string[] _splictword = new string[] { "," };

//        public static readonly PlatFormCache Instance = new PlatFormCache();

//        private PlatFormCache()
//        { }

//        #endregion

//        /// <summary>
//        /// 服务启动序列化行为
//        /// </summary>
//        public override void SerializeInit()
//        {
//            RefreshCache();
//        }

//        /// <summary>
//        /// 自刷新行为
//        /// </summary>
//        public override void RefreshCache()
//        {
//            Dictionary<AuthKey, string> authkeys = new Dictionary<AuthKey, string>();
//            List<AuthNode> list = new List<AuthNode>();
//            List<AuthNode> customers = new List<AuthNode>();
//            string path = string.Format("{0}/Config/FlatForm.xml", AppDomain.CurrentDomain.BaseDirectory);
//            var root = XElement.Load(path);
//            var query = from data in root.Elements("FlatForm") select FormateAuth(data, authkeys, customers);
//            foreach (var auths in query)
//            {
//                list.AddRange(auths);
//            }
//            _auth = authkeys;
//            _cache = list;
//            _customers = customers;
//        }

//        /// <summary>
//        /// 配置文件中重载平台行为的Auth特例
//        /// </summary>
//        public Dictionary<AuthKey, string> AuthItems
//        {
//            get { 
//                return _auth;
//            }
//        }

//        /// <summary>
//        /// 数据库中重载平台行为的Auth特例
//        /// </summary>
//        public List<AuthNode> Customers
//        {
//            get {
//                return _customers;
//            }
//        }

//        #region Private

//        /// <summary>
//        /// 读取配置文件FlatForm.xml，分析平台的行为
//        /// </summary>
//        private List<AuthNode> FormateAuth(XElement data, Dictionary<AuthKey, string> authKeys, List<AuthNode> customers)
//        {
//            List<AuthNode> result = new List<AuthNode>();
//            AuthNode commonnode = new AuthNode();
//            commonnode.Licence = null;
//            FormateAuthNode(data, data, commonnode, "Name");
//            FormateAuthNodeBool(data, data, commonnode, "IsVisable");
//            FormateAuthNode(data, data, commonnode, "FormateImage");
//            FormateAuthNode(data, data, commonnode, "FormateName");
//            FormateAuthNode(data, data, commonnode, "ReplaceName");
//            FormateAuthNode(data, data, commonnode, "NotContainsNames");
//            FormateAuthNode(data, data, commonnode, "ContainsNames");
//            FormateAuthNode(data, data, commonnode, "SearchTypeIndex");
//            FormateAuthNodeBool(data, data, commonnode, "TagIsVisable");
//            result.Add(commonnode);
//            if (commonnode.Name == null)
//                FormateCustomer(data, customers, authKeys);
//            if (data.Elements("Auth").Count() > 0)
//            {
//                foreach (XElement auth in data.Elements("Auth"))
//                {
//                    AuthNode node = new AuthNode();
//                    node.Licence = GetAttribute(auth, "AuthKey");
//                    FormateAuthNode(data, auth, node, "Name");
//                    FormateAuthNodeBool(data, auth, node, "IsVisable");
//                    FormateAuthNode(data, auth, node, "FormateImage");
//                    FormateAuthNode(data, auth, node, "FormateName");
//                    FormateAuthNode(data, auth, node, "ReplaceName");
//                    FormateAuthNode(data, auth, node, "NotContainsNames");
//                    FormateAuthNode(data, auth, node, "ContainsNames");
//                    FormateAuthNode(data, auth, node, "SearchTypeIndex");
//                    FormateAuthNodeBool(data, auth, node, "TagIsVisable");
//                    string[] licences = GetAttribute(auth, "Licence").Split(_splictword, StringSplitOptions.RemoveEmptyEntries);
//                    foreach (string licence in licences)
//                    {
//                        authKeys.Add(new AuthKey() { Auth = licence, PlatForm = node.Name }, node.Licence);
//                    }
//                    result.Add(node);
//                }
//            }
//            return result;
//        }

//        /// <summary>
//        /// 读取数据库中的Customer，加载Auth特例
//        /// </summary>
//        private void FormateCustomer(XElement data, List<AuthNode> customers, Dictionary<AuthKey, string> authKeys)
//        {
//            if (CustomerCache.Instance.Items == null)
//                return;
//            foreach (var customer in CustomerCache.Instance.Items)
//            {
//                AuthNode c = new AuthNode();
//                c.Name = null;
//                c.Licence = customer.CustomerAuth;
//                FormateAuthNodeBool(data, data, c, "IsVisable");
//                FormateAuthNode(data, data, c, "FormateImage");
//                FormateAuthNode(data, data, c, "FormateName");
//                FormateAuthNode(data, data, c, "ReplaceName");
//                FormateAuthNode(data, data, c, "NotContainsNames");
//                FormateAuthNode(data, data, c, "ContainsNames");
//                FormateAuthNode(data, data, c, "SearchTypeIndex");
//                FormateAuthNodeBool(data, data, c, "TagIsVisable");
//                customers.Add(c);
//                authKeys.Add(new AuthKey() { PlatForm = null, Auth = customer.CustomerAuth }, customer.CustomerAuth);
//            }
//        }

//        private string GetAttribute(XElement xml, string attName)
//        {
//            return (string)xml.Attribute(attName);
//        }

//        private void FormateAuthNode(XElement parent, XElement auth, AuthNode node, string attName)
//        {
//            string result = GetAttribute(auth, attName);
//            if (result == null)
//                result = GetAttribute(parent, attName);
//            ReflectionOptimizer.SetProperty<AuthNode, string>(node, result, attName);
//        }

//        private void FormateAuthNodeBool(XElement parent, XElement auth, AuthNode node, string attName)
//        {
//            string result = GetAttribute(auth, attName);
//            if (string.IsNullOrEmpty(result))
//                result = GetAttribute(parent, attName);
//            ReflectionOptimizer.SetProperty<AuthNode, bool>(node, bool.Parse(result), attName);
//        }

//        #endregion
//    }
//}
