//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Synacast.BoxManage.Core.Cache
//{
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.NhibernateActiveRecord.Query;

//    public class CustomerCache : CacheBase<List<CustomerInfo>>
//    {
//        private Dictionary<int, CustomerInfo> _dic;
//        public static readonly CustomerCache Instance = new CustomerCache();

//        private CustomerCache()
//        { }

//        public override void RefreshCache()
//        {
//            string hql = "from CustomerInfo c";
//            var list = CustomerInfo.Execute(new HqlReadQuery<CustomerInfo>(hql)) as IList<CustomerInfo>;
//            _dic = list.ToDictionary(c => c.ID );
//            //_cache = list;
//        }

//        public CustomerInfo this[string platform, int customerId]
//        {
//            get
//            {
//                if (platform != null) return null;
//                CustomerInfo info = null;
//                _dic.TryGetValue(customerId, out info);
//                return info;
//            }
//        }
//    }
//}
