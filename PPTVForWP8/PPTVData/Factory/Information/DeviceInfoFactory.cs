using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Microsoft.Phone.Info;

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
                    object bytes = null;
                    if (DeviceExtendedProperties.TryGetValue("DeviceUniqueId", out bytes))
                    {
                        var buffer = (byte[])bytes;
                        var builder = new StringBuilder(buffer.Length * 2);
                        foreach (var b in buffer)
                        {
                            builder.Append(b.ToString("X"));
                        }
                        id = builder.ToString();
                    }
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
