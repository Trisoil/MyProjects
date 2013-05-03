using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.System.Profile;
using Windows.Storage.Streams;

namespace PPTVData.Factory
{
    using PPTVData.Entity;

    public class DeviceInfoFactory  : FileFactoryBase<DeviceInfoFactory,DeviceInfo>
    {
        public override string FileName
        {
            get { return "DeviceInfo.db"; }
        }

        protected override async Task<ObservableCollection<DeviceInfo>> LoadFromLocal()
        {
            var result = await base.LoadFromLocal();
            if (result.Count == 0)
            {
                string id = string.Empty;
                try
                {
                    var token = HardwareIdentification.GetPackageSpecificToken(null);
                    var buffer = token.Id;
                    int length = (int)buffer.Length;

                    using (var reader = DataReader.FromBuffer(buffer))
                    {
                        var bytes = new byte[length];
                        reader.ReadBytes(bytes);

                        var builder = new StringBuilder(length);
                        foreach (var b in bytes)
                        {
                            builder.Append(b.ToString("X"));
                        }
                        id = builder.ToString();
                    }

                    //var adapter = NetworkInformation.GetInternetConnectionProfile().NetworkAdapter;
                    //id = adapter.NetworkAdapterId.ToString();
                }
                catch
                {
                    id = Guid.NewGuid().ToString();
                }
                var info = new DeviceInfo()
                {
                    UsreId = id,
                    DeviceId = id
                };
                result.Add(info);
                DataInfos = result;
                SubmitChange();
            }
            return result;
        }
    }
}
