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
using PPTVData.Entity;
using System.Linq;
namespace PPTVData.Factory
{
    
    public class LocalPlayFactory
    {
        public PPTVDataContext _dataContext;
        public LocalPlayFactory()
        {
            if (_dataContext == null)
                _dataContext = new PPTVDataContext();
        }
        public bool DeleteRecord(string vid)
        {
            lock (this)
            {
                try
                {
                    var query = from toUpdate in _dataContext.Table_LocalPlay
                                where toUpdate.VID == vid
                                select toUpdate;
                    _dataContext.Table_LocalPlay.DeleteAllOnSubmit<LocalPlay>(query);
                    _dataContext.SubmitChanges();
                    return true;
                }
                catch
                {
                    return false;
                }
            }
        }
        public bool InsertRecord(string vid, long position)
        {
            lock (this)
            {
                try
                {
                    LocalPlay localPlay = new LocalPlay(vid, position);
                    _dataContext.Table_LocalPlay.InsertOnSubmit(localPlay);
                    _dataContext.SubmitChanges();
                    return true;
                }
                catch (Exception exception)
                {
                    string message = exception.Message;
                    return false;
                }
            }
        }
        public bool UpdateRecord(string vid, long position)
        {
            lock (this)
            {
                var query = from toUpdate in _dataContext.Table_LocalPlay
                            where toUpdate.VID == vid
                            select toUpdate;
                int i = 0;
                foreach (var u in query)
                {
                    u.Position = position;
                    i++;
                }
                _dataContext.SubmitChanges();
                if (i > 0)
                {

                    return true;
                }
                else
                    return false;
            }
        }
        public long GetPosition(string vid)
        {
            var query = from toUpdate in _dataContext.Table_LocalPlay
                        where toUpdate.VID == vid
                        select toUpdate;
            if (query.Count<LocalPlay>() > 0)
            {
                return query.First<LocalPlay>().Position;
            }
            else
                return 0;
        }

    }
}
