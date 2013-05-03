//using System;
//using System.Net;
//using System.Linq;
//using PPTVData.Entity;
//using System.Collections.ObjectModel;
//using System.Collections;
//using System.Collections.Generic;
//namespace PPTVData.Factory
//{
//    public class DBHistorySearchFactory
//    {
//        public PPTVDataContext _dataContext;
//        public DBHistorySearchFactory()
//        {
//            if (_dataContext == null)
//                _dataContext = new PPTVDataContext();
//        }
//        /// <summary>
//        /// 新增一条历史搜索记录
//        /// </summary>
//        /// <param name="key"></param>
//        /// <param name="date"></param>
//        /// <returns></returns>
//        public bool InsertRecord(string key, DateTime date)
//        {
//            bool find = false;
//            int id = -1;
//            foreach (DBHistorySearchInfo info in GetAllRows())
//            {
//                if (info.Key == key)
//                {
//                    find = true;
//                    id = info.ID;
//                    break;
//                }
//            }
//            if (find)
//            {
//                return UpdateRecord(id, key, date);

//            }
//            try
//            {
//                DBHistorySearchInfo dbHistorySearchInfo = new DBHistorySearchInfo();
//                dbHistorySearchInfo.Date = date;
//                dbHistorySearchInfo.Key = key;

//                _dataContext.Table_DBHistorySearchInfo.InsertOnSubmit(dbHistorySearchInfo);
//                _dataContext.SubmitChanges();
//                return true;
//            }
//            catch
//            {
//                return false;
//            }
//        }

//        /// <summary>
//        /// 删除一条历史搜索记录
//        /// </summary>
//        /// <param name="id"></param>
//        /// <returns></returns>
//        public bool DeleteRecord(int id)
//        {
//            var query = from toDelete in _dataContext.Table_DBHistorySearchInfo
//                        where toDelete.ID == id
//                        select toDelete;

//            if (query.Count<DBHistorySearchInfo>() == 0)
//                return false;
//            _dataContext.Table_DBHistorySearchInfo.DeleteAllOnSubmit<DBHistorySearchInfo>(query);
//            _dataContext.SubmitChanges();
//            return true;            
//        }

//        /// <summary>
//        /// 更改一条历史搜索记录
//        /// </summary>
//        /// <param name="id"></param>
//        /// <param name="key"></param>
//        /// <param name="date"></param>
//        /// <returns></returns>
//        public bool UpdateRecord(int id, string key, DateTime date)
//        {
//            var query = from toUpdate in _dataContext.Table_DBHistorySearchInfo
//                        where toUpdate.ID == id
//                        select toUpdate;
//            int i = 0;
//            foreach (var u in query)
//            {
//                u.Key = key;
//                u.Date = date;
//                i++;
//            }
//            _dataContext.SubmitChanges();
//            if (i > 0)
//                return true;
//            else
//                return false;
//        }

//        /// <summary>
//        /// 获取所有历史搜索记录
//        /// </summary>
//        /// <returns></returns>
//        public ObservableCollection<DBHistorySearchInfo> GetAllRows()
//        {
//            ObservableCollection<DBHistorySearchInfo> dbHistorySearchInfos = new ObservableCollection<DBHistorySearchInfo>();
//            IEnumerator<DBHistorySearchInfo> iEnumerator = _dataContext.Table_DBHistorySearchInfo.GetEnumerator();
//            while (iEnumerator.MoveNext())
//            {
//                dbHistorySearchInfos.Add(iEnumerator.Current);
//            }
//            return Sort(dbHistorySearchInfos);
//        }

//        private ObservableCollection<DBHistorySearchInfo> Sort(ObservableCollection<DBHistorySearchInfo> source)
//        {
//            var sortedList = source.OrderByDescending(c => c.Date);
//            return new ObservableCollection<DBHistorySearchInfo>(sortedList);
//        }

//        /// <summary>
//        /// 根据ID获取某条历史搜索记录
//        /// </summary>
//        /// <param name="id"></param>
//        /// <returns></returns>
//        public DBHistorySearchInfo GetRowByID(int id)
//        {
//            DBHistorySearchInfo dbHistorySearchInfo = new DBHistorySearchInfo();
//            var query = from toSelect in GetAllRows()
//                        where toSelect.ID == id
//                        select toSelect;
//            foreach (var s in query)
//            {
//                dbHistorySearchInfo = s;
//            }
//            return dbHistorySearchInfo;
//        }
//    }
//}
