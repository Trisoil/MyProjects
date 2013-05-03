//using System;
//using System.Net;
//using PPTVData.Entity;
//using System.Linq;
//using System.Collections.ObjectModel;
//using System.Collections.Generic;

//namespace PPTVData.Factory
//{
//    public class DBWatchHistoryFactory
//    {
//        public PPTVDataContext _dataContext;
//        public DBWatchHistoryFactory()
//        {
//            if (_dataContext == null)
//                _dataContext = new PPTVDataContext();
//        }

//        /// <summary>
//        /// 新增一条观看历史记录
//        /// </summary>
//        /// <param name="historyRecordInfo"></param>
//        /// <returns></returns>
//        public bool InsertRecord(HistoryRecordInfo historyRecordInfo)
//        {
//            try
//            {
//                if (GetChannelRowByVID(historyRecordInfo.VID) != null)//存在 则更新
//                {
//                    UpdateRecord(historyRecordInfo);
//                    return true;
//                }
//                DBWatchHistoryChanneInfo dbWatchHistoryChanneInfo = new DBWatchHistoryChanneInfo(historyRecordInfo.VID, historyRecordInfo.Title, historyRecordInfo.ProgramIndex, historyRecordInfo.CurrentPosition, DateTime.Now, historyRecordInfo.IsOver);
//                _dataContext.Table_DBWatchHistoryChanneInfo.InsertOnSubmit(dbWatchHistoryChanneInfo);
//                if (historyRecordInfo.ProgramIndex != -1)
//                {
//                    DBWatchHistoryProgramInfo dbWatchHistoryProgramInfo = new DBWatchHistoryProgramInfo(historyRecordInfo.VID, historyRecordInfo.ProgramIndex, historyRecordInfo.CurrentPosition, historyRecordInfo.ProgramTitle);
//                    _dataContext.Table_DBWatchHistoryProgramInfo.InsertOnSubmit(dbWatchHistoryProgramInfo);
//                }
//                _dataContext.SubmitChanges();
//                return true;
//            }
//            catch (Exception exception)
//            {
//                string message = exception.Message;
//                return false;
//            }
//        }

//        /// <summary>
//        /// 根据VID删除一条观看历史记录
//        /// </summary>
//        /// <param name="vid"></param>
//        /// <returns></returns>
//        public bool DeleteRecord(int vid)
//        {
//            var query = from toDelete in _dataContext.Table_DBWatchHistoryChanneInfo
//                        where toDelete.VID == vid
//                        select toDelete;
//            var query1 = from toDelete in _dataContext.Table_DBWatchHistoryProgramInfo
//                         where toDelete.VID == vid
//                         select toDelete;
//            if (query.Count<DBWatchHistoryChanneInfo>() == 0)
//                return false;

            
//            _dataContext.Table_DBWatchHistoryChanneInfo.DeleteAllOnSubmit<DBWatchHistoryChanneInfo>(query);            
//            _dataContext.Table_DBWatchHistoryProgramInfo.DeleteAllOnSubmit<DBWatchHistoryProgramInfo>(query1);
//            _dataContext.SubmitChanges();
//            return true;            
//        }

//        /// <summary>
//        /// 更改一条观看历史记录
//        /// </summary>
//        /// <param name="historyRecordInfo"></param>
//        /// <returns></returns>
//        public bool UpdateRecord(HistoryRecordInfo historyRecordInfo)
//        {
//            var query = from toUpdate in _dataContext.Table_DBWatchHistoryChanneInfo
//                        where toUpdate.VID == historyRecordInfo.VID
//                        select toUpdate;
//            int i = 0;
//            foreach (var u in query)
//            {
//                u.Position = historyRecordInfo.CurrentPosition;
//                u.ProgramIndex = historyRecordInfo.ProgramIndex;
//                u.Title = historyRecordInfo.Title;
//                u.Time = DateTime.Now;
//                u.IsOver = historyRecordInfo.IsOver;
//                i++;
//            }
//            if (historyRecordInfo.ProgramIndex != -1)
//            {
//                var query1 = from toUpdate in _dataContext.Table_DBWatchHistoryProgramInfo
//                             where toUpdate.VID == historyRecordInfo.VID && toUpdate.ProgramIndex == historyRecordInfo.ProgramIndex
//                             select toUpdate;
//                if (query1.Count() >= 1)
//                {
//                    foreach (var u in query1)
//                    {
//                        u.Position = historyRecordInfo.CurrentPosition;
//                        u.ProgramTitle = historyRecordInfo.ProgramTitle;
//                    }
//                }
//                else
//                {
//                    //添加一条新ProgramIndex记录
//                    DBWatchHistoryProgramInfo dbWatchHistoryProgramInfo = new DBWatchHistoryProgramInfo(historyRecordInfo.VID, historyRecordInfo.ProgramIndex, historyRecordInfo.CurrentPosition, historyRecordInfo.ProgramTitle);
//                    _dataContext.Table_DBWatchHistoryProgramInfo.InsertOnSubmit(dbWatchHistoryProgramInfo);
//                }
//            }
//            _dataContext.SubmitChanges();
//            if (i > 0)
//                return true;
//            else
//                return false;
//        }

//        /// <summary>
//        /// 获取所有观看电影记录
//        /// </summary>
//        /// <returns></returns>
//        public ObservableCollection<DBWatchHistoryChanneInfo> GetAllChannelRows()
//        {
//            Log.LogManager.Ins.Log("开始获取历史观看的影片");

//            var rows = new ObservableCollection<DBWatchHistoryChanneInfo>(
//                _dataContext.Table_DBWatchHistoryChanneInfo.OrderByDescending(w => w.Time));

//            Log.LogManager.Ins.Log("结束获取历史观看的影片");

//            return rows;
//        }

//        /// <summary>
//        /// 根据VID获取所有分集历史观看记录
//        /// </summary>
//        /// <param name="vid"></param>
//        /// <returns></returns>
//        public ObservableCollection<DBWatchHistoryProgramInfo> GetAllProgramRowsByVID(int vid)
//        {
//           return new ObservableCollection<DBWatchHistoryProgramInfo>( 
//               _dataContext.Table_DBWatchHistoryProgramInfo.Where(v => v.VID == vid));
//        }

//        /// <summary>
//        /// 根据VID获取观看电影记录
//        /// </summary>
//        /// <param name="vid"></param>
//        /// <returns></returns>
//        public DBWatchHistoryChanneInfo GetChannelRowByVID(int vid)
//        {
//            return _dataContext.Table_DBWatchHistoryChanneInfo.FirstOrDefault(v => v.VID == vid);
//        }
//    }
//}
