//using System;
//using System.Collections;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Synacast.BoxManage.Core.Cache.Internal
//{
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Cache.Custom;
//    using Synacast.NhibernateActiveRecord.Query;

//    internal class BKTagCache : SubCacheBase<BKTagInfo>
//    {
//        public static readonly BKTagCache Instance = new BKTagCache();

//        private BKTagCache()
//        { }

//        public override List<BKTagInfo> RefreshCache()
//        {
//            string hql = string.Format("from BKTagInfo b where b.TagType={0} or b.TagType={1}", (int)TagTypeEnum.Act, (int)TagTypeEnum.Director);
//            IList list = BKTagInfo.Execute(new HqlReadQuery<BKTagInfo>(hql)) as IList;
//            return new List<BKTagInfo>(list.OfType<BKTagInfo>());
//        }
//    }
//}
