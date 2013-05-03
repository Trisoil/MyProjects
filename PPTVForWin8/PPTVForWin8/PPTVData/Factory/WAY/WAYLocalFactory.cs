using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Factory.WAY
{
    using PPTVData.Entity.WAY;

    public class WAYLocalFactory : FileFactoryBase<WAYLocalFactory, WAYPostInfo>
    {
        private WAYPostFactory _postFactory;
        private Dictionary<int, int> _viewDownloadList;

        public WAYLocalFactory()
        {
            _postFactory = new WAYPostFactory();
            _viewDownloadList = new Dictionary<int, int>(10);
        }

        /// <summary>
        /// 重载初始化方法，如果有可提交记录先提交。
        /// </summary>
        /// <returns></returns>
        public override async Task Init()
        {
            await base.Init();

            if (DataInfos.Count > 0 
                && DataInfos[0].IsWatch)
            {
                _postFactory.DownLoadDatas();
                _viewDownloadList.Clear();
            }

            RemoveAll();
            InsertRecord(new WAYPostInfo());
            DataInfos[0].DeviceCode = DeviceInfoFactory.Instance.DataInfos[0].DeviceId;
        }

        public override string FileName
        {
            get { return "waylocalinfos.db"; }
        }

        public void SetUserName(string userName)
        {
            if (DataInfos.Count > 0)
            {
                DataInfos[0].UserName = userName;
                SubmitChange();
            }
        }

        public void AddVodWatchTime(int type, int channelId, int watchTime, int allTime, bool isDownload)
        {
            if (DataInfos.Count <= 0) return;

            if (!DataInfos[0].IsWatch)
                DataInfos[0].IsWatch = true;

            switch (type)
            {
                case 2:
                    DataInfos[0].DTime += watchTime;
                    DataInfos[0].DATime += allTime;
                    DataInfos[0].DVV++;
                    break;
                case 3:
                    DataInfos[0].CTime += watchTime;
                    DataInfos[0].CATime += allTime;
                    DataInfos[0].CVV++;
                    break;
                case 4:
                    DataInfos[0].STime += watchTime;
                     DataInfos[0].SATime += allTime;
                    DataInfos[0].SVV++;
                    break;
                default:
                    DataInfos[0].MTime += watchTime;
                    DataInfos[0].MATime += allTime;
                    DataInfos[0].MVV++;
                    break;
            }

            if (WAYGetFactory.WayGetInfo.DayType == 1)
                DataInfos[0].HolidayVV++;

            if (isDownload 
                && !_viewDownloadList.ContainsKey(channelId))
            {
                _viewDownloadList[channelId] = channelId;
                DataInfos[0].ViewDownloadNum++;
            }

            if (DateTime.Now.Hour >= WAYGetFactory.WayGetInfo.NightBegin &&
                DateTime.Now.Hour <= WAYGetFactory.WayGetInfo.NightEnd)
                DataInfos[0].NightVV++;
            SubmitChange();
        }

        public void AddLiveTime(int wathcTime)
        {
            if (DataInfos.Count > 0)
            {
                DataInfos[0].MaxLiveTime += wathcTime;
                SubmitChange();
            }
        }

        public void AddShareCount()
        {
            if (DataInfos.Count > 0)
            {
                if (!DataInfos[0].IsWatch)
                    DataInfos[0].IsWatch = true;

                DataInfos[0].ShareNum++;
                SubmitChange();
            }
        }

        public void AddDownloadCount()
        {
            if (DataInfos.Count > 0)
            {
                if (!DataInfos[0].IsWatch)
                    DataInfos[0].IsWatch = true;

                DataInfos[0].DownloadNum++;
                SubmitChange();
            }
        }
    }
}
