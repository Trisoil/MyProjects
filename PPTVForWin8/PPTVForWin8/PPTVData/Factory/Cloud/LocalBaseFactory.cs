using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Threading.Tasks;

using Windows.UI.Core;
using Windows.UI.Xaml;

namespace PPTVData.Factory.Cloud
{
    using PPTVData.Utils;
    using PPTVData.Entity;
    using PPTVData.Entity.Cloud;

    public abstract class LocalBaseFactory<T> : FileFactoryBase<T, CloudDataInfo>
        where T : new()
    {
        private static char[] _splitSeparator = new char[] { '第', '集' };
        private static int Device = 128;
        private object _lock = new object();
        private int _updateImageCount;
        private CoreDispatcher _dispatcher;
        private CloudPostFactory _postFactory;
        private List<CloudDataInfo> _updateImageList;

        public Action LocalChangeAction;

        public LocalBaseFactory()
        {
            _dispatcher = Window.Current.CoreWindow.Dispatcher;
            _postFactory = new CloudPostFactory();
            _updateImageList = new List<CloudDataInfo>(MaxCount);
        }

        public abstract override string FileName { get; }

        public abstract string CloudTypeName { get; }

        /// <summary>
        /// 最大记录个数
        /// </summary>
        public virtual int MaxCount
        {
            get { return 30; }
        }

        /// <summary>
        /// 根据频道ID返回一条记录，如果不存在则返回null
        /// </summary>
        /// <param name="vid"></param>
        /// <returns></returns>
        public async Task<CloudDataInfo> GetRecord(int vid)
        {
            return await Task.Run(() => { return DataInfos.FirstOrDefault(v => v.Id == vid); });
        }

        /// <summary>
        /// 批量插入云端数据，并更新云端
        /// </summary>
        /// <param name="clouds"></param>
        public async void InsertCloudRecords(List<CloudDataInfo> clouds)
        {
            var removeCount = clouds.Count - MaxCount;
            if (removeCount > 0)
            {
                for (var i = 0; i < removeCount; i++)
                    _postFactory.AddDeleteInfo(clouds[i]);
                clouds = clouds.Skip(removeCount).ToList();
            }
            _updateImageCount = await Task.Run(() => { return clouds.Count(v => DataInfos.IndexOf(v) == -1); });
            var allCount = clouds.Count;
            for (var i = allCount - 1; i >= 0; i--)
            {
                InsertCloudRecord(clouds[i]);
            }
            PostChange();
            SubmitChange();

            if (LocalChangeAction != null)
                LocalChangeAction();
        }

        /// <summary>
        /// 插入一条云端数据，如果存在则更新
        /// </summary>
        /// <param name="record"></param>
        /// <returns></returns>
        private void InsertCloudRecord(CloudDataInfo record)
        {
            var index = DataInfos.IndexOf(record);

            if (record.Id <= 0)
            {
                if (index == -1)
                    _updateImageCount--;
                record.DataType = CloudInfoEnum.Delete;
                return;
            }

            if (index != -1)
            {
                var local = DataInfos[index];
                if (local.ModifyTime > record.ModifyTime)
                {
                    local.DataType = CloudInfoEnum.Update;
                    UpdateDeviceHistory(record);
                    local.UUID = record.UUID;
                }
                else
                {
                    UpdateProgramIndex(record);
                    record.ImageUri = local.ImageUri;
                    local = record;
                    local.DataType = CloudInfoEnum.Normal;
                    DataInfos[index] = local;
                }
            }
            else
            {
                record.DataType = CloudInfoEnum.Normal;
                UpdateProgramIndex(record);
                UpdateImageUri(record);
            }
        }

        /// <summary>
        /// 插入一条本地数据，如果存在则更新
        /// </summary>
        /// <param name="record"></param>
        /// <returns></returns>
        public void InsertLocalRecord(CloudDataInfo record)
        {
            record.Device = Device;
            record.ModifyTime = DateTime.Now.Subtract(DateTime.Parse("1601,1,1")).Ticks + CloudFactoryBase.TimeInterval;

            var index = DataInfos.IndexOf(record);
            if (index != -1)
            {
                var local = DataInfos[index];
                record.UUID = local.UUID;
                UpdateDeviceHistory(local);
                record.DeviceHistory = local.DeviceHistory;
                local = record;
                local.DataType = CloudInfoEnum.Update;
                DataInfos.RemoveAt(index);
                DataInfos.Insert(0, local);
            }
            else
            {
                record.UUID = Guid.NewGuid().ToString();
                if (DataInfos.Count >= MaxCount)
                {
                    var removeIndex = DataInfos.Count - 1;
                    var removeInfo = DataInfos[removeIndex];
                    removeInfo.DataType = CloudInfoEnum.Delete;
                    _postFactory.AddDeleteInfo(removeInfo);
                    DataInfos.RemoveAt(removeIndex);
                }
                record.DataType = CloudInfoEnum.Add;
                UpdateDeviceHistory(record);
                DataInfos.Insert(0, record);
            }
            PostChange();
            SubmitChange();

            if (LocalChangeAction != null)
                LocalChangeAction();
        }

        /// <summary>
        /// 根据ID删除一条记录，并POST到云端
        /// </summary>
        /// <param name="vid"></param>
        /// <returns></returns>
        public void DeleteRecord(int vid)
        {
            var local = DataInfos.FirstOrDefault(v => v.Id == vid);
            if (local != null)
            {
                local.DataType = CloudInfoEnum.Delete;
                _postFactory.AddDeleteInfo(local);
                PostChange();
                base.DeleteRecord(local);

                if (LocalChangeAction != null)
                    LocalChangeAction();
            }
        }

        /// <summary>
        /// 删除所有记录，并POST到云端
        /// </summary>
        /// <returns></returns>
        public override void RemoveAll()
        {
            foreach (var data in DataInfos)
            {
                data.DataType = CloudInfoEnum.Delete;
                _postFactory.AddDeleteInfo(data);
            }
            PostChange();
            base.RemoveAll();

            if (LocalChangeAction != null)
                LocalChangeAction();
        }

        /// <summary>
        /// 提交所做的修改
        /// </summary>
        private void PostChange()
        {
            if (PersonalFactory.Instance.Logined)
                Task.Run(() => _postFactory.DownLoadDatas(CloudTypeName));
        }

        /// <summary>
        /// 更新频道的分集索引
        /// </summary>
        /// <param name="record"></param>
        private void UpdateProgramIndex(CloudDataInfo record)
        {
            if (record.SubId != 0 && record.SubId != record.Id)
            {
                var name = record.SubName;
                var names = name.Split(_splitSeparator, StringSplitOptions.RemoveEmptyEntries);
                int result = 0;
                foreach (var n in names)
                {
                    if (int.TryParse(n, out result))
                        break;
                }
                record.ProgramIndex = result - 1;
                return;
            }
            record.ProgramIndex = -1;
        }

        /// <summary>
        /// 更新频道缩略图与评分信息
        /// </summary>
        /// <param name="record"></param>
        private void UpdateImageUri(CloudDataInfo record)
        {
            var vid = record.SubId == 0 ? record.Id : record.SubId;
            var uri = string.Format("{0}detail.api?ver=2&auth={1}&platform={2}&vid={3}", EpgUtils.VodHost, EpgUtils.Auth, EpgUtils.PlatformName, vid);
            HttpAsyncUtils.HttpGet(uri, null, null, response =>
            {
                try
                {
                    using (var reader = XmlReader.Create(new StringReader(response)))
                    {
                        if (reader.ReadToFollowing("imgurl"))
                        {
                            record.ImageUri = reader.ReadElementContentAsString();
                        }
                        if (reader.ReadToFollowing("mark"))
                        {
                            record.Mark = reader.ReadElementContentAsDouble();
                        }
                    }
                    lock (_lock)
                    {
                        _updateImageList.Add(record);
                        if (_updateImageList.Count >= _updateImageCount)
                        {
                            var list = _updateImageList.OrderByDescending(v => v.ModifyTime);
                            _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                                {
                                    try
                                    {
                                        foreach (var v in list)
                                        {
                                            if (DataInfos.Count >= MaxCount)
                                                _postFactory.AddDeleteInfo(v);
                                            else
                                                DataInfos.Add(v);
                                        }
                                        SubmitChange();
                                        if (LocalChangeAction != null)
                                            LocalChangeAction();
                                    }
                                    catch { }
                                });
                        }
                    }
                }
                catch { }
            });
        }

        /// <summary>
        /// 更新DeviceHistory字段，逻辑或当前设备
        /// </summary>
        /// <param name="record"></param>
        private void UpdateDeviceHistory(CloudDataInfo record)
        {
            record.DeviceHistory = record.DeviceHistory | Device;
        }
    }
}
