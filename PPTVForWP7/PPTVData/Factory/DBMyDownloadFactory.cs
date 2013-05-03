using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Linq;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using PPTVData.Entity;
using System.Threading;
namespace PPTVData.Factory
{
    public class DBMyDownloadFactory
    {
        public PPTVDataContext _dataContext;
        private ObservableCollection<DBMyDownloadInfo> MyDownInfos;
        public static object syncObj = new object();
        public DBMyDownloadFactory()
        {
            if (_dataContext == null)
                _dataContext = new PPTVDataContext();
            MyDownInfos = GetAllRecords();
        }

        public void InsertRecord(string vid, string titleImgUrl, string title, string fileUrl, string localPathName, long progress, long fileLen, int status, int rate)
        {
            DBMyDownloadInfo dbMyDownInfo = new DBMyDownloadInfo(vid, titleImgUrl, title, fileUrl, localPathName, progress, fileLen, status, rate);

            lock (DBMyDownloadFactory.syncObj)
            {
                _dataContext.Table_DBMyDownloadInfo.InsertOnSubmit(dbMyDownInfo);
                _dataContext.SubmitChanges();
                InsertIntoCollection(vid, titleImgUrl, title, fileUrl, localPathName, progress, fileLen, status, rate);
            }
        }

        public void DeleteRecord(string vid)
        {
            ThreadPool.QueueUserWorkItem((o) =>
            {         
                lock (DBMyDownloadFactory.syncObj)
                {
                    var query = from toDelete in _dataContext.Table_DBMyDownloadInfo
                                where toDelete.VID == vid
                                select toDelete;
                    _dataContext.Table_DBMyDownloadInfo.DeleteAllOnSubmit<DBMyDownloadInfo>(query);            
                    _dataContext.SubmitChanges();
                    DeleteFromCollection(vid);
                }
            });
            


        }
        public bool UpdateRecord(string vid, int channelVid, string titleImgUrl, string title, string fileUrl, string localPathName, long progress, long fileLen, int status, int rate, long position)
        {
            //Not implemented
            return false;
            //lock (this)
            //{
            //    var query = from toUpdate in _dataContext.Table_DBMyDownloadInfo
            //                where toUpdate.VID == vid
            //                select toUpdate;
            //    int i = 0;
            //    foreach (var u in query)
            //    {
            //        u.TitleImgUrl = titleImgUrl;
            //        u.Title = title;
            //        u.FileUrl = fileUrl;
            //        u.LocalPathName = localPathName;
            //        u.Progress = progress;
            //        u.FileLen = fileLen;
            //        u.Status = status;
            //        u.Rate = rate;
            //        u.ChannelVid = channelVid;
            //        i++;
            //    }
            //    _dataContext.SubmitChanges();
            //    if (i > 0)
            //    {
            //        UpdateCollection(vid, titleImgUrl, title, fileUrl, localPathName, progress, fileLen, status, rate);
            //        return true;
            //    }
            //    else
            //        return false;
            //}
        }
        public void UpdateFileNameRecord(string vid, string fileName)
        {
            ThreadPool.QueueUserWorkItem((o) =>
            {
                lock (DBMyDownloadFactory.syncObj)
                {
                    var query = from toUpdate in _dataContext.Table_DBMyDownloadInfo
                                where toUpdate.VID == vid
                                select toUpdate;
                    int i = 0;
                    foreach (var u in query)
                    {
                        u.LocalPathName = fileName;
                        i++;
                    }
                    _dataContext.SubmitChanges();
                    if (i > 0)
                    {
                        UpdateFileNameColl(vid, fileName);
                    }
                }
            });
            
        }
        public void UpdateFileLenRecord(string vid, long fileLen)
        {
            ThreadPool.QueueUserWorkItem((o) =>
            {
                lock (DBMyDownloadFactory.syncObj)
                {
                    int i = 0;
                    var query = from toUpdate in _dataContext.Table_DBMyDownloadInfo
                                where toUpdate.VID == vid
                                select toUpdate;
                    foreach (var u in query)
                    {
                        u.FileLen = fileLen;
                        i++;
                    }
                    _dataContext.SubmitChanges();
                    if (i > 0)
                    {
                        UpdateFileLenColl(vid, fileLen);
                    }
                }
            });
        }
        private bool UpdateFileLenColl(string vid, long fileLen)
        {
            foreach (DBMyDownloadInfo info in MyDownInfos)
            {
                if (info.VID == vid)
                {
                    info.FileLen = fileLen;
                    return true;
                }
            }
            return false;
        }

        private bool UpdateFileNameColl(string vid, string localPathName)
        {
            foreach (DBMyDownloadInfo info in MyDownInfos)
            {
                if (info.VID == vid)
                {
                    info.LocalPathName = localPathName;
                    return true;
                }
            }
            return false;
        }
        public void UpdateRecord(string vid, int status, long progress)
        {
            ThreadPool.QueueUserWorkItem((o) =>
            {
                lock (DBMyDownloadFactory.syncObj)
                {
                    var query = from toUpdate in _dataContext.Table_DBMyDownloadInfo
                                where toUpdate.VID == vid
                                select toUpdate;
                    int i = 0;
                    foreach (var u in query)
                    {
                        if (status != -1)
                            u.Status = status;
                        if (progress != -1)
                            u.Progress = progress;
                        i++;
                    }
                    _dataContext.SubmitChanges();
                    if (i > 0)
                    {
                        UpdateCollection(vid, status, progress);
                    }
                }
            });
        }

        private void InsertIntoCollection(string vid, string titleImgUrl, string title, string fileUrl, string localPathName, long progress, long fileLen, int status, int rate)
        {
            DBMyDownloadInfo dbMyDownInfo = new DBMyDownloadInfo(vid, titleImgUrl, title, fileUrl, localPathName, progress, fileLen, status, rate);
            MyDownInfos.Add(dbMyDownInfo);
        }
        private void DeleteFromCollection(string vid)
        {
            DBMyDownloadInfo downInfo = null;
            foreach (DBMyDownloadInfo info in MyDownInfos)
            {
                if (info.VID == vid)
                {
                    downInfo = info;
                    break;
                }
            }
            if (downInfo != null)
                MyDownInfos.Remove(downInfo);
        }

        private void UpdateCollection(string vid, string titleImgUrl, string title, string fileUrl, string localPathName, long progress, long fileLen, int status, int rate)
        {
            foreach (DBMyDownloadInfo info in MyDownInfos)
            {
                if (info.VID == vid)
                {
                    info.TitleImgUrl = titleImgUrl;
                    info.Title = title;
                    info.FileUrl = fileUrl;
                    info.LocalPathName = localPathName;
                    info.Progress = progress;
                    info.FileLen = fileLen;
                    info.Status = status;
                    info.Rate = rate;
                    break;
                }
            }
        }

        private void UpdateCollection(string vid, int status, long progress)
        {
            foreach (DBMyDownloadInfo info in MyDownInfos)
            {
                if (info.VID == vid)
                {
                    if (status != -1)
                        info.Status = status;
                    if (progress != -1)
                        info.Progress = progress;
                    break;
                }
            }
        }
        public ObservableCollection<DBMyDownloadInfo> GetAllRecords()
        {
            if (MyDownInfos == null)
            {
                ObservableCollection<DBMyDownloadInfo> dbMyDownloadInfo = new ObservableCollection<DBMyDownloadInfo>();
                IEnumerator<DBMyDownloadInfo> iEnumerator = _dataContext.Table_DBMyDownloadInfo.GetEnumerator();
                while (iEnumerator.MoveNext())
                {
                    dbMyDownloadInfo.Add(iEnumerator.Current);
                }
                return dbMyDownloadInfo;
            }
            else
                return MyDownInfos;
        }
        public bool IsExistVid(string Vid)
        {
            lock (syncObj)
            {
                foreach (DBMyDownloadInfo info in MyDownInfos)
                {
                    if (info.VID == Vid)
                        return true;
                }
                return false;
            }
        }

        public bool IsExistFile(string fileName)
        {
            string pathName = System.IO.Path.Combine("/PersonalCenter/MyDownload", fileName);
            foreach (DBMyDownloadInfo info in MyDownInfos)
            {
                if (info.LocalPathName == pathName)
                {
                    return true;
                }
            }
            return false;
        }
    }
}
