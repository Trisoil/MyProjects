using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.IO;
using System.Net;
using System.Threading.Tasks;
using System.Runtime.Serialization.Json;

namespace PPTVData.Factory.WAY
{
    using PPTVData.Utils;
    using PPTVData.Entity.WAY;

    public class WAYPostFactory : WAYFactoryBase
    {
        protected override string CreateUri(params object[] paras)
        {
            return string.Format("{0}post/winrt", EpgUtils.WAYHost);
        }

        protected override HttpMethod Method
        {
            get
            {
                return HttpMethod.Post;
            }
        }

        protected override string PostArgs
        {
            get
            {
                var bytes = CreateContent();
                if (bytes == null)
                {
                    StatusCode = HttpStatusCode.InternalServerError;
                    return string.Empty;
                }
                _index = new Random().Next(1, 10);
                var data = Get3DES(bytes, _index);
                var time = Convert.ToInt64(DateTime.UtcNow.Subtract(DateTime.Parse("1970,1,1")).TotalMilliseconds);//(long)DateTime.UtcNow.Subtract(_getTime).TotalMilliseconds + _getTimeMillSeconds;

                var list = new List<APIParameter>(4);
                list.Add(new APIParameter("data", WebUtility.UrlEncode(data)));
                list.Add(new APIParameter("time", time.ToString()));
                list.Add(new APIParameter("md5", MD5Helper.GetMd5String(string.Format("{0}{1}{2}", data, time, _md5Key))));
                list.Add(new APIParameter("index", _index.ToString()));
                return HttpUtils.GetQueryFromParas(list);
            }
        }

        private byte[] CreateContent()
        {
            byte[] result = null;
            var json = new DataContractJsonSerializer(typeof(ObservableCollection<WAYPostInfo>));

            using (var stream = new MemoryStream())
            {
                json.WriteObject(stream, WAYLocalFactory.Instance.DataInfos);

                stream.Seek(0, SeekOrigin.Begin);
                var bytes = new byte[stream.Length];
                stream.Read(bytes, 0, bytes.Length);

                result = bytes;
            }

            return result;
        }
    }
}
