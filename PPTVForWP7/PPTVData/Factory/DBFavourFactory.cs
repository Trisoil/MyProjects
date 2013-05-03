using System;
using System.Net;
using PPTVData.Entity;
using System.Linq;
using System.Collections.ObjectModel;
using System.Collections.Generic;
namespace PPTVData.Factory
{
    public class DBFavourFactory
    {
        public PPTVDataContext _dataContext;
        public DBFavourFactory()
        {
            if (_dataContext == null)
                _dataContext = new PPTVDataContext();
        }

        /// <summary>
        /// 新增一条收藏
        /// </summary>
        /// <param name="channelDetailInfo"></param>
        /// <returns></returns>
        public bool InsertRecord(ChannelDetailInfo channelDetailInfo)
        {
            try
            {
                DBFavourInfo dbFavourInfo = GetRowByVID(channelDetailInfo.VID);
                if (dbFavourInfo != null)
                    return false;
                dbFavourInfo = new DBFavourInfo(channelDetailInfo);
                _dataContext.Table_DBFavourInfo.InsertOnSubmit(dbFavourInfo);
                _dataContext.SubmitChanges();
                return true;
            }
            catch (Exception exception)
            {
                string message = exception.Message;
                return false;
            }
        }

        /// <summary>
        /// 删除一条收藏
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        public bool DeleteRecord(int id)
        {
            var query = from toDelete in _dataContext.Table_DBFavourInfo
                        where toDelete.ID == id
                        select toDelete;
            if (query.Count<DBFavourInfo>() == 0)
                return false;
            _dataContext.Table_DBFavourInfo.DeleteAllOnSubmit<DBFavourInfo>(query);
            _dataContext.SubmitChanges();
            return true;            
        }

        /// <summary>
        /// 更改一条收藏
        /// </summary>
        /// <param name="id"></param>
        /// <param name="channelDetailInfo"></param>
        /// <returns></returns>
        public bool UpdateRecord(int id, ChannelDetailInfo channelDetailInfo)
        {
            var query = from toUpdate in _dataContext.Table_DBFavourInfo
                        where toUpdate.ID == id
                        select toUpdate;
            int i = 0;
            foreach (var u in query)
            {
                u.ActorTag = channelDetailInfo.ActTags;
                u.ImageUrl = channelDetailInfo.ImgUrl;
                u.Mark = channelDetailInfo.Mark;
                u.Title = channelDetailInfo.Title;
                u.VID = channelDetailInfo.VID;
                u.Content = channelDetailInfo.Content;
                i++;
            }
            _dataContext.SubmitChanges();
            if (i > 0)
                return true;
            else
                return false;
        }

        /// <summary>
        /// 获取所有的收藏记录
        /// </summary>
        /// <returns></returns>
        public ObservableCollection<DBFavourInfo> GetAllRows()
        {
            ObservableCollection<DBFavourInfo> dbFavourInfos = new ObservableCollection<DBFavourInfo>();
            IEnumerator<DBFavourInfo> iEnumerator = _dataContext.Table_DBFavourInfo.GetEnumerator();
            while (iEnumerator.MoveNext())
            {
                dbFavourInfos.Add(iEnumerator.Current);
            }
            return dbFavourInfos;
        }

        /// <summary>
        /// 根据ID获取某条收藏记录
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        public DBFavourInfo GetRowByID(int id)
        {
            DBFavourInfo dbFavourInfo = new DBFavourInfo();
            var query = from toSelect in GetAllRows()
                        where toSelect.ID == id
                        select toSelect;
            foreach (var s in query)
            {
                dbFavourInfo = s;
            }
            return dbFavourInfo;
        }

        public DBFavourInfo GetRowByVID(int vid)
        {
            DBFavourInfo dbFavourInfo=null;
            var query = from toSelect in GetAllRows()
                        where toSelect.VID==vid
                        select toSelect;
            foreach (var s in query)
            {
                dbFavourInfo = s;
            }
            return dbFavourInfo;
        }
    }
}
